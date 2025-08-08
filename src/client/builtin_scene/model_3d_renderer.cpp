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

  void InitSystem::onExecute()
  {
  }

  void RenderBaseSystem::render(ecs::EntityId entity)
  {
  }

  void RenderGaussianSplattingSystem::onExecute()
  {
  }

  void RenderGaussianSplattingSystem::render(ecs::EntityId entity)
  {
  }
}
