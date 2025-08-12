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
    LOAD_UNIFORM_LOCATION("clipXY");
    LOAD_UNIFORM_LOCATION("focalAdjustment");
    LOAD_UNIFORM_LOCATION("splatCenters");
    LOAD_UNIFORM_LOCATION("splatColors");
    LOAD_UNIFORM_LOCATION("splatScales");
    LOAD_UNIFORM_LOCATION("splatQuat");
    // Note: viewMatrix and projectionMatrix are handled automatically by WebGL context
#undef LOAD_UNIFORM_LOCATION

    // Set default values for 3DGS parameters (excluding renderSize which is set per frame)
    auto maxStdDevOpt = glContext->getUniformLocation(program, "maxStdDev");
    if (maxStdDevOpt.has_value())
      glContext->uniform1f(maxStdDevOpt.value(), sqrt(8)); // Standard deviations to render

    auto minAlphaOpt = glContext->getUniformLocation(program, "minAlpha");
    if (minAlphaOpt.has_value())
      glContext->uniform1f(minAlphaOpt.value(), 0.5f * (1.0f / 255.0f)); // Minimum alpha threshold

    auto maxPixelRadiusOpt = glContext->getUniformLocation(program, "maxPixelRadius");
    if (maxPixelRadiusOpt.has_value())
      glContext->uniform1f(maxPixelRadiusOpt.value(), 512.0f); // Maximum pixel radius for splats

    auto clipXYOpt = glContext->getUniformLocation(program, "clipXY");
    if (clipXYOpt.has_value())
      glContext->uniform1f(clipXYOpt.value(), 1.4f);

    auto focalAdjustmentOpt = glContext->getUniformLocation(program, "focalAdjustment");
    if (focalAdjustmentOpt.has_value())
      glContext->uniform1f(focalAdjustmentOpt.value(), 1.0f);

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
      return;

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

    // Update render size from drawing buffer
    auto renderSizeOpt = glContext->getUniformLocation(program, "renderSize");
    if (renderSizeOpt.has_value())
    {
      float width = static_cast<float>(glContext->drawingBufferWidth());
      float height = static_cast<float>(glContext->drawingBufferHeight());
      glContext->uniform2f(renderSizeOpt.value(), width, height);
    }

    // Get the GaussianSplatsMesh from the Mesh3d component
    if (mesh != nullptr)
    {
      auto splatsMesh = mesh->getHandleAs<GaussianSplatsMesh>();
      if (splatsMesh != nullptr)
      {
        // Update splat count for drawing
        splatInstanceCount_ = splatsMesh->getTotalSplatCount();

        // Update buffer with sorted indices
        splatsMesh->updateSplatBuffer(glContext);

        // Bind separate splat data textures to texture units 0-3
        auto centersTexture = splatsMesh->getSplatCentersTexture();
        auto colorsTexture = splatsMesh->getSplatColorsTexture();
        auto scalesTexture = splatsMesh->getSplatScalesTexture();
        auto quatTexture = splatsMesh->getSplatQuatTexture();

        if (centersTexture && colorsTexture && scalesTexture && quatTexture)
        {
          // Bind centers texture to unit 0
          glContext->activeTexture(client_graphics::WebGLTextureUnit::kTexture0);
          glContext->bindTexture(client_graphics::WebGLTextureTarget::kTexture2D, centersTexture);
          auto centersOpt = glContext->getUniformLocation(program, "splatCenters");
          if (centersOpt.has_value())
            glContext->uniform1i(centersOpt.value(), 0);

          // Bind colors texture to unit 1
          glContext->activeTexture(client_graphics::WebGLTextureUnit::kTexture1);
          glContext->bindTexture(client_graphics::WebGLTextureTarget::kTexture2D, colorsTexture);
          auto colorsOpt = glContext->getUniformLocation(program, "splatColors");
          if (colorsOpt.has_value())
            glContext->uniform1i(colorsOpt.value(), 1);

          // Bind scales texture to unit 2
          glContext->activeTexture(client_graphics::WebGLTextureUnit::kTexture2);
          glContext->bindTexture(client_graphics::WebGLTextureTarget::kTexture2D, scalesTexture);
          auto scalesOpt = glContext->getUniformLocation(program, "splatScales");
          if (scalesOpt.has_value())
            glContext->uniform1i(scalesOpt.value(), 2);

          // Bind quaternion texture to unit 3
          glContext->activeTexture(client_graphics::WebGLTextureUnit::kTexture3);
          glContext->bindTexture(client_graphics::WebGLTextureTarget::kTexture2D, quatTexture);
          auto quatOpt = glContext->getUniformLocation(program, "splatQuat");
          if (quatOpt.has_value())
            glContext->uniform1i(quatOpt.value(), 3);
        }
      }
    }
  }
}
