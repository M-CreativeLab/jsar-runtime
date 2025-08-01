#include "environment_renderer.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <ios>

#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

namespace jsar::example
{

  using namespace std;

  // Vertex shader for skybox rendering
  const char *EnvironmentRenderer::vertexShaderSource_ = R"(
#version 330 core
layout (location = 0) in vec3 position;

out vec3 texCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    texCoords = position;
    
    // Remove translation from view matrix for skybox
    mat4 rotView = mat4(mat3(view));
    vec4 pos = projection * rotView * vec4(position, 1.0);
    
    // Set z to w so that after perspective division, z will be 1.0 (maximum depth)
    gl_Position = pos.xyww;
}
)";

  // Fragment shader for skybox rendering
  const char *EnvironmentRenderer::fragmentShaderSource_ = R"(
#version 330 core
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, texCoords);
}
)";

  EnvironmentRenderer::EnvironmentRenderer()
      : initialized_(false)
      , enabled_(true)
      , hasCubeMapTexture_(false)
      , shaderProgram_(0)
      , vertexShader_(0)
      , fragmentShader_(0)
      , vao_(0)
      , vbo_(0)
      , cubeMapTexture_(0)
      , viewMatrixLocation_(-1)
      , projectionMatrixLocation_(-1)
      , cubeMapLocation_(-1)
  {
  }

  EnvironmentRenderer::~EnvironmentRenderer()
  {
    shutdown();
  }

  bool EnvironmentRenderer::initialize(const string &cubeMapPath)
  {
    if (initialized_)
    {
      return true;
    }

    cout << "Initializing Environment Renderer..." << endl;

    // Create shaders
    createShaders();
    if (shaderProgram_ == 0)
    {
      cerr << "Failed to create environment shaders" << endl;
      return false;
    }

    // Create cube geometry
    createCubeGeometry();
    if (vao_ == 0)
    {
      cerr << "Failed to create cube geometry" << endl;
      return false;
    }

    // Load cube map texture
    if (!cubeMapPath.empty())
    {
      if (!loadCubeMap(cubeMapPath))
      {
        cout << "Failed to load cube map from " << cubeMapPath << ", using procedural environment" << endl;
        createProceduralCubeMap();
      }
    }
    else
    {
      createProceduralCubeMap();
    }

    // Get uniform locations
    viewMatrixLocation_ = glGetUniformLocation(shaderProgram_, "view");
    projectionMatrixLocation_ = glGetUniformLocation(shaderProgram_, "projection");
    cubeMapLocation_ = glGetUniformLocation(shaderProgram_, "skybox");

    initialized_ = true;
    cout << "Environment Renderer initialized successfully" << endl;
    return true;
  }

  void EnvironmentRenderer::render(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
  {
    if (!initialized_ || !enabled_ || !hasCubeMapTexture_)
    {
      return;
    }

    // Disable depth writing (but keep depth testing to ensure skybox is behind everything)
    glDepthMask(GL_FALSE);

    glUseProgram(shaderProgram_);

    // Set uniforms
    glUniformMatrix4fv(viewMatrixLocation_, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(projectionMatrixLocation_, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Bind cube map texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture_);
    glUniform1i(cubeMapLocation_, 0);

    // Render cube
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Re-enable depth writing
    glDepthMask(GL_TRUE);

    glUseProgram(0);
  }

  void EnvironmentRenderer::shutdown()
  {
    destroyResources();
    initialized_ = false;
  }

  bool EnvironmentRenderer::loadCubeMap(const string &filePath)
  {
    // Determine file type by extension
    string extension = filePath.substr(filePath.find_last_of(".") + 1);
    transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    if (extension == "hdr")
    {
      return loadHDRCubeMap(filePath);
    }
    else if (extension == "dds")
    {
      return loadDDSCubeMap(filePath);
    }
    else
    {
      cerr << "Unsupported cube map format: " << extension << endl;
      return false;
    }
  }

  void EnvironmentRenderer::createShaders()
  {
    // Compile vertex shader
    vertexShader_ = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader_, 1, &vertexShaderSource_, nullptr);
    glCompileShader(vertexShader_);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader_, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(vertexShader_, 512, nullptr, infoLog);
      cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
           << infoLog << endl;
      return;
    }

    // Compile fragment shader
    fragmentShader_ = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader_, 1, &fragmentShaderSource_, nullptr);
    glCompileShader(fragmentShader_);

    glGetShaderiv(fragmentShader_, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(fragmentShader_, 512, nullptr, infoLog);
      cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
           << infoLog << endl;
      return;
    }

    // Link shader program
    shaderProgram_ = glCreateProgram();
    glAttachShader(shaderProgram_, vertexShader_);
    glAttachShader(shaderProgram_, fragmentShader_);
    glLinkProgram(shaderProgram_);

    glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(shaderProgram_, 512, nullptr, infoLog);
      cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
           << infoLog << endl;
      shaderProgram_ = 0;
      return;
    }

    // Clean up individual shaders as they're now linked into the program
    glDeleteShader(vertexShader_);
    glDeleteShader(fragmentShader_);
  }

  void EnvironmentRenderer::createCubeGeometry()
  {
    // Cube vertices for skybox (positions only, no normals or texture coordinates needed)
    float skyboxVertices[] = {
      // positions
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,

      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,

      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,

      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,

      -1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      -1.0f,

      -1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f};

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    glBindVertexArray(0);
  }

  void EnvironmentRenderer::createProceduralCubeMap()
  {
    // Create a simple procedural cubemap with sky gradient
    glGenTextures(1, &cubeMapTexture_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture_);

    int width = 256, height = 256;
    vector<unsigned char> data(width * height * 3);

    // Define colors for different directions
    glm::vec3 skyColor(0.5f, 0.7f, 1.0f);     // Light blue
    glm::vec3 horizonColor(1.0f, 0.9f, 0.7f); // Warm horizon
    glm::vec3 groundColor(0.3f, 0.2f, 0.1f);  // Brown ground

    // Generate each face of the cubemap
    for (int face = 0; face < 6; face++)
    {
      for (int y = 0; y < height; y++)
      {
        for (int x = 0; x < width; x++)
        {
          // Convert to normalized cube coordinates
          float u = (2.0f * x / width) - 1.0f;
          float v = (2.0f * y / height) - 1.0f;

          glm::vec3 dir;
          switch (face)
          {
          case 0:
            dir = glm::vec3(1.0f, -v, -u);
            break; // +X
          case 1:
            dir = glm::vec3(-1.0f, -v, u);
            break; // -X
          case 2:
            dir = glm::vec3(u, 1.0f, v);
            break; // +Y
          case 3:
            dir = glm::vec3(u, -1.0f, -v);
            break; // -Y
          case 4:
            dir = glm::vec3(u, -v, 1.0f);
            break; // +Z
          case 5:
            dir = glm::vec3(-u, -v, -1.0f);
            break; // -Z
          }
          dir = glm::normalize(dir);

          // Create gradient based on Y component (up/down)
          float t = dir.y;
          glm::vec3 color;
          if (t > 0.0f)
          {
            // Sky
            color = glm::mix(horizonColor, skyColor, t);
          }
          else
          {
            // Ground
            color = glm::mix(horizonColor, groundColor, -t);
          }

          int index = (y * width + x) * 3;
          data[index] = (unsigned char)(color.r * 255);
          data[index + 1] = (unsigned char)(color.g * 255);
          data[index + 2] = (unsigned char)(color.b * 255);
        }
      }

      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    hasCubeMapTexture_ = true;

    cout << "Created procedural cube map environment" << endl;
  }

  bool EnvironmentRenderer::loadHDRCubeMap(const string &filePath)
  {
    // For now, fall back to procedural. HDR loading would require additional libraries like stb_image
    cout << "HDR cube map loading not yet implemented, using procedural environment" << endl;
    createProceduralCubeMap();
    return true;
  }

  bool EnvironmentRenderer::loadDDSCubeMap(const string &filePath)
  {
    cout << "Loading DDS cube map: " << filePath << endl;
    
    // Check if file exists
    ifstream file(filePath, ios::binary);
    if (!file.is_open())
    {
      cout << "Failed to open DDS file: " << filePath << ", falling back to procedural environment" << endl;
      createProceduralCubeMap();
      return true;
    }
    
    // DDS file format structures
    struct DDSPixelFormat {
      uint32_t dwSize;
      uint32_t dwFlags;
      uint32_t dwFourCC;
      uint32_t dwRGBBitCount;
      uint32_t dwRBitMask;
      uint32_t dwGBitMask;
      uint32_t dwBBitMask;
      uint32_t dwABitMask;
    };
    
    struct DDSHeader {
      uint32_t dwSize;
      uint32_t dwFlags;
      uint32_t dwHeight;
      uint32_t dwWidth;
      uint32_t dwPitchOrLinearSize;
      uint32_t dwDepth;
      uint32_t dwMipMapCount;
      uint32_t dwReserved1[11];
      DDSPixelFormat ddspf;
      uint32_t dwCaps;
      uint32_t dwCaps2;
      uint32_t dwCaps3;
      uint32_t dwCaps4;
      uint32_t dwReserved2;
    };
    
    // Read and validate DDS magic number
    uint32_t magic;
    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    if (magic != 0x20534444) // "DDS "
    {
      cout << "Invalid DDS magic number in file: " << filePath << ", falling back to procedural environment" << endl;
      file.close();
      createProceduralCubeMap();
      return true;
    }
    
    // Read DDS header
    DDSHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    // Validate that this is a cube map
    const uint32_t DDSCAPS2_CUBEMAP = 0x200;
    const uint32_t DDSCAPS2_CUBEMAP_POSITIVEX = 0x400;
    const uint32_t DDSCAPS2_CUBEMAP_NEGATIVEX = 0x800;
    const uint32_t DDSCAPS2_CUBEMAP_POSITIVEY = 0x1000;
    const uint32_t DDSCAPS2_CUBEMAP_NEGATIVEY = 0x2000;
    const uint32_t DDSCAPS2_CUBEMAP_POSITIVEZ = 0x4000;
    const uint32_t DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x8000;
    const uint32_t DDSCAPS2_CUBEMAP_ALLFACES = (DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX |
                                                DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGATIVEY |
                                                DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEZ);
    
    if (!(header.dwCaps2 & DDSCAPS2_CUBEMAP) || (header.dwCaps2 & DDSCAPS2_CUBEMAP_ALLFACES) != DDSCAPS2_CUBEMAP_ALLFACES)
    {
      cout << "DDS file is not a complete cube map: " << filePath << ", falling back to procedural environment" << endl;
      file.close();
      createProceduralCubeMap();
      return true;
    }
    
    // Determine format
    GLenum format = GL_RGBA;
    GLenum internalFormat = GL_RGBA8;
    GLenum type = GL_UNSIGNED_BYTE;
    bool compressed = false;
    
    // Check for common DDS formats
    const uint32_t DDSPF_FOURCC = 0x4;
    const uint32_t DDSPF_RGB = 0x40;
    const uint32_t DDSPF_RGBA = 0x41;
    
    if (header.ddspf.dwFlags & DDSPF_FOURCC)
    {
      // Handle compressed formats
      switch (header.ddspf.dwFourCC)
      {
        case 0x31545844: // DXT1
          format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
          internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
          compressed = true;
          break;
        case 0x33545844: // DXT3
          format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
          internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
          compressed = true;
          break;
        case 0x35545844: // DXT5
          format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
          internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
          compressed = true;
          break;
        default:
          cout << "Unsupported DDS compression format: " << hex << header.ddspf.dwFourCC << dec 
               << " in file: " << filePath << ", falling back to procedural environment" << endl;
          file.close();
          createProceduralCubeMap();
          return true;
      }
    }
    else if (header.ddspf.dwFlags & DDSPF_RGB)
    {
      // Uncompressed RGB/RGBA formats
      if (header.ddspf.dwRGBBitCount == 24)
      {
        format = GL_RGB;
        internalFormat = GL_RGB8;
        type = GL_UNSIGNED_BYTE;
      }
      else if (header.ddspf.dwRGBBitCount == 32)
      {
        format = GL_RGBA;
        internalFormat = GL_RGBA8;
        type = GL_UNSIGNED_BYTE;
      }
      else
      {
        cout << "Unsupported DDS bit count: " << header.ddspf.dwRGBBitCount 
             << " in file: " << filePath << ", falling back to procedural environment" << endl;
        file.close();
        createProceduralCubeMap();
        return true;
      }
    }
    else
    {
      cout << "Unsupported DDS pixel format in file: " << filePath << ", falling back to procedural environment" << endl;
      file.close();
      createProceduralCubeMap();
      return true;
    }
    
    // Create OpenGL cube map texture
    if (cubeMapTexture_ != 0)
    {
      glDeleteTextures(1, &cubeMapTexture_);
    }
    
    glGenTextures(1, &cubeMapTexture_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture_);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    // Calculate face data size
    uint32_t faceSize;
    if (compressed)
    {
      // For compressed formats, calculate size based on compression ratio
      uint32_t blockSize = (header.ddspf.dwFourCC == 0x31545844) ? 8 : 16; // DXT1 = 8 bytes/block, DXT3/5 = 16 bytes/block
      uint32_t width = header.dwWidth;
      uint32_t height = header.dwHeight;
      faceSize = max(1u, (width + 3) / 4) * max(1u, (height + 3) / 4) * blockSize;
    }
    else
    {
      faceSize = header.dwWidth * header.dwHeight * (header.ddspf.dwRGBBitCount / 8);
    }
    
    // Read and upload each face
    GLenum faces[6] = {
      GL_TEXTURE_CUBE_MAP_POSITIVE_X, // Right
      GL_TEXTURE_CUBE_MAP_NEGATIVE_X, // Left  
      GL_TEXTURE_CUBE_MAP_POSITIVE_Y, // Top
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, // Bottom
      GL_TEXTURE_CUBE_MAP_POSITIVE_Z, // Front
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Z  // Back
    };
    
    vector<uint8_t> faceData(faceSize);
    
    for (int i = 0; i < 6; i++)
    {
      file.read(reinterpret_cast<char*>(faceData.data()), faceSize);
      if (file.fail())
      {
        cout << "Failed to read face " << i << " from DDS file: " << filePath << ", falling back to procedural environment" << endl;
        file.close();
        glDeleteTextures(1, &cubeMapTexture_);
        cubeMapTexture_ = 0;
        createProceduralCubeMap();
        return true;
      }
      
      if (compressed)
      {
        glCompressedTexImage2D(faces[i], 0, internalFormat, header.dwWidth, header.dwHeight, 0, faceSize, faceData.data());
      }
      else
      {
        glTexImage2D(faces[i], 0, internalFormat, header.dwWidth, header.dwHeight, 0, format, type, faceData.data());
      }
      
      GLenum error = glGetError();
      if (error != GL_NO_ERROR)
      {
        cout << "OpenGL error uploading face " << i << " from DDS file: " << filePath << " (error: " << error << "), falling back to procedural environment" << endl;
        file.close();
        glDeleteTextures(1, &cubeMapTexture_);
        cubeMapTexture_ = 0;
        createProceduralCubeMap();
        return true;
      }
    }
    
    file.close();
    hasCubeMapTexture_ = true;
    
    cout << "Successfully loaded DDS cube map: " << filePath << " (" << header.dwWidth << "x" << header.dwHeight << ")" << endl;
    return true;
  }

  void EnvironmentRenderer::destroyResources()
  {
    if (cubeMapTexture_ != 0)
    {
      glDeleteTextures(1, &cubeMapTexture_);
      cubeMapTexture_ = 0;
    }

    if (vao_ != 0)
    {
      glDeleteVertexArrays(1, &vao_);
      vao_ = 0;
    }

    if (vbo_ != 0)
    {
      glDeleteBuffers(1, &vbo_);
      vbo_ = 0;
    }

    if (shaderProgram_ != 0)
    {
      glDeleteProgram(shaderProgram_);
      shaderProgram_ = 0;
    }

    hasCubeMapTexture_ = false;
  }

} // namespace jsar::example