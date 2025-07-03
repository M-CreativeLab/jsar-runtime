#pragma once

#include <optional>
#include <memory>

#include "./common.hpp"

class GLFramebufferAttachment
{
  enum AttachmentType
  {
    kTexture,
    kRenderbuffer,
    kNone,
  };

public:
  struct TextureAttachmentInit
  {
    GLenum internal_format = GL_RGBA8;
    GLsizei width = 0;
    GLsizei height = 0;
    GLint num_views = 1;
    GLint base_view_index = 0;
    GLint samples = 1;
  };

public:
  /**
   * Get a `GLFramebufferAttachment` from the current bound framebuffer object. It will get the attachment type,
   * multisampled, multiview, and texture target.
   *
   * @param attachment The attachment type, such as `GL_COLOR_ATTACHMENT0`, `GL_DEPTH_STENCIL_ATTACHMENT`, etc.
   * @returns An optional `GLFramebufferAttachment` object if the attachment is valid, otherwise an empty optional.
   */
  static std::unique_ptr<GLFramebufferAttachment> FromCurrent(GLenum attachment);

private:
  GLFramebufferAttachment(AttachmentType type, GLuint id, GLint samples, bool should_release_on_destructor)
      : type_(type)
      , id_(id)
      , samples_(samples)
      , should_release_on_destructor_(should_release_on_destructor)
  {
    assert(type != kNone && "Invalid framebuffer attachment type.");
  }
  GLFramebufferAttachment(AttachmentType type,
                          GLuint id,
                          bool should_release_on_destructor,
                          const GLFramebufferAttachment &color)
      : type_(type)
      , id_(id)
      , should_release_on_destructor_(should_release_on_destructor)
      , samples_(color.samples_)
      , width_(color.width_)
      , height_(color.height_)
      , num_views_(color.num_views_)
      , base_view_index_(color.base_view_index_)
      , texture_target_(color.texture_target_)
      , texture_internal_format_(color.texture_internal_format_)
  {
    assert(type != kNone && "Invalid framebuffer attachment type.");
  }

public:
  GLFramebufferAttachment()
      : type_(kNone)
      , id_(0)
      , samples_(1)
  {
  }
  ~GLFramebufferAttachment()
  {
    if (should_release_on_destructor_ == true && id_ > 0)
    {
      if (isTexture())
        glDeleteTextures(1, &id_);
      else if (isRenderbuffer())
        glDeleteRenderbuffers(1, &id_);
      DEBUG(LOG_TAG_ERROR, "Deleted framebuffer attachment: %u", id_);
    }
  }

public:
  GLuint id() const
  {
    return id_;
  }
  GLsizei width() const
  {
    return width_;
  }
  GLsizei height() const
  {
    return height_;
  }
  GLint numViews() const
  {
    return num_views_;
  }
  GLint baseViewIndex() const
  {
    return base_view_index_;
  }
  GLint samples() const
  {
    return samples_;
  }

  inline bool isTexture() const
  {
    return type_ == kTexture;
  }
  inline bool isRenderbuffer() const
  {
    return type_ == kRenderbuffer;
  }
  inline bool isNone() const
  {
    return type_ == kNone;
  }
  inline bool isMultisample() const
  {
    return samples_ > 1;
  }
  inline bool isMultiview() const
  {
    return num_views_ >= 2;
  }
  // Returns if the attachment has the same dimensions.
  inline bool isSameDimensions(const GLFramebufferAttachment &other) const
  {
    return width_ == other.width_ && height_ == other.height_;
  }

  void print(const char *prefix) const;

private:
  AttachmentType type_;
  GLuint id_;
  GLint samples_;
  bool should_release_on_destructor_ = false;

  GLsizei width_;
  GLsizei height_;

  GLint num_views_ = 1;
  GLint base_view_index_ = 0;

  GLenum texture_target_ = GL_TEXTURE_2D;
  GLint texture_internal_format_;
};
