#include "./framebuffer_attachment.hpp"

using namespace std;

#define FRAMEBUFFER_ATTACHMENT_TEXTURE_SAMPLES_EXT 0x8D6C
#define FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR 0x9630
#define FRAMEBUFFER_ATTACHMENT_TEXTURE_BASE_VIEW_INDEX_OVR 0x9632

unique_ptr<GLFramebufferAttachment> GLFramebufferAttachment::FromCurrent(GLenum attachment)
{
  assert(attachment == GL_COLOR_ATTACHMENT0 ||
         attachment == GL_COLOR_ATTACHMENT1 ||
         attachment == GL_COLOR_ATTACHMENT2 ||
         attachment == GL_COLOR_ATTACHMENT3 ||
         attachment == GL_DEPTH_STENCIL_ATTACHMENT ||
         attachment == GL_DEPTH_ATTACHMENT ||
         attachment == GL_STENCIL_ATTACHMENT &&
           "Invalid framebuffer attachment type.");

  GLint current_framebuffer;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current_framebuffer);
  assert(current_framebuffer != 0 &&
         "No framebuffer is bound, cannot get attachment.");

  GLint attachment_type = 0;
  glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                        attachment,
                                        GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
                                        &attachment_type);
  auto err = glGetError();
  if (err != GL_NO_ERROR)
    DEBUG(LOG_TAG_ERROR, "Failed to get framebuffer attachment type: 0x%04X", err);

  GLFramebufferAttachment *attachment_object = nullptr;
  if (attachment_type == GL_RENDERBUFFER)
  {
    GLuint attachment_rbo = 0;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                          attachment,
                                          GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                          (GLint *)&attachment_rbo);

    GLint num_samples = 0;
    glBindRenderbuffer(GL_RENDERBUFFER, attachment_rbo);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_SAMPLES, &num_samples);
    attachment_object = new GLFramebufferAttachment(kRenderbuffer, attachment_rbo, num_samples, false);
    {
      // Read the renderbuffer's width and height
      glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &attachment_object->width_);
      glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &attachment_object->height_);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
  }
  else if (attachment_type == GL_TEXTURE)
  {
    GLuint attachment_texture = 0;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                          attachment,
                                          GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                          (GLint *)&attachment_texture);

    GLint num_samples = 0;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                          attachment,
                                          FRAMEBUFFER_ATTACHMENT_TEXTURE_SAMPLES_EXT,
                                          &num_samples);

    attachment_object = new GLFramebufferAttachment(kTexture, attachment_texture, num_samples, false);

    // Check if the texture is a multiview texture
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                          attachment,
                                          FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR,
                                          &attachment_object->num_views_);
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                          attachment,
                                          FRAMEBUFFER_ATTACHMENT_TEXTURE_BASE_VIEW_INDEX_OVR,
                                          &attachment_object->base_view_index_);

    GLint current_texture_binding = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &current_texture_binding);

    // Check if the texture is GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_2D_ARRAY or GL_TEXTURE_2D
    // TODO(yorkie): multiview must use TEXTURE_2D_ARRAY?
    GLint is_texture_array = 0;
    glBindTexture(GL_TEXTURE_2D_ARRAY, attachment_texture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_DEPTH, &is_texture_array);
    glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_WIDTH, &attachment_object->width_);
    glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_HEIGHT, &attachment_object->height_);

    if (is_texture_array > 0)
    {
      attachment_object->texture_target_ = GL_TEXTURE_2D_ARRAY;
    }
    else
    {
      attachment_object->texture_target_ = attachment_object->isMultisample()
                                             ? GL_TEXTURE_2D_MULTISAMPLE
                                             : GL_TEXTURE_2D;
    }

    glGetTexLevelParameteriv(attachment_object->texture_target_,
                             0,
                             GL_TEXTURE_INTERNAL_FORMAT,
                             &attachment_object->texture_internal_format_);

    glBindTexture(GL_TEXTURE_2D_ARRAY, current_texture_binding);
  }
  else if (attachment_type == GL_NONE)
  {
    return nullptr;
  }

  assert(attachment_object != nullptr &&
         "Failed to create framebuffer attachment from current framebuffer.");
  return unique_ptr<GLFramebufferAttachment>(attachment_object);
}

void GLFramebufferAttachment::print(const char *prefix) const
{
  if (isTexture())
    DEBUG(LOG_TAG_RENDERER,
          "  [%s]: FramebufferAttachment(Texture(%u, %s, [%dx%d], %s), multiview(%s, base=%d), samples(%d))",
          prefix,
          id_,
          gles::glTextureTargetToString(texture_target_).c_str(),
          width_,
          height_,
          gles::glTextureInternalFormatToString(texture_internal_format_).c_str(),
          isMultiview() ? "Yes" : "No",
          base_view_index_,
          samples_);
  else if (isRenderbuffer())
    DEBUG(LOG_TAG_RENDERER,
          "  [%s]: FramebufferAttachment(Renderbuffer(%u, [%dx%d]), samples(%d))",
          prefix,
          id_,
          width_,
          height_,
          samples_);
  else
    DEBUG(LOG_TAG_RENDERER, "  [%s]: FramebufferAttachment(None)", prefix);
}
