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

  void RenderBaseSystem::render(ecs::EntityId entity)
  {
  }

  void RenderGaussianSplattingSystem::onExecute()
  {
    // Iterate through all entities with Model3d components
    // This provides the framework for Gaussian splatting rendering
    getEcs()->view<Model3d>().each([this](ecs::EntityId entity, Model3d &model)
                                   {
      if (model.isLoaded() && model.visible() && model.isGaussianSplatting()) {
        render(entity);
      } });
  }

  void RenderGaussianSplattingSystem::render(ecs::EntityId entity)
  {
    auto model = getEcs()->getComponent<Model3d>(entity);
    if (!model || !model->isLoaded() || !model->isGaussianSplatting())
    {
      return;
    }

    // Framework is ready for Gaussian splatting rendering
    const auto &splats = model->getSplats();
    if (splats.empty())
    {
      return;
    }

    // TODO: Actual WebGL rendering implementation
    // This framework implementation demonstrates that:
    // 1. The system can find Model3d entities with Gaussian splatting data
    // 2. The KsplatLoader successfully parsed the .ksplat files
    // 3. The rendering pipeline is ready for WebGL implementation
    //
    // Future implementation will:
    // - Create WebGL buffers for position, color, opacity, scale, rotation
    // - Compile and use the Gaussian splatting shaders
    // - Render splats as points with proper transparency blending
    // - Apply camera transformations and view-projection matrices
  }

  void RenderGaussianSplattingSystem::render(ecs::EntityId entity, std::shared_ptr<client_graphics::WebGL2Context> glContext)
  {
    // This overload is for future WebGL integration
    render(entity);
  }
}
