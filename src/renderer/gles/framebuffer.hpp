#pragma once

#include <optional>

#include "./common.hpp"
#include "./framebuffer_attachment.hpp"

class GLFramebuffer
{
public:
  GLFramebuffer(GLuint id, bool should_release_on_destructor)
      : id_(id)
      , color_attachment_(nullptr)
      , depth_stencil_attachment_(nullptr)
  {
    assert(id_ > 0 && "Invalid framebuffer ID.");
  }
  ~GLFramebuffer()
  {
    if (should_release_on_destructor_ && id_ > 0)
    {
      glDeleteFramebuffers(1, &id_);
      id_ = 0;
      DEBUG(LOG_TAG_ERROR, "Deleted framebuffer: %u", id_);
    }
  }

public:
  inline GLuint id() const
  {
    return id_;
  }
  inline const GLFramebufferAttachment &colorAttachment() const
  {
    assert(color_attachment_ != nullptr && "No color attachment available.");
    return *color_attachment_;
  }
  inline const GLFramebufferAttachment &depthStencilAttachment() const
  {
    assert(depth_stencil_attachment_ != nullptr && "No depth-stencil attachment available.");
    return *depth_stencil_attachment_;
  }
  inline bool hasColorAttachment() const
  {
    return color_attachment_ != nullptr && color_attachment_->id() > 0;
  }
  inline bool hasDepthStencilAttachment() const
  {
    return depth_stencil_attachment_ != nullptr && depth_stencil_attachment_->id() > 0;
  }
  inline bool isColorAttachmentOnly() const
  {
    return hasColorAttachment() && !hasDepthStencilAttachment();
  }

  // Setup the color and depth-stencil attachments from the current framebuffer.
  void setupAttachmentsFromCurrent();

  void print(const char *prefix) const;
  void dumpPixels();

private:
  GLuint id_;
  bool should_release_on_destructor_ = false;

  std::shared_ptr<GLFramebufferAttachment> color_attachment_;
  std::shared_ptr<GLFramebufferAttachment> depth_stencil_attachment_;
};

class GLFramebufferScope
{
public:
  GLFramebufferScope(GLenum target, const GLFramebuffer &framebuffer)
      : target_(target)
  {
    assert(target == GL_FRAMEBUFFER ||
           target == GL_DRAW_FRAMEBUFFER ||
           target == GL_READ_FRAMEBUFFER &&
             "Invalid framebuffer target.");
    assert(framebuffer.id() > 0 && "Invalid framebuffer ID.");

    if (target == GL_FRAMEBUFFER || GL_DRAW_FRAMEBUFFER)
      glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint *)&last_framebuffer_binding_);
    else if (target == GL_READ_FRAMEBUFFER)
      glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, (GLint *)&last_framebuffer_binding_);
    glBindFramebuffer(target, framebuffer.id());
  }
  ~GLFramebufferScope()
  {
    glBindFramebuffer(target_, last_framebuffer_binding_);
  }

private:
  GLenum target_;
  GLuint last_framebuffer_binding_ = 0;
};

using GLRenderTarget = GLFramebuffer;
