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
    auto vao = vertex_array_objects_.get(array);
    if (!vao) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }
    current_vertex_array_object_ = vao;
  }

  void TrContextWebGL::glDeleteVertexArrays(WebGLsizei n, const WebGLuint *arrays)
  {
    if (n <= 0 || arrays == nullptr)
      return;

    for (WebGLsizei i = 0; i < n; i++)
    {
      if (current_vertex_array_object_ && current_vertex_array_object_->id == arrays[i])
        current_vertex_array_object_ = nullptr;
      vertex_array_objects_.remove(arrays[i]);
    }
  }

  void TrContextWebGL::glGenVertexArrays(WebGLsizei n, WebGLuint *arrays)
  {
    if (n <= 0 || arrays == nullptr)
      return;

    size_t size_before = vertex_array_objects_.size();
    glGenTypedObjects(vertex_array_objects_, n, arrays);

    for (WebGLsizei i = 0; i < n; i++)
    {
      auto &vao = vertex_array_objects_[size_before + static_cast<size_t>(i)];
      vao->vertex_state.entryPoint = "main";
      vao->vertex_state.constantCount = 0;
      vao->vertex_state.constants = nullptr;
      vao->vertex_state.bufferCount = 0;
      vao->vertex_state.buffers = nullptr;
      vao->buffer_layouts.clear();
      vao->attributes.clear();
    }
  }

  WebGLboolean TrContextWebGL::glIsVertexArray(WebGLuint array)
  {
    return vertex_array_objects_.has(array);
  }
}
