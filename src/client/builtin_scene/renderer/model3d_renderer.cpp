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

    // Get the renderer and scene components
    auto renderer = getResource<Renderer>();
    if (!renderer)
      return;

    auto glContext = renderer->glContext();
    if (!glContext)
      return;

    // Get the GaussianSplattingMaterial from the entity
    // Note: This assumes the material is stored as a component on the entity
    // The actual implementation may need adjustment based on how materials are handled in the ECS
    try
    {
      auto scene = getResource<Scene>();
      if (!scene)
        return;

      // Try to get the material component from the entity
      // This is a placeholder - the actual component retrieval may differ
      auto gaussianMaterial = scene->getComponent<std::shared_ptr<materials::GaussianSplattingMaterial>>(entity);
      if (!gaussianMaterial)
      {
        // If no material is attached to the entity, skip rendering
        return;
      }

      // Get or create the material's shader program
      auto program = gaussianMaterial->getShaderProgram();
      if (!program)
        return;

      // Use the material's shader program
      glContext->useProgram(program);

      // Enable depth testing and blending for proper 3D rendering
      glContext->enable(WEBGL_DEPTH_TEST);
      glContext->enable(WEBGL_BLEND);
      glContext->blendFunc(WEBGL_SRC_ALPHA, WEBGL_ONE_MINUS_SRC_ALPHA);

      // Set up camera matrices
      // TODO: Get actual camera matrices from the scene
      glm::mat4 modelMatrix = glm::mat4(1.0f);
      glm::mat4 viewMatrix = glm::mat4(1.0f);
      glm::mat4 projectionMatrix = glm::mat4(1.0f);
      glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

      // Set matrix uniforms (updated to use new naming convention)
      auto mvpLoc = glContext->getUniformLocation(program, "uMvpMatrix");
      if (mvpLoc.has_value())
      {
        glContext->uniformMatrix4fv(mvpLoc.value(), 1, false, &mvpMatrix[0][0]);
      }

      auto viewLoc = glContext->getUniformLocation(program, "uViewMatrix");
      if (viewLoc.has_value())
      {
        glContext->uniformMatrix4fv(viewLoc.value(), 1, false, &viewMatrix[0][0]);
      }

      // Set up the material for rendering (attributes, uniforms, etc.)
      gaussianMaterial->onBeforeDrawMesh(program, nullptr);

      // Perform the instanced drawing
      gaussianMaterial->drawInstanced(glContext, program);

      // Clean up after rendering
      gaussianMaterial->onAfterDrawMesh(program, nullptr);

      // Disable blending and depth testing
      glContext->disable(WEBGL_BLEND);
      glContext->disable(WEBGL_DEPTH_TEST);
    }
    catch (const std::exception &e)
    {
      // Handle cases where the material component is not found
      std::cerr << "Failed to render Gaussian splatting model: " << e.what() << std::endl;
      return;
    }
  }
}
