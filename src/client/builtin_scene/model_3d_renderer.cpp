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
      
      if (ext == "ksplat") {
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
  {
  }

  void RenderGaussianSplattingSystem::onExecute()
  {
    auto list = queryEntitiesWithComponent<Model3d>([](const Model3d &model) -> bool {
      return model.isLoaded() && model.isGaussianSplatting() && model.visible();
    });

    if (list.size() == 0)
      return;

    DEBUG(LOG_TAG, "Found %zu 3DGS models to render (rendering not implemented in this PR)", list.size());

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
    DEBUG(LOG_TAG, "Model has %zu splats (rendering not implemented in this PR)", splats.size());
    
    // TODO: Implement actual 3DGS rendering in future PR
    // This PR only provides the framework and data structures
    
    // Mark content as clean since we're not actually rendering anything yet
    content.setDirty(false);
  }

  void RenderGLTFSystem::onExecute()
  {
    auto list = queryEntitiesWithComponent<Model3d>([](const Model3d &model) -> bool {
      return model.isLoaded() && !model.isGaussianSplatting() && model.visible();
    });

    if (list.size() == 0)
      return;

    DEBUG(LOG_TAG, "Found %zu GLTF models to render (rendering not implemented)", list.size());

    for (auto &item : list) {
      auto webContent = getComponent<WebContent>(item.first);
      if (webContent != nullptr && webContent->isDirty()) {
        render(item.first, *webContent);
      }
    }
  }

  void RenderGLTFSystem::render(ecs::EntityId entity, WebContent &content)
  {
    // TODO: Implement GLTF rendering in future PR
    DEBUG(LOG_TAG, "GLTF rendering not yet implemented");
    content.setDirty(false);
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