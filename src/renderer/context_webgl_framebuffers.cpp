#include <command_buffers/shared.hpp>
#include <command_buffers/webgl_constants.hpp>
#include <renderer/context_webgl.hpp>
#include <renderer/content_renderer.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  void TrContextWebGL::glBindFramebuffer(WebGLenum target, WebGLuint framebuffer)
  {
    if (!glIsFramebuffer(framebuffer)) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_FRAMEBUFFER_OPERATION;
      return;
    }

    if (target != WEBGL_FRAMEBUFFER &&
        target != WEBGL2_DRAW_FRAMEBUFFER &&
        target != WEBGL2_READ_FRAMEBUFFER) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_ENUM;
      return;
    }

    for (auto &binding : framebuffer_bindings_)
    {
      if (binding.framebuffer == framebuffer)
      {
        binding.target = target;
        break;
      }
    }
  }

  void TrContextWebGL::glBindRenderbuffer(WebGLenum target, WebGLuint renderbuffer)
  {
    if (!glIsRenderbuffer(renderbuffer)) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_FRAMEBUFFER_OPERATION;
      return;
    }

    if (target != WEBGL_RENDERBUFFER) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_ENUM;
      return;
    }

    for (auto &binding : renderbuffer_bindings_)
    {
      if (binding.renderbuffer == renderbuffer)
      {
        binding.target = target;
        break;
      }
    }
  }

  void TrContextWebGL::glBlitFramebuffer(WebGLint srcX0,
                                         WebGLint srcY0,
                                         WebGLint srcX1,
                                         WebGLint srcY1,
                                         WebGLint dstX0,
                                         WebGLint dstY0,
                                         WebGLint dstX1,
                                         WebGLint dstY1,
                                         WebGLbitfield mask,
                                         WebGLenum filter)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glCheckFramebufferStatus(WebGLenum target)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteFramebuffers(WebGLsizei n, const WebGLuint *framebuffers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteRenderbuffers(WebGLsizei n, const WebGLuint *renderbuffers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDrawBuffers(WebGLsizei n, const WebGLenum *buffers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glFramebufferRenderbuffer(WebGLenum target, WebGLenum attachment, WebGLenum renderbuffertarget, WebGLuint renderbuffer)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glFramebufferTexture2D(WebGLenum target, WebGLenum attachment, WebGLenum textarget, WebGLuint texture, WebGLint level)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glFramebufferTextureLayer(WebGLenum target, WebGLenum attachment, WebGLuint texture, WebGLint level, WebGLint layer)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenFramebuffers(WebGLsizei n, WebGLuint *framebuffers)
  {
    glGenObjects(framebuffers_, n, framebuffers);
  }

  void TrContextWebGL::glGenRenderbuffers(WebGLsizei n, WebGLuint *renderbuffers)
  {
    glGenObjects(renderbuffers_, n, renderbuffers);
  }

  void TrContextWebGL::glGenerateMipmap(WebGLenum target)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetFramebufferAttachmentParameteriv(WebGLenum target, WebGLenum attachment, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetRenderbufferParameteriv(WebGLenum target, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glInvalidateFramebuffer(WebGLenum target, WebGLsizei n, const WebGLenum *attachments)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glInvalidateSubFramebuffer(WebGLenum target,
                                                  WebGLsizei n,
                                                  const WebGLenum *attachments,
                                                  WebGLint x,
                                                  WebGLint y,
                                                  WebGLsizei width,
                                                  WebGLsizei height)
  {
    /* TODO(yorkie): implement */
  }

  WebGLboolean TrContextWebGL::glIsFramebuffer(WebGLuint framebuffer)
  {
    for (const auto &binding : framebuffer_bindings_)
    {
      if (binding.framebuffer == framebuffer)
      {
        return true;
      }
    }
    return false;
  }

  WebGLboolean TrContextWebGL::glIsRenderbuffer(WebGLuint renderbuffer)
  {
    for (const auto &binding : renderbuffer_bindings_)
    {
      if (binding.renderbuffer == renderbuffer)
      {
        return true;
      }
    }
    return false;
  }

  void TrContextWebGL::glRenderbufferStorage(WebGLenum target, WebGLenum internalformat, WebGLsizei width, WebGLsizei height)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glRenderbufferStorageMultisample(WebGLenum target,
                                                        WebGLsizei samples,
                                                        WebGLenum internalformat,
                                                        WebGLsizei width,
                                                        WebGLsizei height)
  {
    /* TODO(yorkie): implement */
  }
}
