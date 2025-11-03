#include "./mesh_material.hpp"
#include "./meshes.hpp"

namespace endor
{
  namespace builtin_scene
  {
    using namespace std;

    bool MeshMaterial3d::matchesPass(const RenderPass &pass) const
    {
      assert(pass == RenderPass::kOpaques || pass == RenderPass::kTransparents);
      if (pass == RenderPass::kOpaques)
        return isOpaque();
      else if (pass == RenderPass::kTransparents)
        return isTransparent();
      else
        return false;
    }

    void MeshMaterial3d::initialize(shared_ptr<client_graphics::WebGL2Context> glContext,
                                    shared_ptr<client_graphics::WebGLProgram> program,
                                    shared_ptr<Mesh3d> mesh)
    {
      if (program == nullptr)
        throw runtime_error("The program is not initialized.");

      // Configure the mesh attributes
      mesh->configureVertexAttribs(program);
      mesh->configureInstanceAttribs(program);

      // Initialize the material with the WebGL context and program.
      if (material_->initialize(glContext, program))
      {
        program_ = program;
        glContext_ = glContext;
        mesh_ = mesh;
        initialized_ = true;
      }
      else
      {
        throw runtime_error("Failed to initialize the material: " + material_->name());
      }
    }
  }
} // namespace endor
