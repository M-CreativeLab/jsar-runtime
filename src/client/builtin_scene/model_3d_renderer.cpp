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

    // Get the splat data
    const auto &splats = model->getSplats();
    if (splats.empty())
    {
      return;
    }

    // Get the scene renderer and WebGL context
    auto renderer = getResource<SceneRenderer>();
    if (!renderer)
    {
      return;
    }
    auto glContext = renderer->glContext();
    if (!glContext)
    {
      return;
    }

    // Get the materials resource to manage our Gaussian splatting material
    auto materials = getResource<Materials>();
    if (!materials)
    {
      return;
    }

    // Create or get the Gaussian splatting material
    static std::shared_ptr<materials::GaussianSplattingMaterial> material = nullptr;
    if (!material)
    {
      material = std::make_shared<materials::GaussianSplattingMaterial>();
      // Initialize the material with the WebGL context
      auto program = materials->getOrCreateProgram(glContext, material.get());
      if (program)
      {
        material->initialize(glContext, program);
      }
    }

    if (!material)
    {
      return;
    }

    // Convert Model3d splats to material format and update
    std::vector<materials::GaussianSplattingMaterial::GaussianSplat> materialSplats;
    materialSplats.reserve(splats.size());

    for (const auto &splat : splats)
    {
      materials::GaussianSplattingMaterial::GaussianSplat materialSplat;
      materialSplat.position = splat.position;
      materialSplat.color = splat.color;
      materialSplat.opacity = splat.opacity;
      materialSplat.scale = splat.scale;
      materialSplat.rotation = splat.rotation;
      materialSplats.push_back(materialSplat);
    }

    material->updateSplats(materialSplats);

    // Get the shader program for rendering
    auto program = materials->getOrCreateProgram(glContext, material.get());
    if (!program)
    {
      return;
    }

    // Use the shader program
    glContext->useProgram(program);

    // Set up uniforms - TODO: Get proper MVP and view matrices from camera
    // For now, use identity matrices as placeholders
    glm::mat4 mvpMatrix = glm::mat4(1.0f);
    glm::mat4 viewMatrix = glm::mat4(1.0f);

    auto mvpLoc = glContext->getUniformLocation(program, "u_mvpMatrix");
    if (mvpLoc.has_value())
    {
      glContext->uniformMatrix4fv(mvpLoc.value(), false, &mvpMatrix[0][0]);
    }

    auto viewLoc = glContext->getUniformLocation(program, "u_viewMatrix");
    if (viewLoc.has_value())
    {
      glContext->uniformMatrix4fv(viewLoc.value(), false, &viewMatrix[0][0]);
    }

    // Enable blending for transparency
    glContext->enable(client_graphics::WebGL2Context::BLEND);
    glContext->blendFunc(client_graphics::WebGL2Context::SRC_ALPHA,
                         client_graphics::WebGL2Context::ONE_MINUS_SRC_ALPHA);

    // Render the splats using the material
    if (material->getSplatCount() > 0)
    {
      // Create a dummy mesh for the material system
      auto meshes = getResource<Meshes>();
      if (meshes)
      {
        // The material's onBeforeDrawMesh will set up vertex attributes
        material->onBeforeDrawMesh(program, nullptr);

        // Draw the splats as points
        glContext->drawArrays(client_graphics::WebGL2Context::POINTS, 0, material->getSplatCount());

        // Clean up
        material->onAfterDrawMesh(program, nullptr);
      }
    }

    // Disable blending
    glContext->disable(client_graphics::WebGL2Context::BLEND);
  }

  void RenderGaussianSplattingSystem::render(ecs::EntityId entity, std::shared_ptr<client_graphics::WebGL2Context> glContext)
  {
    // This overload is for future WebGL integration
    render(entity);
  }
}
