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

#include "./model3d_renderer.hpp"
#include "../web_content.hpp"
#include "../materials.hpp"
#include "../client_renderer.hpp"
#include "./scene_renderer.hpp"

namespace builtin_scene::model_renderer
{
  using namespace std;
  using namespace client_graphics;

  void InitSystem::onExecute()
  {
  }

  void RenderBaseSystem::render(ecs::EntityId entity, Model3d &model)
  {
  }

  void RenderGaussianSplattingSystem::onExecute()
  {
    auto list = queryEntitiesWithComponent<Model3d>([](const Model3d &model) -> bool
                                                    { return model.isLoaded() && model.visible() && model.isGaussianSplatting(); });

    if (list.size() == 0)
      return;

    for (auto &item : list)
      render(item.first, *item.second);
  }

  void RenderGaussianSplattingSystem::render(ecs::EntityId entity, Model3d &model)
  {
    // Get the splat data
    const auto &splats = model.getSplats();
    if (splats.empty())
      return;

    // Get the GaussianSplattingMaterial from the entity
    auto scene = getResource<Scene>();
    if (!scene)
      return;

    // Try to get the material component from the entity
    auto gaussianMaterial = scene->getComponent<std::shared_ptr<materials::GaussianSplattingMaterial>>(entity);
    if (!gaussianMaterial)
    {
      // If no material is attached to the entity, skip rendering
      return;
    }

    // Use the material's glContext (inherited from Material base class)
    auto glContext = gaussianMaterial->get()->glContext_.lock();
    if (!glContext)
      return;

    // TODO: The material needs to be initialized with a shader program first
    // This is typically done by the material management system
    // For now, we'll skip the rendering if the material is not properly initialized
    
    // The actual rendering would involve:
    // 1. Creating/getting the shader program for gaussian splatting
    // 2. Initializing the material with the program
    // 3. Setting up uniforms and attributes
    // 4. Drawing the instanced splats
    
    // Placeholder for future implementation
    std::cerr << "Gaussian splatting rendering framework ready, but needs shader program initialization" << std::endl;
  }
}
