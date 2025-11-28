#include <command_buffers/shared.hpp>
#include <command_buffers/webgl_constants.hpp>
#include <renderer/context_webgl.hpp>
#include <renderer/content_renderer.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  // --- Rendering ---
  void TrContextWebGL::glClear(WebGLbitfield mask)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glClearBufferiv(WebGLenum buffer,
                                       WebGLint drawbuffer,
                                       const WebGLint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glClearBufferuiv(WebGLenum buffer,
                                        WebGLint drawbuffer,
                                        const WebGLuint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glClearBufferfv(WebGLenum buffer,
                                       WebGLint drawbuffer,
                                       const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glClearBufferfi(WebGLenum buffer,
                                       WebGLint drawbuffer,
                                       WebGLfloat depth,
                                       WebGLint stencil)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glClearColor(WebGLfloat red,
                                    WebGLfloat green,
                                    WebGLfloat blue,
                                    WebGLfloat alpha)
  {
    clear_color_[0] = red;
    clear_color_[1] = green;
    clear_color_[2] = blue;
    clear_color_[3] = alpha;
  }

  void TrContextWebGL::glClearDepthf(WebGLfloat depth)
  {
    clear_depth_ = depth;
  }

  void TrContextWebGL::glClearStencil(WebGLint s)
  {
    clear_stencil_ = s;
  }

  void TrContextWebGL::glFinish()
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glFlush()
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glReadBuffer(WebGLenum buffer)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glReadPixels(WebGLint x,
                                    WebGLint y,
                                    WebGLsizei width,
                                    WebGLsizei height,
                                    WebGLenum format,
                                    WebGLenum type,
                                    WebGLvoid *pixels)
  {
    /* TODO(yorkie): implement */
  }
}
