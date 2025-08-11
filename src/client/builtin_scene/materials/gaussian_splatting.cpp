#include <memory>
#include <vector>
#include <client/graphics/webgl_context.hpp>
#include <client/graphics/webgl_program.hpp>
#include <client/graphics/webgl_buffer.hpp>
#include <common/debug.hpp>

#include "./gaussian_splatting.hpp"
#include "../meshes.hpp"

namespace builtin_scene::materials
{
  bool GaussianSplattingMaterial::initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                                             std::shared_ptr<client_graphics::WebGLProgram> program)
  {
    if (TR_UNLIKELY(!Material::initialize(glContext, program)))
      return false;

#define LOAD_UNIFORM_LOCATION(name)                          \
  {                                                          \
    auto loc = glContext->getUniformLocation(program, name); \
    if (loc.has_value())                                     \
    {                                                        \
      uniforms_.emplace(name, loc.value());                  \
    }                                                        \
  }
    // Load uniforms for improved 3DGS rendering
    LOAD_UNIFORM_LOCATION("renderSize");
    LOAD_UNIFORM_LOCATION("maxStdDev");
    LOAD_UNIFORM_LOCATION("minAlpha");
    LOAD_UNIFORM_LOCATION("maxPixelRadius");
    // Note: viewMatrix and projectionMatrix are handled automatically by WebGL context
#undef LOAD_UNIFORM_LOCATION

    // Set default values for 3DGS parameters
    auto renderSizeOpt = glContext->getUniformLocation(program, "renderSize");
    if (renderSizeOpt.has_value())
    {
      glContext->uniform2f(renderSizeOpt.value(), 1600.0f, 900.0f); // Default render size
    }

    auto maxStdDevOpt = glContext->getUniformLocation(program, "maxStdDev");
    if (maxStdDevOpt.has_value())
    {
      glContext->uniform1f(maxStdDevOpt.value(), sqrt(8)); // Standard deviations to render
    }

    auto minAlphaOpt = glContext->getUniformLocation(program, "minAlpha");
    if (minAlphaOpt.has_value())
    {
      glContext->uniform1f(minAlphaOpt.value(), 0.5f * (1.0f / 255.0f)); // Minimum alpha threshold
    }

    auto maxPixelRadiusOpt = glContext->getUniformLocation(program, "maxPixelRadius");
    if (maxPixelRadiusOpt.has_value())
    {
      glContext->uniform1f(maxPixelRadiusOpt.value(), 512.0f); // Maximum pixel radius for splats
    }

    return true;
  }

  void GaussianSplattingMaterial::drawMeshImpl(shared_ptr<client_graphics::WebGLProgram> program,
                                               const Mesh3d &mesh,
                                               RenderPass renderPass,
                                               optional<XRRenderTarget>)
  {
    assert(renderPass == RenderPass::kTransparents &&
           "GaussianSplattingMaterial should only be used in the transparent render pass.");

    auto glContext = glContext_.lock();
    if (!glContext)
      return;

    // Only handle drawing if we have splats to render
    if (splatInstanceCount_ == 0)
    {
      DEBUG("GaussianSplattingMaterial", "No splats to render, skipping draw call");
      return;
    }

    DEBUG("GaussianSplattingMaterial", "Rendering %zu splat instances", splatInstanceCount_);

    // Do the instanced draw call
    glContext->drawElementsInstanced(
      mesh.primitiveTopology(),
      mesh.indices().size(),
      WEBGL_UNSIGNED_INT,
      0,
      splatInstanceCount_);
  }

  void GaussianSplattingMaterial::onBeforeDrawMesh(shared_ptr<client_graphics::WebGLProgram> program,
                                                   shared_ptr<Mesh3d> mesh)
  {
    Material::onBeforeDrawMesh(program, mesh);

    auto glContext = glContext_.lock();
    if (!glContext) [[unlikely]]
      return;

    // Get the GaussianSplatsMesh from the Mesh3d component
    if (mesh != nullptr)
    {
      auto splatsMesh = mesh->getHandleAs<GaussianSplatsMesh>();
      if (splatsMesh != nullptr)
      {
        // Update splat count for drawing
        splatInstanceCount_ = splatsMesh->getTotalSplatCount();

        // Update buffer with current splat data
        splatsMesh->updateSplatBuffer(glContext);
      }
    }
  }
}
