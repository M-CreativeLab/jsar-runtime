#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <cstring>

#include <common/debug.hpp>
#include <client/per_process.hpp>
#include <client/graphics/webgl_context.hpp>

#include "./model_3d_renderer.hpp"
#include "./web_content.hpp"
#include "./materials.hpp"

namespace builtin_scene::model_renderer
{
  using namespace std;
  using namespace client_graphics;

  static const char *LOG_TAG = "Model3DRenderer";

  // Helper function to read shader source from file
  string readShaderFile(const string &filename)
  {
    string fullPath = "src/client/builtin_scene/shaders/" + filename;
    ifstream file(fullPath);
    if (!file.is_open()) {
      DEBUG(LOG_TAG, "Failed to open shader file: %s", fullPath.c_str());
      return "";
    }
    
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  }

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

  RenderGaussianSplattingSystem::RenderGaussianSplattingSystem()
      : RenderBaseSystem()
      , webglInitialized_(false)
  {
  }

  RenderGaussianSplattingSystem::~RenderGaussianSplattingSystem()
  {
    shutdownWebGL();
  }

  void RenderGaussianSplattingSystem::onExecute()
  {
    // Get the renderer resource to access WebGL context
    auto renderer = getResource<Renderer>();
    if (!renderer) {
      DEBUG(LOG_TAG, "Renderer resource not available");
      return;
    }

    // Cache the WebGL context
    if (glContext_.expired()) {
      glContext_ = renderer->glContext();
    }

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

  void RenderGaussianSplattingSystem::render(ecs::EntityId entity, WebContent &content)
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

  bool RenderGaussianSplattingSystem::initializeWebGL()
  {
    DEBUG(LOG_TAG, "Initializing WebGL for 3DGS rendering");
    
    auto glContext = glContext_.lock();
    if (!glContext) {
      DEBUG(LOG_TAG, "WebGL context is not available");
      return false;
    }
    
    // Create shader program
    if (!createShaderProgram()) {
      DEBUG(LOG_TAG, "Failed to create shader program");
      return false;
    }

    // Create vertex array and buffer using WebGL API
    vao_ = glContext->createVertexArray();
    vbo_ = glContext->createBuffer();

    webglInitialized_ = true;
    DEBUG(LOG_TAG, "WebGL initialization successful");
    return true;
  }

  void RenderGaussianSplattingSystem::shutdownWebGL()
  {
    if (!webglInitialized_) {
      return;
    }

    auto glContext = glContext_.lock();
    if (glContext) {
      destroyShaderProgram();
      
      if (vao_) {
        glContext->deleteVertexArray(vao_);
        vao_.reset();
      }
      
      if (vbo_) {
        glContext->deleteBuffer(vbo_);
        vbo_.reset();
      }
    }

    webglInitialized_ = false;
    DEBUG(LOG_TAG, "WebGL shutdown complete");
  }

  bool RenderGaussianSplattingSystem::createShaderProgram()
  {
    auto glContext = glContext_.lock();
    if (!glContext) {
      DEBUG(LOG_TAG, "WebGL context is not available");
      return false;
    }

    // Read shader sources from files
    string vertexSource = readShaderFile("gaussian_splatting.vert");
    string fragmentSource = readShaderFile("gaussian_splatting.frag");
    
    if (vertexSource.empty() || fragmentSource.empty()) {
      DEBUG(LOG_TAG, "Failed to read shader files");
      return false;
    }

    try {
      // Create and compile vertex shader
      vertexShader_ = glContext->createShader(WebGLShaderType::kVertexShader);
      glContext->shaderSource(vertexShader_, vertexSource);
      glContext->compileShader(vertexShader_);
      
      // Check compilation status
      bool compiled = glContext->getShaderParameter(vertexShader_, WEBGL_COMPILE_STATUS);
      if (!compiled) {
        string infoLog = glContext->getShaderInfoLog(vertexShader_);
        DEBUG(LOG_TAG, "Vertex shader compilation failed: %s", infoLog.c_str());
        return false;
      }

      // Create and compile fragment shader
      fragmentShader_ = glContext->createShader(WebGLShaderType::kFragmentShader);
      glContext->shaderSource(fragmentShader_, fragmentSource);
      glContext->compileShader(fragmentShader_);
      
      compiled = glContext->getShaderParameter(fragmentShader_, WEBGL_COMPILE_STATUS);
      if (!compiled) {
        string infoLog = glContext->getShaderInfoLog(fragmentShader_);
        DEBUG(LOG_TAG, "Fragment shader compilation failed: %s", infoLog.c_str());
        return false;
      }

      // Create and link shader program
      shaderProgram_ = glContext->createProgram();
      glContext->attachShader(shaderProgram_, vertexShader_);
      glContext->attachShader(shaderProgram_, fragmentShader_);
      glContext->linkProgram(shaderProgram_);

      bool linked = glContext->getProgramParameter(shaderProgram_, WEBGL_LINK_STATUS);
      if (!linked) {
        string infoLog = glContext->getProgramInfoLog(shaderProgram_);
        DEBUG(LOG_TAG, "Shader program linking failed: %s", infoLog.c_str());
        return false;
      }

      DEBUG(LOG_TAG, "Shader program created successfully");
      return true;
    } catch (const exception &e) {
      DEBUG(LOG_TAG, "Exception during shader creation: %s", e.what());
      return false;
    }
  }

  void RenderGaussianSplattingSystem::destroyShaderProgram()
  {
    auto glContext = glContext_.lock();
    if (!glContext) {
      return;
    }

    if (shaderProgram_) {
      glContext->deleteProgram(shaderProgram_);
      shaderProgram_.reset();
    }
    if (vertexShader_) {
      glContext->deleteShader(vertexShader_);
      vertexShader_.reset();
    }
    if (fragmentShader_) {
      glContext->deleteShader(fragmentShader_);
      fragmentShader_.reset();
    }
  }

  void RenderGaussianSplattingSystem::renderGaussianSplats(const vector<GaussianSplat> &splats, WebContent &content)
  {
    auto glContext = glContext_.lock();
    if (!glContext || !shaderProgram_) {
      return;
    }

    // Use the shader program
    glContext->useProgram(shaderProgram_);

    // TODO: Set up MVP matrices properly
    float identityMatrix[16] = {
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f
    };

    auto mvpLocation = glContext->getUniformLocation(shaderProgram_, "u_mvpMatrix");
    auto viewLocation = glContext->getUniformLocation(shaderProgram_, "u_viewMatrix");
    
    if (mvpLocation) {
      glContext->uniformMatrix4fv(mvpLocation, false, identityMatrix);
    }
    if (viewLocation) {
      glContext->uniformMatrix4fv(viewLocation, false, identityMatrix);
    }

    // Bind vertex array and buffer
    glContext->bindVertexArray(vao_);
    glContext->bindBuffer(WebGLBufferBindingTarget::kArrayBuffer, vbo_);

    // Upload splat data
    glContext->bufferData(WebGLBufferBindingTarget::kArrayBuffer, 
                         splats.size() * sizeof(GaussianSplat), 
                         splats.data(), 
                         WebGLBufferUsage::kDynamicDraw);

    // Set up vertex attributes
    auto positionLocation = glContext->getAttribLocation(shaderProgram_, "a_position");
    auto colorLocation = glContext->getAttribLocation(shaderProgram_, "a_color");
    auto opacityLocation = glContext->getAttribLocation(shaderProgram_, "a_opacity");
    auto scaleLocation = glContext->getAttribLocation(shaderProgram_, "a_scale");
    auto rotationLocation = glContext->getAttribLocation(shaderProgram_, "a_rotation");

    size_t stride = sizeof(GaussianSplat);
    
    if (positionLocation >= 0) {
      glContext->enableVertexAttribArray(positionLocation);
      glContext->vertexAttribPointer(positionLocation, 3, WebGLDataType::kFloat, false, stride, 
                                    offsetof(GaussianSplat, position));
    }
    
    if (colorLocation >= 0) {
      glContext->enableVertexAttribArray(colorLocation);
      glContext->vertexAttribPointer(colorLocation, 3, WebGLDataType::kFloat, false, stride, 
                                    offsetof(GaussianSplat, color));
    }
    
    if (opacityLocation >= 0) {
      glContext->enableVertexAttribArray(opacityLocation);
      glContext->vertexAttribPointer(opacityLocation, 1, WebGLDataType::kFloat, false, stride, 
                                    offsetof(GaussianSplat, opacity));
    }
    
    if (scaleLocation >= 0) {
      glContext->enableVertexAttribArray(scaleLocation);
      glContext->vertexAttribPointer(scaleLocation, 3, WebGLDataType::kFloat, false, stride, 
                                    offsetof(GaussianSplat, scale));
    }
    
    if (rotationLocation >= 0) {
      glContext->enableVertexAttribArray(rotationLocation);
      glContext->vertexAttribPointer(rotationLocation, 4, WebGLDataType::kFloat, false, stride, 
                                    offsetof(GaussianSplat, rotation));
    }

    // Enable blending for transparency
    glContext->enable(WEBGL_BLEND);
    glContext->blendFunc(WebGLBlendFactor::kSrcAlpha, WebGLBlendFactor::kOneMinusSrcAlpha);

    // Render as points (will be expanded to quads in shader)
    glContext->drawArrays(WebGLDrawMode::kPoints, 0, static_cast<int>(splats.size()));

    // Clean up
    glContext->bindVertexArray(nullptr);
    glContext->useProgram(nullptr);
    glContext->disable(WEBGL_BLEND);

    // Mark content as using texture
    content.setTextureUsing(true);
  }

  void RenderGaussianSplattingSystem::sortSplats(vector<GaussianSplat> &splats, const float viewMatrix[16])
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