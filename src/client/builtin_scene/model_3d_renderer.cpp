#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <cstring>

// WebGL/OpenGL headers
#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten/html5_webgl.h>
#else
#include <GL/gl.h>
#endif

#include <common/debug.hpp>
#include <client/per_process.hpp>

#include "./model_3d_renderer.hpp"
#include "./web_content.hpp"
#include "./materials.hpp"

namespace builtin_scene::model_renderer
{
  using namespace std;

  static const char *LOG_TAG = "Model3DRenderer";

  // WebGL vertex shader for 3D Gaussian Splatting
  static const char *VERTEX_SHADER_SOURCE = R"(
    attribute vec3 a_position;
    attribute vec3 a_color;
    attribute float a_opacity;
    attribute vec3 a_scale;
    attribute vec4 a_rotation;
    
    uniform mat4 u_mvpMatrix;
    uniform mat4 u_viewMatrix;
    
    varying vec3 v_color;
    varying float v_opacity;
    varying vec2 v_texCoord;
    
    vec3 rotateByQuaternion(vec3 v, vec4 q) {
      return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
    }
    
    void main() {
      // Apply gaussian scaling and rotation
      vec3 scaledPos = a_position * a_scale;
      vec3 rotatedPos = rotateByQuaternion(scaledPos, a_rotation);
      
      gl_Position = u_mvpMatrix * vec4(rotatedPos, 1.0);
      
      v_color = a_color;
      v_opacity = a_opacity;
      v_texCoord = vec2(0.5, 0.5); // Center of gaussian
      
      // Point size for gaussian splatting
      gl_PointSize = 10.0;
    }
  )";

  // WebGL fragment shader for 3D Gaussian Splatting
  static const char *FRAGMENT_SHADER_SOURCE = R"(
    precision mediump float;
    
    varying vec3 v_color;
    varying float v_opacity;
    varying vec2 v_texCoord;
    
    void main() {
      // Gaussian falloff
      vec2 coord = gl_PointCoord - vec2(0.5, 0.5);
      float dist = dot(coord, coord);
      float alpha = exp(-dist * 4.0) * v_opacity;
      
      gl_FragColor = vec4(v_color * alpha, alpha);
    }
  )";

  void InitSystem::onExecute()
  {
    // Initialize any global resources needed for 3D model rendering
    DEBUG(LOG_TAG, "Initializing 3D model rendering system");
  }

  void LoadModelSystem::onExecute()
  {
    auto list = queryEntitiesWithComponent<Model3d>([](const Model3d &model) -> bool {
      return !model.isLoaded() && !model.src().empty();
    });

    if (list.size() == 0)
      return;

    for (auto &item : list) {
      auto webContent = getComponent<WebContent>(item.first);
      if (webContent != nullptr) {
        render(item.first, *webContent);
      }
    }
  }

  void LoadModelSystem::render(ecs::EntityId entity, WebContent &content)
  {
    auto model = getComponent<Model3d>(entity);
    if (!model || model->isLoaded()) {
      return;
    }

    const string &src = model->src();
    DEBUG(LOG_TAG, "Loading 3D model: %s", src.c_str());

    // Load based on model type
    if (model->isGaussianSplatting()) {
      if (loadGaussianSplattingModel(src, *model)) {
        DEBUG(LOG_TAG, "Successfully loaded 3DGS model with %zu splats", model->getSplatCount());
        content.setDirty(true);
      } else {
        DEBUG(LOG_TAG, "Failed to load 3DGS model: %s", src.c_str());
      }
    } else {
      if (loadGLTFModel(src, *model)) {
        DEBUG(LOG_TAG, "Successfully loaded GLTF model: %s", src.c_str());
      } else {
        DEBUG(LOG_TAG, "Failed to load GLTF model: %s", src.c_str());
      }
    }
  }

  Model3d::ModelType LoadModelSystem::detectModelType(const string &src, const string &typeHint)
  {
    if (!typeHint.empty()) {
      if (typeHint == "3dgs" || typeHint == "gaussian-splatting") {
        return Model3d::ModelType::GaussianSplatting;
      }
    }

    // Auto-detect from file extension
    size_t dotPos = src.find_last_of('.');
    if (dotPos != string::npos) {
      string ext = src.substr(dotPos + 1);
      transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
      
      if (ext == "gsplat" || ext == "ply") {
        return Model3d::ModelType::GaussianSplatting;
      } else if (ext == "gltf") {
        return Model3d::ModelType::GLTF;
      } else if (ext == "glb") {
        return Model3d::ModelType::GLB;
      }
    }

    return Model3d::ModelType::Unknown;
  }

  bool LoadModelSystem::loadGLTFModel(const string &filePath, Model3d &model)
  {
    // TODO: Implement GLTF loading
    DEBUG(LOG_TAG, "GLTF loading not yet implemented: %s", filePath.c_str());
    return false;
  }

  bool LoadModelSystem::loadGaussianSplattingModel(const string &filePath, Model3d &model)
  {
    vector<GaussianSplat> splats;
    
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos != string::npos) {
      string ext = filePath.substr(dotPos + 1);
      transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
      
      if (ext == "gsplat") {
        if (loadGSplatFile(filePath, splats)) {
          model.setSplats(move(splats));
          return true;
        }
      } else if (ext == "ply") {
        if (loadPlyFile(filePath, splats)) {
          model.setSplats(move(splats));
          return true;
        }
      }
    }
    
    return false;
  }

  bool LoadModelSystem::loadGSplatFile(const string &filePath, vector<GaussianSplat> &splats)
  {
    // TODO: Implement actual .gsplat file parsing
    // For now, create some test data
    DEBUG(LOG_TAG, "Creating test 3DGS data for: %s", filePath.c_str());
    
    // Create a simple test splat
    GaussianSplat testSplat;
    testSplat.position[0] = 0.0f;
    testSplat.position[1] = 0.0f;
    testSplat.position[2] = 0.0f;
    testSplat.color[0] = 1.0f;
    testSplat.color[1] = 0.0f;
    testSplat.color[2] = 0.0f;
    testSplat.opacity = 1.0f;
    testSplat.scale[0] = 1.0f;
    testSplat.scale[1] = 1.0f;
    testSplat.scale[2] = 1.0f;
    testSplat.rotation[0] = 0.0f;
    testSplat.rotation[1] = 0.0f;
    testSplat.rotation[2] = 0.0f;
    testSplat.rotation[3] = 1.0f;
    
    splats.push_back(testSplat);
    return true;
  }

  bool LoadModelSystem::loadPlyFile(const string &filePath, vector<GaussianSplat> &splats)
  {
    // TODO: Implement actual .ply file parsing
    // For now, create some test data
    DEBUG(LOG_TAG, "Creating test PLY data for: %s", filePath.c_str());
    
    // Create a few test splats
    for (int i = 0; i < 3; ++i) {
      GaussianSplat splat;
      splat.position[0] = i * 0.5f;
      splat.position[1] = 0.0f;
      splat.position[2] = 0.0f;
      splat.color[0] = (i == 0) ? 1.0f : 0.0f;
      splat.color[1] = (i == 1) ? 1.0f : 0.0f;
      splat.color[2] = (i == 2) ? 1.0f : 0.0f;
      splat.opacity = 0.8f;
      splat.scale[0] = 0.5f;
      splat.scale[1] = 0.5f;
      splat.scale[2] = 0.5f;
      splat.rotation[0] = 0.0f;
      splat.rotation[1] = 0.0f;
      splat.rotation[2] = 0.0f;
      splat.rotation[3] = 1.0f;
      
      splats.push_back(splat);
    }
    return true;
  }

  Render3DGSSystem::Render3DGSSystem()
      : RenderBaseSystem()
      , webglInitialized_(false)
  {
  }

  Render3DGSSystem::~Render3DGSSystem()
  {
    shutdownWebGL();
  }

  void Render3DGSSystem::onExecute()
  {
    if (!webglInitialized_) {
      if (!initializeWebGL()) {
        DEBUG(LOG_TAG, "Failed to initialize WebGL for 3DGS rendering");
        return;
      }
    }

    auto list = queryEntitiesWithComponent<Model3d>([](const Model3d &model) -> bool {
      return model.isLoaded() && model.isGaussianSplatting() && model.visible();
    });

    if (list.size() == 0)
      return;

    for (auto &item : list) {
      auto webContent = getComponent<WebContent>(item.first);
      if (webContent != nullptr && webContent->isDirty()) {
        render(item.first, *webContent);
      }
    }
  }

  void Render3DGSSystem::render(ecs::EntityId entity, WebContent &content)
  {
    auto model = getComponent<Model3d>(entity);
    if (!model || !model->isGaussianSplatting() || !model->isLoaded()) {
      return;
    }

    const auto &splats = model->getSplats();
    if (splats.empty()) {
      return;
    }

    DEBUG(LOG_TAG, "Rendering 3DGS model with %zu splats", splats.size());
    renderGaussianSplats(splats, content);
  }

  bool Render3DGSSystem::initializeWebGL()
  {
    DEBUG(LOG_TAG, "Initializing WebGL for 3DGS rendering");
    
    // Create shader program
    if (!createShaderProgram()) {
      DEBUG(LOG_TAG, "Failed to create shader program");
      return false;
    }

    // Create vertex array and buffer
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    webglInitialized_ = true;
    DEBUG(LOG_TAG, "WebGL initialization successful");
    return true;
  }

  void Render3DGSSystem::shutdownWebGL()
  {
    if (!webglInitialized_) {
      return;
    }

    destroyShaderProgram();
    
    if (vao_ != 0) {
      glDeleteVertexArrays(1, &vao_);
      vao_ = 0;
    }
    
    if (vbo_ != 0) {
      glDeleteBuffers(1, &vbo_);
      vbo_ = 0;
    }

    webglInitialized_ = false;
    DEBUG(LOG_TAG, "WebGL shutdown complete");
  }

  bool Render3DGSSystem::createShaderProgram()
  {
    // Compile vertex shader
    vertexShader_ = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader_, 1, &VERTEX_SHADER_SOURCE, nullptr);
    glCompileShader(vertexShader_);

    GLint success;
    glGetShaderiv(vertexShader_, GL_COMPILE_STATUS, &success);
    if (!success) {
      char infoLog[512];
      glGetShaderInfoLog(vertexShader_, 512, nullptr, infoLog);
      DEBUG(LOG_TAG, "Vertex shader compilation failed: %s", infoLog);
      return false;
    }

    // Compile fragment shader
    fragmentShader_ = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader_, 1, &FRAGMENT_SHADER_SOURCE, nullptr);
    glCompileShader(fragmentShader_);

    glGetShaderiv(fragmentShader_, GL_COMPILE_STATUS, &success);
    if (!success) {
      char infoLog[512];
      glGetShaderInfoLog(fragmentShader_, 512, nullptr, infoLog);
      DEBUG(LOG_TAG, "Fragment shader compilation failed: %s", infoLog);
      return false;
    }

    // Link shader program
    shaderProgram_ = glCreateProgram();
    glAttachShader(shaderProgram_, vertexShader_);
    glAttachShader(shaderProgram_, fragmentShader_);
    glLinkProgram(shaderProgram_);

    glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &success);
    if (!success) {
      char infoLog[512];
      glGetProgramInfoLog(shaderProgram_, 512, nullptr, infoLog);
      DEBUG(LOG_TAG, "Shader program linking failed: %s", infoLog);
      return false;
    }

    DEBUG(LOG_TAG, "Shader program created successfully");
    return true;
  }

  void Render3DGSSystem::destroyShaderProgram()
  {
    if (shaderProgram_ != 0) {
      glDeleteProgram(shaderProgram_);
      shaderProgram_ = 0;
    }
    if (vertexShader_ != 0) {
      glDeleteShader(vertexShader_);
      vertexShader_ = 0;
    }
    if (fragmentShader_ != 0) {
      glDeleteShader(fragmentShader_);
      fragmentShader_ = 0;
    }
  }

  void Render3DGSSystem::renderGaussianSplats(const vector<GaussianSplat> &splats, WebContent &content)
  {
    // Use the shader program
    glUseProgram(shaderProgram_);

    // TODO: Set up MVP matrices
    float identityMatrix[16] = {
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f
    };

    GLint mvpLocation = glGetUniformLocation(shaderProgram_, "u_mvpMatrix");
    GLint viewLocation = glGetUniformLocation(shaderProgram_, "u_viewMatrix");
    
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, identityMatrix);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, identityMatrix);

    // Bind vertex array
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    // Upload splat data
    glBufferData(GL_ARRAY_BUFFER, splats.size() * sizeof(GaussianSplat), splats.data(), GL_DYNAMIC_DRAW);

    // Set up vertex attributes
    GLint positionLocation = glGetAttribLocation(shaderProgram_, "a_position");
    GLint colorLocation = glGetAttribLocation(shaderProgram_, "a_color");
    GLint opacityLocation = glGetAttribLocation(shaderProgram_, "a_opacity");
    GLint scaleLocation = glGetAttribLocation(shaderProgram_, "a_scale");
    GLint rotationLocation = glGetAttribLocation(shaderProgram_, "a_rotation");

    size_t stride = sizeof(GaussianSplat);
    
    glEnableVertexAttribArray(positionLocation);
    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(GaussianSplat, position));
    
    glEnableVertexAttribArray(colorLocation);
    glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(GaussianSplat, color));
    
    glEnableVertexAttribArray(opacityLocation);
    glVertexAttribPointer(opacityLocation, 1, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(GaussianSplat, opacity));
    
    glEnableVertexAttribArray(scaleLocation);
    glVertexAttribPointer(scaleLocation, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(GaussianSplat, scale));
    
    glEnableVertexAttribArray(rotationLocation);
    glVertexAttribPointer(rotationLocation, 4, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(GaussianSplat, rotation));

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Render as points (will be expanded to quads in shader)
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(splats.size()));

    // Clean up
    glBindVertexArray(0);
    glUseProgram(0);
    glDisable(GL_BLEND);

    // Mark content as using texture
    content.setTextureUsing(true);
  }

  void Render3DGSSystem::sortSplats(vector<GaussianSplat> &splats, const float viewMatrix[16])
  {
    // TODO: Implement depth-based sorting for correct alpha blending
    // This would typically involve:
    // 1. Transform splat positions to view space
    // 2. Sort by depth (back to front for alpha blending)
    // 3. Update the splats vector with sorted order
    
    DEBUG(LOG_TAG, "Sorting %zu splats (placeholder implementation)", splats.size());
  }

  void RenderGLTFSystem::onExecute()
  {
    auto list = queryEntitiesWithComponent<Model3d>([](const Model3d &model) -> bool {
      return model.isLoaded() && !model.isGaussianSplatting() && model.visible();
    });

    if (list.size() == 0)
      return;

    for (auto &item : list) {
      auto webContent = getComponent<WebContent>(item.first);
      if (webContent != nullptr && webContent->isDirty()) {
        render(item.first, *webContent);
      }
    }
  }

  void RenderGLTFSystem::render(ecs::EntityId entity, WebContent &content)
  {
    // TODO: Implement GLTF rendering
    DEBUG(LOG_TAG, "GLTF rendering not yet implemented");
  }

  void UpdateTextureSystem::onExecute()
  {
    auto list = queryEntitiesWithComponent<Model3d>([](const Model3d &model) -> bool {
      return model.isLoaded() && model.visible();
    });

    if (list.size() == 0)
      return;

    for (auto &item : list) {
      auto webContent = getComponent<WebContent>(item.first);
      if (webContent != nullptr && webContent->isDirty()) {
        render(item.first, *webContent);
      }
    }
  }

  void UpdateTextureSystem::render(ecs::EntityId entity, WebContent &content)
  {
    // Similar to UpdateTextureSystem in web_content_renderer.cpp
    // This would update the WebGL texture with the rendered 3D content
    content.setDirty(false);
  }
}