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
    LOAD_UNIFORM_LOCATION("quadSize");
#undef LOAD_UNIFORM_LOCATION

    glContext->uniform2f(uniform("quadSize"), 2.0f, 2.0f);

    // No buffer initialization needed - handled by GaussianSplatsMesh
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
