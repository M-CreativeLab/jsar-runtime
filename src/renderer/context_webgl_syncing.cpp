#include <command_buffers/shared.hpp>
#include <command_buffers/webgl_constants.hpp>
#include <renderer/context_webgl.hpp>
#include <renderer/content_renderer.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  void TrContextWebGL::glClientWaitSync(WebGLsync sync, WebGLbitfield flags, WebGLuint64 timeout)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteSync(WebGLsync sync)
  {
    /* TODO(yorkie): implement */
  }

  WebGLsync TrContextWebGL::glFenceSync(WebGLenum condition, WebGLbitfield flags)
  {
    /* TODO(yorkie): implement */
    return nullptr;
  }

  void TrContextWebGL::glGetSynciv(WebGLsync sync, WebGLenum pname, WebGLsizei bufSize, WebGLsizei *length, WebGLint *values)
  {
    /* TODO(yorkie): implement */
  }

  WebGLboolean TrContextWebGL::glIsSync(WebGLsync sync)
  {
    /* TODO(yorkie): implement */
    return false;
  }

  void TrContextWebGL::glWaitSync(WebGLsync sync, WebGLbitfield flags, WebGLuint64 timeout)
  {
    /* TODO(yorkie): implement */
  }
}
