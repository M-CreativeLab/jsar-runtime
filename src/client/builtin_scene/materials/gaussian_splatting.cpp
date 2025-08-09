#include <memory>
#include <vector>
#include <client/graphics/webgl_context.hpp>
#include <client/graphics/webgl_program.hpp>
#include <client/graphics/webgl_buffer.hpp>
#include "./gaussian_splatting.hpp"
#include "../meshes.hpp"

namespace builtin_scene::materials
{
  bool GaussianSplattingMaterial::initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                                             std::shared_ptr<client_graphics::WebGLProgram> program)
  {
    if (TR_UNLIKELY(!Material::initialize(glContext, program)))
      return false;

    // No buffer initialization needed - handled by GaussianSplatsMesh
    return true;
  }

  void GaussianSplattingMaterial::drawMeshImpl(shared_ptr<client_graphics::WebGLProgram> program,
                                               const Mesh3d &,
                                               RenderPass,
                                               optional<XRRenderTarget>)
  {
    auto glContext = glContext_.lock();
    if (!glContext)
      return;

    // Only handle drawing if we have splats to render
    if (splatInstanceCount_ == 0)
      return;

    // Do the instanced draw call
    glContext->drawElementsInstanced(
      client_graphics::WebGLDrawMode::kTriangles,
      6, // 6 indices for quad (2 triangles)
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

        // Setup vertex attributes for rendering
        if (splatInstanceCount_ > 0)
        {
          splatsMesh->setupSplatAttributes(program, glContext);
        }
      }
    }
  }
}
