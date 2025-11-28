#include <command_buffers/shared.hpp>
#include <command_buffers/webgl_constants.hpp>
#include <renderer/context_webgl.hpp>
#include <renderer/content_renderer.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  void TrContextWebGL::glGetInternalformativ(WebGLenum target, WebGLenum internalformat, WebGLsizei propCount, WebGLenum *props, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetString(WebGLenum pname, WebGLchar *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetStringi(WebGLenum pname, WebGLuint index)
  {
    /* TODO(yorkie): implement */
  }
}
