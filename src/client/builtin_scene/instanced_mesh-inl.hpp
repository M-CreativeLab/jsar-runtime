#pragma once

#include "./instanced_mesh.hpp"
#include "./meshes.hpp"

namespace builtin_scene
{
  template <typename MeshType>
    requires std::is_base_of<Mesh, MeshType>::value
  void InstancedMesh<MeshType>::onMesh3dInitialized(const Mesh3d &mesh3d,
                                                    std::shared_ptr<client_graphics::WebGL2Context> glContext)
  {
    MeshType::onMesh3dInitialized(mesh3d, glContext);

    using namespace client_graphics;

    /**
       * For WebContent instanced mesh, we need to achieve the following:
       *
       * - vao as the opaque mesh vertex array object.
       * - creating a new VAO as the transparent mesh vertex array object.
       */
    auto transparentVao = glContext->createVertexArray();
    {
      WebGLVertexArrayScope vaoScope(glContext, transparentVao);
      glContext->bindBuffer(WebGLBufferBindingTarget::kElementArrayBuffer, mesh3d.elementBufferObject());
    }
    setup(glContext,
          mesh3d.vertexArrayObject(),
          glContext->createBuffer(),
          transparentVao,
          glContext->createBuffer());
  }
}
