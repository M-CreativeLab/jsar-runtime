#include "./framebuffer.hpp"

using namespace std;
using TextureAttachmentInit = GLFramebufferAttachment::TextureAttachmentInit;

void GLFramebuffer::setupAttachmentsFromCurrent()
{
  color_attachment_ = GLFramebufferAttachment::FromCurrent(GL_COLOR_ATTACHMENT0);
  depth_stencil_attachment_ = GLFramebufferAttachment::FromCurrent(GL_DEPTH_STENCIL_ATTACHMENT);
}

void GLFramebuffer::print(const char *prefix) const
{
  bool completed = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
  DEBUG(LOG_TAG_RENDERER,
        "%s framebuffer(%d, %s):",
        prefix,
        id_,
        completed ? "COMPLETE" : "INCOMPLETE");
  if (hasColorAttachment())
    color_attachment_->print("Color Attachment");
  if (hasDepthStencilAttachment())
    depth_stencil_attachment_->print("Depth-Stencil Attachment");
}

void GLFramebuffer::dumpPixels()
{
}
