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
#include "./client_renderer.hpp"
#include "./renderer/scene_renderer.hpp"

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

    // TODO: Implement the rendering logic for Gaussian splatting.
  }
}
