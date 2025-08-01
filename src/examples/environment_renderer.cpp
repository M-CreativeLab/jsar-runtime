#include "environment_renderer.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <ios>
#include <sys/stat.h>

#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>
#include <CoreFoundation/CoreFoundation.h>
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
    // Check if path is a directory
    struct stat pathStat;
    if (stat(filePath.c_str(), &pathStat) == 0)
    {
      if (S_ISDIR(pathStat.st_mode))
      {
        // Path is a directory
        return loadDirectoryCubeMap(filePath);
      }
      else
      {
        cerr << "Path is not a directory: " << filePath << ". Only directory-based cube maps are supported." << endl;
        return false;
      }
    }
    
    cerr << "Could not access directory: " << filePath << endl;
    return false;
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

#ifdef __APPLE__
  bool EnvironmentRenderer::loadImageWithCoreGraphics(const string &filePath, vector<unsigned char> &imageData, int &width, int &height, int &channels)
  {
    // Create a CFString from the file path
    CFStringRef cfPath = CFStringCreateWithCString(kCFAllocatorDefault, filePath.c_str(), kCFStringEncodingUTF8);
    if (!cfPath)
    {
      cerr << "Failed to create CFString for path: " << filePath << endl;
      return false;
    }

    // Create a file URL
    CFURLRef fileURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, cfPath, kCFURLPOSIXPathStyle, false);
    CFRelease(cfPath);
    if (!fileURL)
    {
      cerr << "Failed to create file URL for path: " << filePath << endl;
      return false;
    }

    // Create an image source
    CGImageSourceRef imageSource = CGImageSourceCreateWithURL(fileURL, nullptr);
    CFRelease(fileURL);
    if (!imageSource)
    {
      cerr << "Failed to create image source for path: " << filePath << endl;
      return false;
    }

    // Create an image from the source
    CGImageRef image = CGImageSourceCreateImageAtIndex(imageSource, 0, nullptr);
    CFRelease(imageSource);
    if (!image)
    {
      cerr << "Failed to create image from source for path: " << filePath << endl;
      return false;
    }

    // Get image dimensions
    width = (int)CGImageGetWidth(image);
    height = (int)CGImageGetHeight(image);
    channels = 4; // We'll always convert to RGBA for simplicity

    // Allocate data for the image
    imageData.resize(width * height * channels);

    // Create a color space
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    if (!colorSpace)
    {
      cerr << "Failed to create color space" << endl;
      CGImageRelease(image);
      return false;
    }

    // Create a bitmap context
    CGContextRef context = CGBitmapContextCreate(
        imageData.data(),
        width,
        height,
        8, // bits per component
        width * channels, // bytes per row
        colorSpace,
        kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big
    );

    CGColorSpaceRelease(colorSpace);
    
    if (!context)
    {
      cerr << "Failed to create bitmap context" << endl;
      CGImageRelease(image);
      return false;
    }

    // Draw the image into the context
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), image);

    // Clean up
    CGContextRelease(context);
    CGImageRelease(image);

    cout << "Successfully loaded image: " << filePath << " (" << width << "x" << height << ")" << endl;
    return true;
  }
#endif

  void EnvironmentRenderer::createPlaceholderFace(GLenum target, int faceIndex)
  {
    // Create a simple colored face as placeholder
    vector<unsigned char> data(256 * 256 * 3);
    // Use different colors for different faces for testing
    unsigned char r = (faceIndex == 0 || faceIndex == 1) ? 255 : 64;  // Red for X faces
    unsigned char g = (faceIndex == 2 || faceIndex == 3) ? 255 : 64;  // Green for Y faces
    unsigned char b = (faceIndex == 4 || faceIndex == 5) ? 255 : 64;  // Blue for Z faces
    
    for (int j = 0; j < 256 * 256; j++)
    {
      data[j * 3] = r;
      data[j * 3 + 1] = g;
      data[j * 3 + 2] = b;
    }
    
    glTexImage2D(target, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
  }

  bool EnvironmentRenderer::loadDirectoryCubeMap(const string &directoryPath)
  {
    cout << "Loading cube map from directory: " << directoryPath << endl;
    
    // Face names and corresponding OpenGL targets
    struct FaceInfo {
      string filename;
      GLenum target;
    };
    
    FaceInfo faces[6] = {
      {"px", GL_TEXTURE_CUBE_MAP_POSITIVE_X}, // +X (right)
      {"nx", GL_TEXTURE_CUBE_MAP_NEGATIVE_X}, // -X (left)
      {"py", GL_TEXTURE_CUBE_MAP_POSITIVE_Y}, // +Y (top)
      {"ny", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y}, // -Y (bottom)
      {"pz", GL_TEXTURE_CUBE_MAP_POSITIVE_Z}, // +Z (front)
      {"nz", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z}  // -Z (back)
    };
    
    // Supported extensions in order of preference
    vector<string> extensions = {"hdr", "png", "jpg", "jpeg"};
    
    // Find files for each face
    vector<string> faceFiles(6);
    bool allFacesFound = true;
    
    for (int i = 0; i < 6; i++)
    {
      bool faceFound = false;
      for (const string& ext : extensions)
      {
        string fullPath = directoryPath + "/" + faces[i].filename + "." + ext;
        ifstream testFile(fullPath);
        if (testFile.good())
        {
          faceFiles[i] = fullPath;
          faceFound = true;
          testFile.close();
          break;
        }
        testFile.close();
      }
      
      if (!faceFound)
      {
        cout << "Could not find face file for " << faces[i].filename << " in directory: " << directoryPath << endl;
        allFacesFound = false;
      }
    }
    
    if (!allFacesFound)
    {
      cout << "Not all cube map faces found in directory: " << directoryPath << ", falling back to procedural environment" << endl;
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
    
    // Load each face
    bool success = true;
    for (int i = 0; i < 6; i++)
    {
      string extension = faceFiles[i].substr(faceFiles[i].find_last_of(".") + 1);
      transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
      
      if (extension == "hdr")
      {
        // For now, HDR loading is not implemented, so we'll create a placeholder
        cout << "HDR face loading not yet implemented for " << faceFiles[i] << ", using placeholder" << endl;
        createPlaceholderFace(faces[i].target, i);
      }
      else if (extension == "png" || extension == "jpg" || extension == "jpeg")
      {
#ifdef __APPLE__
        // Use Core Graphics to load PNG/JPG on macOS
        vector<unsigned char> imageData;
        int imgWidth, imgHeight, imgChannels;
        
        if (loadImageWithCoreGraphics(faceFiles[i], imageData, imgWidth, imgHeight, imgChannels))
        {
          // Convert RGBA to RGB if needed
          if (imgChannels == 4)
          {
            vector<unsigned char> rgbData(imgWidth * imgHeight * 3);
            for (int j = 0; j < imgWidth * imgHeight; j++)
            {
              rgbData[j * 3] = imageData[j * 4];         // R
              rgbData[j * 3 + 1] = imageData[j * 4 + 1]; // G
              rgbData[j * 3 + 2] = imageData[j * 4 + 2]; // B
              // Skip alpha channel
            }
            glTexImage2D(faces[i].target, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, rgbData.data());
          }
          else
          {
            glTexImage2D(faces[i].target, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData.data());
          }
          cout << "Loaded " << extension << " face: " << faceFiles[i] << " (" << imgWidth << "x" << imgHeight << ")" << endl;
        }
        else
        {
          cout << "Failed to load " << extension << " face: " << faceFiles[i] << ", using placeholder" << endl;
          createPlaceholderFace(faces[i].target, i);
        }
#else
        // On non-macOS platforms, PNG/JPG loading is not implemented, use placeholder
        cout << "PNG/JPG face loading not implemented on this platform for " << faceFiles[i] << ", using placeholder" << endl;
        createPlaceholderFace(faces[i].target, i);
#endif
      }
      
      GLenum error = glGetError();
      if (error != GL_NO_ERROR)
      {
        cout << "OpenGL error loading face " << faceFiles[i] << " (error: " << error << ")" << endl;
        success = false;
        break;
      }
    }
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
    if (!success)
    {
      cout << "Failed to load cube map from directory: " << directoryPath << ", falling back to procedural environment" << endl;
      glDeleteTextures(1, &cubeMapTexture_);
      cubeMapTexture_ = 0;
      createProceduralCubeMap();
      return true;
    }
    
    hasCubeMapTexture_ = true;
    cout << "Successfully loaded cube map from directory: " << directoryPath << endl;
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