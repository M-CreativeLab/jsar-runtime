#include "environment_renderer.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <cstring>
#include <algorithm>

#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

namespace jsar::example
{

  // Vertex shader for skybox rendering
  const char* EnvironmentRenderer::vertexShaderSource_ = R"(
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
  const char* EnvironmentRenderer::fragmentShaderSource_ = R"(
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

  bool EnvironmentRenderer::initialize(const std::string& cubeMapPath)
  {
    if (initialized_) {
      return true;
    }

    std::cout << "Initializing Environment Renderer..." << std::endl;

    // Create shaders
    createShaders();
    if (shaderProgram_ == 0) {
      std::cerr << "Failed to create environment shaders" << std::endl;
      return false;
    }

    // Create cube geometry
    createCubeGeometry();
    if (vao_ == 0) {
      std::cerr << "Failed to create cube geometry" << std::endl;
      return false;
    }

    // Load cube map texture
    if (!cubeMapPath.empty()) {
      if (!loadCubeMap(cubeMapPath)) {
        std::cout << "Failed to load cube map from " << cubeMapPath << ", using procedural environment" << std::endl;
        createProceduralCubeMap();
      }
    } else {
      createProceduralCubeMap();
    }

    // Get uniform locations
    viewMatrixLocation_ = glGetUniformLocation(shaderProgram_, "view");
    projectionMatrixLocation_ = glGetUniformLocation(shaderProgram_, "projection");
    cubeMapLocation_ = glGetUniformLocation(shaderProgram_, "skybox");

    initialized_ = true;
    std::cout << "Environment Renderer initialized successfully" << std::endl;
    return true;
  }

  void EnvironmentRenderer::render(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
  {
    if (!initialized_ || !enabled_ || !hasCubeMapTexture_) {
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

  bool EnvironmentRenderer::loadCubeMap(const std::string& filePath)
  {
    // Determine file type by extension
    std::string extension = filePath.substr(filePath.find_last_of(".") + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "hdr") {
      return loadHDRCubeMap(filePath);
    } else if (extension == "dds") {
      return loadDDSCubeMap(filePath);
    } else {
      std::cerr << "Unsupported cube map format: " << extension << std::endl;
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
    if (!success) {
      glGetShaderInfoLog(vertexShader_, 512, nullptr, infoLog);
      std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
      return;
    }

    // Compile fragment shader
    fragmentShader_ = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader_, 1, &fragmentShaderSource_, nullptr);
    glCompileShader(fragmentShader_);
    
    glGetShaderiv(fragmentShader_, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(fragmentShader_, 512, nullptr, infoLog);
      std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
      return;
    }

    // Link shader program
    shaderProgram_ = glCreateProgram();
    glAttachShader(shaderProgram_, vertexShader_);
    glAttachShader(shaderProgram_, fragmentShader_);
    glLinkProgram(shaderProgram_);
    
    glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shaderProgram_, 512, nullptr, infoLog);
      std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
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
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
  }

  void EnvironmentRenderer::createProceduralCubeMap()
  {
    // Create a simple procedural cubemap with sky gradient
    glGenTextures(1, &cubeMapTexture_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture_);

    int width = 256, height = 256;
    std::vector<unsigned char> data(width * height * 3);

    // Define colors for different directions
    glm::vec3 skyColor(0.5f, 0.7f, 1.0f);      // Light blue
    glm::vec3 horizonColor(1.0f, 0.9f, 0.7f);  // Warm horizon
    glm::vec3 groundColor(0.3f, 0.2f, 0.1f);   // Brown ground

    // Generate each face of the cubemap
    for (int face = 0; face < 6; face++) {
      for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
          // Convert to normalized cube coordinates
          float u = (2.0f * x / width) - 1.0f;
          float v = (2.0f * y / height) - 1.0f;
          
          glm::vec3 dir;
          switch(face) {
            case 0: dir = glm::vec3(1.0f, -v, -u); break;  // +X
            case 1: dir = glm::vec3(-1.0f, -v, u); break;  // -X
            case 2: dir = glm::vec3(u, 1.0f, v); break;    // +Y
            case 3: dir = glm::vec3(u, -1.0f, -v); break;  // -Y
            case 4: dir = glm::vec3(u, -v, 1.0f); break;   // +Z
            case 5: dir = glm::vec3(-u, -v, -1.0f); break; // -Z
          }
          dir = glm::normalize(dir);
          
          // Create gradient based on Y component (up/down)
          float t = dir.y;
          glm::vec3 color;
          if (t > 0.0f) {
            // Sky
            color = glm::mix(horizonColor, skyColor, t);
          } else {
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
    
    std::cout << "Created procedural cube map environment" << std::endl;
  }

  bool EnvironmentRenderer::loadHDRCubeMap(const std::string& filePath)
  {
    // For now, fall back to procedural. HDR loading would require additional libraries like stb_image
    std::cout << "HDR cube map loading not yet implemented, using procedural environment" << std::endl;
    createProceduralCubeMap();
    return true;
  }

  bool EnvironmentRenderer::loadDDSCubeMap(const std::string& filePath)
  {
    // For now, fall back to procedural. DDS loading would require additional implementation
    std::cout << "DDS cube map loading not yet implemented, using procedural environment" << std::endl;
    createProceduralCubeMap();
    return true;
  }

  void EnvironmentRenderer::destroyResources()
  {
    if (cubeMapTexture_ != 0) {
      glDeleteTextures(1, &cubeMapTexture_);
      cubeMapTexture_ = 0;
    }
    
    if (vao_ != 0) {
      glDeleteVertexArrays(1, &vao_);
      vao_ = 0;
    }
    
    if (vbo_ != 0) {
      glDeleteBuffers(1, &vbo_);
      vbo_ = 0;
    }
    
    if (shaderProgram_ != 0) {
      glDeleteProgram(shaderProgram_);
      shaderProgram_ = 0;
    }
    
    hasCubeMapTexture_ = false;
  }

} // namespace jsar::example