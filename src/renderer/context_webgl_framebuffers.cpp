#include <command_buffers/shared.hpp>
#include <command_buffers/webgl_constants.hpp>
#include <renderer/context_webgl.hpp>
#include <renderer/content_renderer.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  void TrContextWebGL::glBindFramebuffer(WebGLenum target, WebGLuint id)
  {
    if (target != WEBGL_FRAMEBUFFER &&
        target != WEBGL2_DRAW_FRAMEBUFFER &&
        target != WEBGL2_READ_FRAMEBUFFER) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_ENUM;
      return;
    }

    auto framebuffer_target = details::FramebufferTarget(target);
    if (id == 0)
    {
      framebuffer_bindings_[framebuffer_target] = nullptr;

      if (target == WEBGL_FRAMEBUFFER || target == WEBGL2_DRAW_FRAMEBUFFER)
      {
        current_render_pass_ = content_renderer_->opaqueRenderPass();
        current_render_pass_->bindTarget(0);
      }
      return;
    }

    auto framebuffer = framebuffers_.get(id);
    if (!framebuffer) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_FRAMEBUFFER_OPERATION;
      return;
    }

    framebuffer_bindings_[framebuffer_target] = framebuffer;

    if (target == WEBGL_FRAMEBUFFER || target == WEBGL2_DRAW_FRAMEBUFFER)
    {
      current_render_pass_ = content_renderer_->offscreenRenderPass();
      current_render_pass_->bindTarget(id);
      current_render_pass_->setColorAttachmentCount(1);
      current_render_pass_->ensureDepthStencilAttachment();
    }
  }

  void TrContextWebGL::glBindRenderbuffer(WebGLenum target, WebGLuint id)
  {
    auto renderbuffer = renderbuffers_.get(id);
    if (!renderbuffer) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_FRAMEBUFFER_OPERATION;
      return;
    }

    if (target != WEBGL_RENDERBUFFER) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_ENUM;
      return;
    }

    auto renderbuffer_target = details::RenderbufferTarget(target);
    renderbuffer_bindings_[renderbuffer_target] = renderbuffer;
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
    (void)srcX0;
    (void)srcY0;
    (void)srcX1;
    (void)srcY1;
    (void)dstX0;
    (void)dstY0;
    (void)dstX1;
    (void)dstY1;
    (void)mask;
    (void)filter;
  }

  void TrContextWebGL::glCheckFramebufferStatus(WebGLenum target)
  {
    (void)target;
  }

  void TrContextWebGL::glDeleteFramebuffers(WebGLsizei n, const WebGLuint *framebuffers)
  {
    if (n <= 0 || framebuffers == nullptr)
      return;
    for (WebGLsizei i = 0; i < n; i++)
    {
      auto offscreen = content_renderer_->offscreenRenderPass();
      if (offscreen)
        offscreen->discardTarget(framebuffers[i]);
      framebuffers_.remove(framebuffers[i]);
    }
  }

  void TrContextWebGL::glDeleteRenderbuffers(WebGLsizei n, const WebGLuint *renderbuffers)
  {
    if (n <= 0 || renderbuffers == nullptr)
      return;
    for (WebGLsizei i = 0; i < n; i++)
      renderbuffers_.remove(renderbuffers[i]);
  }

  void TrContextWebGL::glDrawBuffers(WebGLsizei n, const WebGLenum *buffers)
  {
    (void)buffers;
    if (n < 0)
      return;
    auto pass = getCurrentRenderPass();
    if (pass)
      pass->setColorAttachmentCount(static_cast<size_t>(n));
  }

  void TrContextWebGL::glFramebufferRenderbuffer(WebGLenum target, WebGLenum attachment, WebGLenum renderbuffertarget, WebGLuint renderbuffer)
  {
    if (target != WEBGL_FRAMEBUFFER &&
        target != WEBGL2_DRAW_FRAMEBUFFER &&
        target != WEBGL2_READ_FRAMEBUFFER)
    {
      last_error_ = WEBGL_INVALID_ENUM;
      return;
    }
    if (renderbuffertarget != WEBGL_RENDERBUFFER)
    {
      last_error_ = WEBGL_INVALID_ENUM;
      return;
    }
    auto fb_target = details::FramebufferTarget(target);
    auto framebuffer = framebuffer_bindings_[fb_target];
    if (!framebuffer)
    {
      last_error_ = WEBGL_INVALID_FRAMEBUFFER_OPERATION;
      return;
    }
    if (renderbuffer != 0)
    {
      auto rb = renderbuffers_.get(renderbuffer);
      if (!rb)
      {
        last_error_ = WEBGL_INVALID_OPERATION;
        return;
      }
      if (attachment == WEBGL_DEPTH_ATTACHMENT || attachment == WEBGL_DEPTH_STENCIL_ATTACHMENT)
      {
        if (!framebuffer->depthAttachment)
          framebuffer->depthAttachment = AcquireRef(new details::FramebufferAttachment());
        framebuffer->depthAttachment->renderbuffer = rb;
      }
      if (attachment == WEBGL_STENCIL_ATTACHMENT || attachment == WEBGL_DEPTH_STENCIL_ATTACHMENT)
      {
        if (!framebuffer->stencilAttachment)
          framebuffer->stencilAttachment = AcquireRef(new details::FramebufferAttachment());
        framebuffer->stencilAttachment->renderbuffer = rb;
      }
      auto pass = content_renderer_->offscreenRenderPass();
      if (pass)
      {
        pass->bindTarget(framebuffer->id);
        pass->ensureDepthStencilAttachment();
      }
    }
    else
    {
      if (attachment == WEBGL_DEPTH_ATTACHMENT)
        framebuffer->depthAttachment = nullptr;
      else if (attachment == WEBGL_STENCIL_ATTACHMENT)
        framebuffer->stencilAttachment = nullptr;
      else if (attachment == WEBGL_DEPTH_STENCIL_ATTACHMENT)
        framebuffer->depthAttachment = framebuffer->stencilAttachment = nullptr;
    }
  }

  void TrContextWebGL::glFramebufferTexture2D(WebGLenum target, WebGLenum attachment, WebGLenum textarget, WebGLuint texture, WebGLint level)
  {
    if (target != WEBGL_FRAMEBUFFER &&
        target != WEBGL2_DRAW_FRAMEBUFFER &&
        target != WEBGL2_READ_FRAMEBUFFER)
    {
      last_error_ = WEBGL_INVALID_ENUM;
      return;
    }
    if (textarget != WEBGL_TEXTURE_2D &&
        textarget != WEBGL2_TEXTURE_3D &&
        textarget != WEBGL2_TEXTURE_2D_ARRAY)
    {
      last_error_ = WEBGL_INVALID_ENUM;
      return;
    }
    auto fb_target = details::FramebufferTarget(target);
    auto framebuffer = framebuffer_bindings_[fb_target];
    if (!framebuffer)
    {
      last_error_ = WEBGL_INVALID_FRAMEBUFFER_OPERATION;
      return;
    }
    if (texture != 0)
    {
      auto tex = textures_.get(texture);
      if (!tex)
      {
        last_error_ = WEBGL_INVALID_OPERATION;
        return;
      }

      if (!framebuffer->colorAttachment)
        framebuffer->colorAttachment = AcquireRef(new details::FramebufferAttachment());
      framebuffer->colorAttachment->texture = tex;
      (void)level;
      (void)attachment;
    }
    else
    {
      framebuffer->colorAttachment = nullptr;
      (void)attachment;
      (void)level;
    }
  }

  void TrContextWebGL::glFramebufferTextureLayer(WebGLenum target, WebGLenum attachment, WebGLuint texture, WebGLint level, WebGLint layer)
  {
    if (target != WEBGL_FRAMEBUFFER &&
        target != WEBGL2_DRAW_FRAMEBUFFER &&
        target != WEBGL2_READ_FRAMEBUFFER)
    {
      last_error_ = WEBGL_INVALID_ENUM;
      return;
    }
    auto fb_target = details::FramebufferTarget(target);
    auto framebuffer = framebuffer_bindings_[fb_target];
    if (!framebuffer)
    {
      last_error_ = WEBGL_INVALID_FRAMEBUFFER_OPERATION;
      return;
    }
    if (texture != 0)
    {
      auto tex = textures_.get(texture);
      if (!tex)
      {
        last_error_ = WEBGL_INVALID_OPERATION;
        return;
      }
      if (!framebuffer->colorAttachment)
        framebuffer->colorAttachment = AcquireRef(new details::FramebufferAttachment());
      framebuffer->colorAttachment->texture = tex;
    }
    else
    {
      framebuffer->colorAttachment = nullptr;
    }
    (void)attachment;
    (void)level;
    (void)layer;
  }

  void TrContextWebGL::glGenFramebuffers(WebGLsizei n, WebGLuint *framebuffers)
  {
    glGenTypedObjects(framebuffers_, n, framebuffers);
  }

  void TrContextWebGL::glGenRenderbuffers(WebGLsizei n, WebGLuint *renderbuffers)
  {
    glGenTypedObjects(renderbuffers_, n, renderbuffers);
  }

  void TrContextWebGL::glGenerateMipmap(WebGLenum target)
  {
    (void)target;
  }

  void TrContextWebGL::glGetFramebufferAttachmentParameteriv(WebGLenum target, WebGLenum attachment, WebGLenum pname, WebGLint *params)
  {
    if (params == nullptr)
      return;
    *params = 0;
    (void)target;
    (void)attachment;
    (void)pname;
  }

  void TrContextWebGL::glGetRenderbufferParameteriv(WebGLenum target, WebGLenum pname, WebGLint *params)
  {
    if (params == nullptr)
      return;
    *params = 0;
    (void)target;
    (void)pname;
  }

  void TrContextWebGL::glInvalidateFramebuffer(WebGLenum target, WebGLsizei n, const WebGLenum *attachments)
  {
    (void)target;
    (void)n;
    (void)attachments;
  }

  void TrContextWebGL::glInvalidateSubFramebuffer(WebGLenum target,
                                                  WebGLsizei n,
                                                  const WebGLenum *attachments,
                                                  WebGLint x,
                                                  WebGLint y,
                                                  WebGLsizei width,
                                                  WebGLsizei height)
  {
    (void)target;
    (void)n;
    (void)attachments;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
  }

  WebGLboolean TrContextWebGL::glIsFramebuffer(WebGLuint framebuffer)
  {
    return framebuffers_.has(framebuffer);
  }

  WebGLboolean TrContextWebGL::glIsRenderbuffer(WebGLuint renderbuffer)
  {
    return renderbuffers_.has(renderbuffer);
  }

  void TrContextWebGL::glRenderbufferStorage(WebGLenum target, WebGLenum internalformat, WebGLsizei width, WebGLsizei height)
  {
    (void)target;
    (void)internalformat;
    (void)width;
    (void)height;
  }

  void TrContextWebGL::glRenderbufferStorageMultisample(WebGLenum target,
                                                        WebGLsizei samples,
                                                        WebGLenum internalformat,
                                                        WebGLsizei width,
                                                        WebGLsizei height)
  {
    (void)target;
    (void)samples;
    (void)internalformat;
    (void)width;
    (void)height;
  }
}
