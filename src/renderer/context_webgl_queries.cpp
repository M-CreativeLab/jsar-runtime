#include <command_buffers/shared.hpp>
#include <command_buffers/webgl_constants.hpp>
#include <renderer/context_webgl.hpp>
#include <renderer/content_renderer.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  void TrContextWebGL::glBeginQuery(WebGLenum target, WebGLuint id)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteQueries(WebGLsizei n, const WebGLuint *ids)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glEndQuery(WebGLenum target)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenQueries(WebGLsizei n, WebGLuint *ids)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetQueryObjectuiv(WebGLuint id, WebGLenum pname, WebGLuint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetQueryiv(WebGLenum target, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glIsQuery(WebGLuint id)
  {
    /* TODO(yorkie): implement */
  }
}
