#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <cstring>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common/debug.hpp>
#include <client/per_process.hpp>
#include <client/graphics/webgl_context.hpp>

#include "./model_3d_renderer.hpp"
#include "./web_content.hpp"
#include "./materials.hpp"
#include "./ply_parser.hpp"

namespace builtin_scene::model_renderer
{
  using namespace std;
  using namespace client_graphics;

  static const char *LOG_TAG = "Model3DRenderer";



  void InitSystem::onExecute()
  {
    // Initialize any global resources needed for 3D model rendering
    DEBUG(LOG_TAG, "Initializing 3D model rendering system");
  }

  void LoadModelSystem::onExecute()
  {
    // Model loading is now handled by HTMLModelElement
    // This system is no longer needed, but kept for backward compatibility
    DEBUG(LOG_TAG, "LoadModelSystem is deprecated - model loading moved to HTMLModelElement");
  }

  void LoadModelSystem::render(ecs::EntityId entity, WebContent &content)
  {
    // No longer needed - HTMLModelElement handles loading
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

  RenderGaussianSplattingSystem::RenderGaussianSplattingSystem()
      : RenderBaseSystem()
      , materialInitialized_(false)
  {
  }

  void RenderGaussianSplattingSystem::onExecute()
  {
    // Initialize material on first use
    if (!materialInitialized_) {
      if (!initializeMaterial()) {
        DEBUG(LOG_TAG, "Failed to initialize Gaussian Splatting material");
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
    
    // Get camera position for depth sorting
    glm::vec3 cameraPos = getCameraPosition();
    
    // Convert Model3d splat format to material format
    std::vector<materials::GaussianSplattingMaterial::GaussianSplat> materialSplats;
    materialSplats.reserve(splats.size());
    
    for (const auto &splat : splats) {
      materials::GaussianSplattingMaterial::GaussianSplat ms;
      ms.position = glm::vec3(splat.position[0], splat.position[1], splat.position[2]);
      ms.color = glm::vec3(splat.color[0], splat.color[1], splat.color[2]);
      ms.opacity = splat.opacity;
      ms.scale = glm::vec3(splat.scale[0], splat.scale[1], splat.scale[2]);
      ms.rotation = glm::vec4(splat.rotation[0], splat.rotation[1], 
                              splat.rotation[2], splat.rotation[3]);
      materialSplats.push_back(ms);
    }
    
    // Sort splats by depth (back to front for proper alpha blending)
    DEBUG(LOG_TAG, "Sorting %zu splats by depth", materialSplats.size());
    std::sort(materialSplats.begin(), materialSplats.end(), 
      [&cameraPos](const materials::GaussianSplattingMaterial::GaussianSplat &a, 
                   const materials::GaussianSplattingMaterial::GaussianSplat &b) {
        float distA = glm::distance2(a.position, cameraPos);
        float distB = glm::distance2(b.position, cameraPos);
        return distA > distB; // Back to front sorting
      });
    
    // Update material with sorted splats data
    gaussianMaterial_->updateSplats(materialSplats);
    
    // TODO: Actually render using the material and mesh system
    // This would typically involve creating/updating a mesh and rendering it
    // with the gaussian splatting material
  }

  bool RenderGaussianSplattingSystem::initializeMaterial()
  {
    DEBUG(LOG_TAG, "Initializing Gaussian Splatting material");
    
    gaussianMaterial_ = std::make_shared<materials::GaussianSplattingMaterial>();
    
    // Get the renderer to access the WebGL context
    auto renderer = getResource<Renderer>();
    if (!renderer) {
      DEBUG(LOG_TAG, "Renderer resource not available");
      return false;
    }
    
    auto glContext = renderer->glContext();
    if (!glContext) {
      DEBUG(LOG_TAG, "WebGL context not available");
      return false;
    }
    
    // The material will be initialized when first used
    materialInitialized_ = true;
    DEBUG(LOG_TAG, "Gaussian Splatting material initialized successfully");
    return true;
  }

  glm::vec3 RenderGaussianSplattingSystem::getCameraPosition()
  {
    // Get camera position from the renderer or scene
    // For now, return a default position (origin)
    // TODO: Get actual camera position from the scene
    return glm::vec3(0.0f, 0.0f, 0.0f);
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