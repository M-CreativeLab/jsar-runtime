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

    // Get the renderer and scene components
    auto renderer = getResource<Renderer>();
    if (!renderer)
      return;

    auto glContext = renderer->glContext();
    if (!glContext)
      return;

    // Get materials resource
    auto materials = getResource<Materials>();
    if (!materials)
      return;

    // Get or create the Gaussian Splatting material
    auto gaussianMaterial = materials->get<materials::GaussianSplattingMaterial>("gaussian_splatting");
    if (!gaussianMaterial)
    {
      // Create the material if it doesn't exist
      auto newMaterial = std::make_shared<materials::GaussianSplattingMaterial>();
      materials->add("gaussian_splatting", newMaterial);
      gaussianMaterial = newMaterial;
    }

    // Convert Model3d splats to material splats format
    std::vector<materials::GaussianSplattingMaterial::GaussianSplat> materialSplats;
    materialSplats.reserve(splats.size());

    for (const auto &splat : splats)
    {
      materials::GaussianSplattingMaterial::GaussianSplat materialSplat;
      materialSplat.position = glm::vec3(splat.position[0], splat.position[1], splat.position[2]);
      materialSplat.color = glm::vec3(splat.color[0], splat.color[1], splat.color[2]);
      materialSplat.opacity = splat.opacity;
      materialSplat.scale = glm::vec3(splat.scale[0], splat.scale[1], splat.scale[2]);
      materialSplat.rotation = glm::vec4(splat.rotation[0], splat.rotation[1], splat.rotation[2], splat.rotation[3]);
      materialSplats.push_back(materialSplat);
    }

    // Update the material with splat data
    gaussianMaterial->updateSplats(materialSplats);

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

    // Set matrix uniforms
    auto mvpLoc = glContext->getUniformLocation(program, "u_mvpMatrix");
    if (mvpLoc.has_value())
    {
      glContext->uniformMatrix4fv(mvpLoc.value(), 1, false, &mvpMatrix[0][0]);
    }

    auto viewLoc = glContext->getUniformLocation(program, "u_viewMatrix");
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
}
