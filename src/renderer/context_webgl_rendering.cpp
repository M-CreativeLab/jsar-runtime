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
    if (mask == 0)
      return;

    const bool clearColor = (mask & WEBGL_COLOR_BUFFER_BIT) != 0;
    const bool clearDepth = (mask & WEBGL_DEPTH_BUFFER_BIT) != 0;
    const bool clearStencil = (mask & WEBGL_STENCIL_BUFFER_BIT) != 0;

    if (!clearColor && !clearDepth && !clearStencil)
    {
      last_error_ = WEBGL_INVALID_VALUE;
      return;
    }

    if (clearColor)
    {
      caps_.applyColorMask();
    }

    auto pass = getCurrentRenderPass();
    if (pass)
    {
      const float rgba[4] = {
        clear_color_[0],
        clear_color_[1],
        clear_color_[2],
        clear_color_[3],
      };
      pass->clearAttachments(clearColor,
                             clearDepth,
                             clearStencil,
                             rgba,
                             clear_depth_,
                             clear_stencil_);
    }
  }

  void TrContextWebGL::glClearBufferiv(WebGLenum buffer,
                                       WebGLint drawbuffer,
                                       const WebGLint *value)
  {
    if (value == nullptr)
      return;
    if (drawbuffer != 0)
    {
      last_error_ = WEBGL_INVALID_VALUE;
      return;
    }

    if (buffer == WEBGL2_COLOR)
    {
      clear_color_[0] = static_cast<WebGLfloat>(value[0]);
      clear_color_[1] = static_cast<WebGLfloat>(value[1]);
      clear_color_[2] = static_cast<WebGLfloat>(value[2]);
      clear_color_[3] = static_cast<WebGLfloat>(value[3]);
      auto pass = getCurrentRenderPass();
      if (pass)
      {
        const float rgba[4] = {clear_color_[0], clear_color_[1], clear_color_[2], clear_color_[3]};
        pass->clearAttachments(true, false, false, rgba, clear_depth_, clear_stencil_);
      }
    }
    else if (buffer == WEBGL2_DEPTH)
    {
      clear_depth_ = static_cast<WebGLfloat>(value[0]);
      auto pass = getCurrentRenderPass();
      if (pass)
      {
        const float rgba[4] = {clear_color_[0], clear_color_[1], clear_color_[2], clear_color_[3]};
        pass->clearAttachments(false, true, false, rgba, clear_depth_, clear_stencil_);
      }
    }
    else if (buffer == WEBGL2_STENCIL)
    {
      clear_stencil_ = static_cast<WebGLint>(value[0]);
      auto pass = getCurrentRenderPass();
      if (pass)
      {
        const float rgba[4] = {clear_color_[0], clear_color_[1], clear_color_[2], clear_color_[3]};
        pass->clearAttachments(false, false, true, rgba, clear_depth_, clear_stencil_);
      }
    }
    else
    {
      last_error_ = WEBGL_INVALID_ENUM;
    }
  }

  void TrContextWebGL::glClearBufferuiv(WebGLenum buffer,
                                        WebGLint drawbuffer,
                                        const WebGLuint *value)
  {
    if (value == nullptr)
      return;
    if (drawbuffer != 0)
    {
      last_error_ = WEBGL_INVALID_VALUE;
      return;
    }

    if (buffer == WEBGL2_COLOR)
    {
      clear_color_[0] = static_cast<WebGLfloat>(value[0]);
      clear_color_[1] = static_cast<WebGLfloat>(value[1]);
      clear_color_[2] = static_cast<WebGLfloat>(value[2]);
      clear_color_[3] = static_cast<WebGLfloat>(value[3]);
      auto pass = getCurrentRenderPass();
      if (pass)
      {
        const float rgba[4] = {clear_color_[0], clear_color_[1], clear_color_[2], clear_color_[3]};
        pass->clearAttachments(true, false, false, rgba, clear_depth_, clear_stencil_);
      }
    }
    else if (buffer == WEBGL2_DEPTH)
    {
      clear_depth_ = static_cast<WebGLfloat>(value[0]);
      auto pass = getCurrentRenderPass();
      if (pass)
      {
        const float rgba[4] = {clear_color_[0], clear_color_[1], clear_color_[2], clear_color_[3]};
        pass->clearAttachments(false, true, false, rgba, clear_depth_, clear_stencil_);
      }
    }
    else if (buffer == WEBGL2_STENCIL)
    {
      clear_stencil_ = static_cast<WebGLint>(value[0]);
      auto pass = getCurrentRenderPass();
      if (pass)
      {
        const float rgba[4] = {clear_color_[0], clear_color_[1], clear_color_[2], clear_color_[3]};
        pass->clearAttachments(false, false, true, rgba, clear_depth_, clear_stencil_);
      }
    }
    else
    {
      last_error_ = WEBGL_INVALID_ENUM;
    }
  }

  void TrContextWebGL::glClearBufferfv(WebGLenum buffer,
                                       WebGLint drawbuffer,
                                       const WebGLfloat *value)
  {
    if (value == nullptr)
      return;
    if (drawbuffer != 0)
    {
      last_error_ = WEBGL_INVALID_VALUE;
      return;
    }

    if (buffer == WEBGL2_COLOR)
    {
      clear_color_[0] = value[0];
      clear_color_[1] = value[1];
      clear_color_[2] = value[2];
      clear_color_[3] = value[3];
      auto pass = getCurrentRenderPass();
      if (pass)
      {
        const float rgba[4] = {clear_color_[0], clear_color_[1], clear_color_[2], clear_color_[3]};
        pass->clearAttachments(true, false, false, rgba, clear_depth_, clear_stencil_);
      }
    }
    else if (buffer == WEBGL2_DEPTH)
    {
      clear_depth_ = value[0];
      auto pass = getCurrentRenderPass();
      if (pass)
      {
        const float rgba[4] = {clear_color_[0], clear_color_[1], clear_color_[2], clear_color_[3]};
        pass->clearAttachments(false, true, false, rgba, clear_depth_, clear_stencil_);
      }
    }
    else if (buffer == WEBGL2_STENCIL)
    {
      clear_stencil_ = static_cast<WebGLint>(value[0]);
      auto pass = getCurrentRenderPass();
      if (pass)
      {
        const float rgba[4] = {clear_color_[0], clear_color_[1], clear_color_[2], clear_color_[3]};
        pass->clearAttachments(false, false, true, rgba, clear_depth_, clear_stencil_);
      }
    }
    else
    {
      last_error_ = WEBGL_INVALID_ENUM;
    }
  }

  void TrContextWebGL::glClearBufferfi(WebGLenum buffer,
                                       WebGLint drawbuffer,
                                       WebGLfloat depth,
                                       WebGLint stencil)
  {
    if (drawbuffer != 0)
    {
      last_error_ = WEBGL_INVALID_VALUE;
      return;
    }
    if (buffer != WEBGL_DEPTH_STENCIL)
    {
      last_error_ = WEBGL_INVALID_ENUM;
      return;
    }
    clear_depth_ = depth;
    clear_stencil_ = stencil;
    auto pass = getCurrentRenderPass();
    if (pass)
    {
      const float rgba[4] = {clear_color_[0], clear_color_[1], clear_color_[2], clear_color_[3]};
      pass->clearAttachments(false, true, true, rgba, clear_depth_, clear_stencil_);
    }
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
    (void)0;
  }

  void TrContextWebGL::glFlush()
  {
    (void)0;
  }

  void TrContextWebGL::glReadBuffer(WebGLenum buffer)
  {
    (void)buffer;
  }

  void TrContextWebGL::glReadPixels(WebGLint x,
                                    WebGLint y,
                                    WebGLsizei width,
                                    WebGLsizei height,
                                    WebGLenum format,
                                    WebGLenum type,
                                    WebGLvoid *pixels)
  {
    (void)x;
    (void)y;
    (void)width;
    (void)height;
    (void)format;
    (void)type;
    (void)pixels;
  }
}
