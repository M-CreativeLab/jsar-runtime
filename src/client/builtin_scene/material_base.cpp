#include "./material_base.hpp"
#include "./meshes.hpp"

namespace builtin_scene
{
  using namespace std;

  void Material::SetGlobalDefines(const string &define)
  {
    if (find(GlobalDefines.begin(), GlobalDefines.end(), define) == GlobalDefines.end())
      GlobalDefines.push_back(define);
  }

  void Material::UnsetGlobalDefines(const string &define)
  {
    auto it = find(GlobalDefines.begin(), GlobalDefines.end(), define);
    if (it != GlobalDefines.end())
      GlobalDefines.erase(it);
  }

  bool Material::initialize(shared_ptr<client_graphics::WebGL2Context> glContext,
                            shared_ptr<client_graphics::WebGLProgram> program)
  {
    if (TR_UNLIKELY(glContext == nullptr || program == nullptr))
      return false;
    glContext_ = glContext;
    return true;
  }

  void Material::drawMeshImpl(shared_ptr<client_graphics::WebGLProgram> program,
                              const Mesh3d &mesh,
                              RenderPass renderPass,
                              optional<XRRenderTarget> renderTarget)
  {
    auto glContext = glContext_.lock();
    if (glContext == nullptr) [[unlikely]]
      return;

    glContext->drawElements(mesh.primitiveTopology(),
                            mesh.indices().size(),
                            WEBGL_UNSIGNED_INT,
                            0);
  }

  void Material::onBeforeDrawMesh(shared_ptr<client_graphics::WebGLProgram> program, shared_ptr<Mesh3d> mesh)
  {
  }

  void Material::onAfterDrawMesh(shared_ptr<client_graphics::WebGLProgram> program, shared_ptr<Mesh3d> mesh)
  {
  }

  const vector<string> Material::mixDefines(const vector<string> &baseDefines,
                                            const vector<string> &definesToAdd) const
  {
    vector<string> result = definesToAdd;

    // Ignore duplicates.
    for (const auto &define : baseDefines)
    {
      if (find(result.begin(), result.end(), define) == result.end())
        result.push_back(define);
    }
    return result;
  }
}
