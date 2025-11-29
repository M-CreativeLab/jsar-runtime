#include <command_buffers/shared.hpp>
#include <command_buffers/webgl_constants.hpp>
#include <renderer/context_webgl.hpp>
#include <renderer/content_renderer.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  void TrContextWebGL::glBindVertexArray(WebGLuint array)
  {
    current_vertex_array_object_ = vertex_array_objects_.get(array);
  }

  void TrContextWebGL::glDeleteVertexArrays(WebGLsizei n, const WebGLuint *arrays)
  {
    // vertex_array_objects_.remove(arrays, n);
  }

  void TrContextWebGL::glGenVertexArrays(WebGLsizei n, WebGLuint *arrays)
  {
    glGenTypedObjects(vertex_array_objects_, n, arrays);
  }

  WebGLboolean TrContextWebGL::glIsVertexArray(WebGLuint array)
  {
    return vertex_array_objects_.has(array);
  }
}
