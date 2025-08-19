#pragma once

#include "./instanced_mesh.hpp"
#include "./meshes.hpp"

namespace builtin_scene
{
  template <typename MeshType>
    requires std::is_base_of<Mesh, MeshType>::value
  void InstancedMesh<MeshType>::onMesh3dInitialized(std::shared_ptr<Mesh3d> mesh3d,
                                                    std::shared_ptr<client_graphics::WebGL2Context> glContext)
  {
    MeshType::onMesh3dInitialized(mesh3d, glContext);

    setup(glContext, mesh3d);
  }

  template <typename MeshType>
    requires std::is_base_of<Mesh, MeshType>::value
  void InstancedMesh<MeshType>::onConfigureVertexAttribs(std::shared_ptr<client_graphics::WebGLProgram> program)
  {
    configureVertexAttribs(program);
  }
}
