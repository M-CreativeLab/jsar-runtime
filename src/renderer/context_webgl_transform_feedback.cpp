#include <command_buffers/shared.hpp>
#include <command_buffers/webgl_constants.hpp>
#include <renderer/context_webgl.hpp>
#include <renderer/content_renderer.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  void TrContextWebGL::glBeginTransformFeedback(WebGLenum primitiveMode)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glBindTransformFeedback(WebGLenum target, WebGLuint transformFeedback)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteTransformFeedbacks(WebGLsizei n, const WebGLuint *transformFeedbacks)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glEndTransformFeedback()
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenTransformFeedbacks(WebGLsizei n, WebGLuint *transformFeedbacks)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetTransformFeedbackVarying(WebGLuint program,
                                                     WebGLuint index,
                                                     WebGLsizei bufSize,
                                                     WebGLsizei *length,
                                                     WebGLsizei *size,
                                                     WebGLenum *type,
                                                     WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glIsTransformFeedback(WebGLuint transformFeedback)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glPauseTransformFeedback()
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glResumeTransformFeedback()
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glTransformFeedbackVaryings(WebGLuint program,
                                                   WebGLsizei count,
                                                   const WebGLchar **varyings,
                                                   WebGLenum bufferMode)
  {
    /* TODO(yorkie): implement */
  }
}
