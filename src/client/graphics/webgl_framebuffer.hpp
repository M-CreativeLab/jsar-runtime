#pragma once

#include <map>
#include "common/command_buffers/details/program.hpp"
#include "./webgl_object.hpp"

namespace client_graphics
{
  enum class WebGLFramebufferBindingTarget : uint32_t
  {
    kFramebuffer = WEBGL_FRAMEBUFFER,
    kReadFramebuffer = WEBGL2_READ_FRAMEBUFFER,
    kDrawFramebuffer = WEBGL2_DRAW_FRAMEBUFFER,
  };

  enum class WebGLFramebufferAttachment : uint32_t
  {
    kColorAttachment0 = WEBGL_COLOR_ATTACHMENT0,
    kDepthAttachment = WEBGL_DEPTH_ATTACHMENT,
    kStencilAttachment = WEBGL_STENCIL_ATTACHMENT,
    kDepthStencilAttachment = WEBGL_DEPTH_STENCIL_ATTACHMENT,

    // WebGL2
    kColorAttachment1 = WEBGL2_COLOR_ATTACHMENT1,
    kColorAttachment2 = WEBGL2_COLOR_ATTACHMENT2,
    kColorAttachment3 = WEBGL2_COLOR_ATTACHMENT3,
    kColorAttachment4 = WEBGL2_COLOR_ATTACHMENT4,
    kColorAttachment5 = WEBGL2_COLOR_ATTACHMENT5,
    kColorAttachment6 = WEBGL2_COLOR_ATTACHMENT6,
    kColorAttachment7 = WEBGL2_COLOR_ATTACHMENT7,
    kColorAttachment8 = WEBGL2_COLOR_ATTACHMENT8,
    kColorAttachment9 = WEBGL2_COLOR_ATTACHMENT9,
    kColorAttachment10 = WEBGL2_COLOR_ATTACHMENT10,
    kColorAttachment11 = WEBGL2_COLOR_ATTACHMENT11,
    kColorAttachment12 = WEBGL2_COLOR_ATTACHMENT12,
    kColorAttachment13 = WEBGL2_COLOR_ATTACHMENT13,
    kColorAttachment14 = WEBGL2_COLOR_ATTACHMENT14,
    kColorAttachment15 = WEBGL2_COLOR_ATTACHMENT15,
  };

  /**
   * The type of the framebuffer attachment.
   */
  enum class WebGLFramebufferAttachmentType : uint32_t
  {
    kColor = WEBGL2_COLOR,
    kDepth = WEBGL2_DEPTH,
    kStencil = WEBGL2_STENCIL,
    kDepthStencil = WEBGL2_DEPTH_STENCIL,
  };

  class WebGLFramebuffer : public WebGLObject
  {
  public:
    WebGLFramebuffer()
        : WebGLObject(WebGLObjectType::Framebuffer)
    {
    }

  protected:
    /**
     * Create a new `WebGLFramebuffer` with the given id, that is used for the specific framebuffers
     * like the default framebuffer or the host framebuffer.
     */
    WebGLFramebuffer(int defaultId)
        : WebGLObject(WebGLObjectType::Framebuffer, defaultId)
    {
    }
  };

  /**
   * The host framebuffer is a special framebuffer that is created and managed by the host process,
   * but it could be used by the client process to draw the content.
   */
  class WebGLHostFramebuffer : public WebGLFramebuffer
  {
  public:
    WebGLHostFramebuffer()
        : WebGLFramebuffer(-1)
    {
    }
  };
}
