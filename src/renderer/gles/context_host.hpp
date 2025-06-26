#pragma once

#include <optional>
#include <memory>
#include "./context_storage.hpp"

class GLFramebufferAttachment
{
  enum AttachmentType
  {
    kTexture,
    kRenderbuffer,
    kNone,
  };

public:
  /**
   * Get a `GLFramebufferAttachment` from the current bound framebuffer object. It will get the attachment type,
   * multisampled, multiview, and texture target.
   *
   * @param attachment The attachment type, such as `GL_COLOR_ATTACHMENT0`, `GL_DEPTH_STENCIL_ATTACHMENT`, etc.
   * @returns An optional `GLFramebufferAttachment` object if the attachment is valid, otherwise an empty optional.
   */
  static std::optional<GLFramebufferAttachment> FromCurrent(GLenum attachment);
  /**
   * Create a new `GLFramebufferAttachment`, and also use the GLES API to create an non-color attachment on the GPU, it
   * is used to create a depth, stencil, or depth-stencil attachment if the current binding framebuffer has no such
   * attachment.
   * 
   * @param attachment The attachment type, such as `GL_DEPTH_ATTACHMENT`, `GL_STENCIL_ATTACHMENT`, etc.
   * @param color The color attachment to be used as the source of the non-color attachment.
   * @returns A new `GLFramebufferAttachment` object that is created on the GPU.
   */
  static std::unique_ptr<GLFramebufferAttachment> CreateNonColorAttachmentOnGPU(GLenum attachment,
                                                                                GLFramebufferAttachment &color);

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
                          GLFramebufferAttachment &color)
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
    if (should_release_on_destructor_ && id_ > 0)
    {
      if (isTexture())
        glDeleteTextures(1, &id_);
      else if (isRenderbuffer())
        glDeleteRenderbuffers(1, &id_);
    }
  }

public:
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
  inline GLsizei samples() const
  {
    return samples_;
  }
  inline bool isMultiview() const
  {
    return num_views_ >= 2;
  }

  void bindToFramebuffer(GLenum attachment) const;
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

/**
 * The `ContextGLStorage` for the host such as: Unity, Unreal Engine, etc.
 * 
 * It records the host's context states and fbos, and provides methods to restore them.
 */
class ContextGLHost : public ContextGLStorage
{
public:
  ContextGLHost();

public:
  void recordFromHost();
  void recordTextureBindingFromHost();
  void restore();
  void configureFramebuffer();
  void restoreFramebuffer();

  // Create a render target(framebuffer) for application to render into.
  std::optional<GLuint> createRenderTarget();

private:
  inline bool isHostFramebufferColorOnly() const
  {
    return host_color_attachment_.has_value() && !host_depth_stencil_attachment_.has_value();
  }
  void onHostFramebufferChanged();

  void saveDepthBufferFromHost();
  void restoreDepthBufferToHost();

private:
  std::optional<GLFramebufferAttachment> host_color_attachment_;
  std::optional<GLFramebufferAttachment> host_depth_stencil_attachment_;
  std::unique_ptr<GLFramebufferAttachment> shared_depth_stencil_attachment_;
  GLuint shared_framebuffer_;
};
