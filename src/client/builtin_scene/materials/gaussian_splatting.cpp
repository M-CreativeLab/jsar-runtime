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
    LOAD_UNIFORM_LOCATION("maxDistance");
    LOAD_UNIFORM_LOCATION("compressedSplats");
    LOAD_UNIFORM_LOCATION("scaleMin");
    LOAD_UNIFORM_LOCATION("scaleMax");
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

    auto maxDistanceOpt = glContext->getUniformLocation(program, "maxDistance");
    if (maxDistanceOpt.has_value())
      glContext->uniform1f(maxDistanceOpt.value(), 2.0f); // Default 1 meters

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

        // Bind compressed splat texture to texture unit 0
        auto compressedTexture = splatsMesh->getCompressedSplatsTexture();
        if (compressedTexture)
        {
          // Bind compressed texture to unit 0 (now texture2D instead of texture2DArray)
          glContext->activeTexture(client_graphics::WebGLTextureUnit::kTexture0);
          glContext->bindTexture(client_graphics::WebGLTextureTarget::kTexture2D, compressedTexture);
          auto compressedOpt = glContext->getUniformLocation(program, "compressedSplats");
          if (compressedOpt.has_value())
            glContext->uniform1i(compressedOpt.value(), 0);

          // Set normalization parameters as uniforms (only scale needed now)
          auto scaleMinOpt = glContext->getUniformLocation(program, "scaleMin");
          if (scaleMinOpt.has_value())
          {
            const auto &normParams = splatsMesh->getNormalizationParams();
            glContext->uniform3f(scaleMinOpt.value(), normParams.scaleMin[0], normParams.scaleMin[1], normParams.scaleMin[2]);
          }

          auto scaleMaxOpt = glContext->getUniformLocation(program, "scaleMax");
          if (scaleMaxOpt.has_value())
          {
            const auto &normParams = splatsMesh->getNormalizationParams();
            glContext->uniform3f(scaleMaxOpt.value(), normParams.scaleMax[0], normParams.scaleMax[1], normParams.scaleMax[2]);
          }
        }
      }
    }
  }
}
