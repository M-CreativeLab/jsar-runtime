#include "./context_host.hpp"

#ifndef GL_COMPUTE_SHADER
#define GL_COMPUTE_SHADER 0x91B9
#endif

ContextGLHost::ContextGLHost()
    : ContextGLStorage("Host")
{
  // Create the shared framebuffer which is used to store the shared attachments between the applications.
  glGenFramebuffers(1, &shared_framebuffer_);
  assert(shared_framebuffer_ != 0 && "Failed to create shared framebuffer for host context");

  // Record the host's context states
  recordFromHost();

  //   const char *process_shader_source = R"(
  //     #version 310 es
  //     #extension GL_EXT_compute_shader : enable
  //     precision highp float;
  //     layout(local_size_x = 16, local_size_y = 16) in;

  //     uniform sampler2DArray uSrcDepthStencil;
  //     layout(r32f, binding = 0) writeonly uniform image2DArray uDstDepth;

  //     void main() {
  //       ivec3 coord = ivec3(gl_GlobalInvocationID);
  //       uint packedValue = texelFetch(uSrcDepthStencil, coord, 0).r;
  //       float depth = float(packedValue >> 8) / float(0xFFFFFF);
  //       imageStore(uDstDepth, coord, vec4(depth, 0.0, 0.0, 0.0));
  //     }
  // )";
  //   GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
  //   glShaderSource(shader, 1, &process_shader_source, nullptr);
  //   glCompileShader(shader);

  //   GLuint program = glCreateProgram();
  //   glAttachShader(program, shader);
  //   glLinkProgram(program);
  //   glDeleteShader(shader);

  //   // check link status
  //   GLint linkStatus;
  //   glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
  //   if (linkStatus == GL_FALSE)
  //   {
  //     GLint logLength;
  //     glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
  //     if (logLength > 0)
  //     {
  //       char *log = new char[logLength];
  //       glGetProgramInfoLog(program, logLength, nullptr, log);
  //       DEBUG(LOG_TAG_ERROR, "Compute shader program link error: %s", log);
  //       delete[] log;
  //     }
  //     glDeleteProgram(program);
  //     program = 0;
  //   }

  //   auto err = glGetError();
  //   if (err != GL_NO_ERROR)
  //   {
  //     DEBUG(LOG_TAG_ERROR, "Failed to create compute shader program in %s context: 0x%04X", name(), err);
  //   }
  //   else
  //   {
  //     DEBUG(LOG_TAG_RENDERER, "Compute shader program created in %s context: %u", name(), program);
  //   }
}

void ContextGLHost::recordFromHost()
{
  glGetIntegerv(GL_VIEWPORT, viewport_);

  // Record the framebuffer state and check if it has changed
  GLint current_framebuffer;
  bool is_framebuffer_changed = false;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current_framebuffer);
  if (current_framebuffer != m_FramebufferId)
  {
    m_FramebufferId = current_framebuffer;
    is_framebuffer_changed = true;
  }

  // Record objects
  glGetIntegerv(GL_CURRENT_PROGRAM, &m_ProgramId);
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &m_ArrayBufferId);
  glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &m_ElementArrayBufferId);
  glGetIntegerv(GL_RENDERBUFFER_BINDING, &m_RenderbufferId);
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &m_VertexArrayObjectId);
  glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint *)&m_LastActiveTextureUnit);

  clearTextureBindings();
  for (int i = GL_TEXTURE0; i <= GL_TEXTURE31; i++)
  {
    GLint texture = 0;
    glActiveTexture(i);

    // TODO: how to support other texture targets?
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture);
    m_TextureBindingsWithUnit[i] = make_shared<OpenGLTextureBinding>(GL_TEXTURE_2D, texture);
  }
  glActiveTexture(m_LastActiveTextureUnit);

  // Enable or disable
  m_CullFaceEnabled = glIsEnabled(GL_CULL_FACE);
  m_DepthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
  m_DitherEnabled = glIsEnabled(GL_DITHER);
  m_BlendEnabled = glIsEnabled(GL_BLEND);
  m_StencilTestEnabled = glIsEnabled(GL_STENCIL_TEST);
  m_ScissorTestEnabled = glIsEnabled(GL_SCISSOR_TEST);

  // Global States
  glGetIntegerv(GL_CULL_FACE_MODE, (GLint *)&m_CullFace);
  glGetIntegerv(GL_FRONT_FACE, (GLint *)&m_FrontFace);
  glGetBooleanv(GL_COLOR_WRITEMASK, (GLboolean *)&m_ColorMask);
  /**
   * Recording the depth parameters.
   */
  {
    glGetBooleanv(GL_DEPTH_WRITEMASK, &m_DepthMask);
    glGetIntegerv(GL_DEPTH_FUNC, (GLint *)&m_DepthFunc);
    glGetFloatv(GL_DEPTH_RANGE, m_DepthRange);
  }
  /**
   * Recording the stencil parameters.
   */
  {
    // glStencilMask(mask)
    glGetIntegerv(GL_STENCIL_WRITEMASK, (GLint *)&m_StencilMask);
    glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, (GLint *)&m_StencilMaskBack);
    // glStencilFunc(func, ref, mask)
    glGetIntegerv(GL_STENCIL_FUNC, (GLint *)&m_StencilFunc.func);
    glGetIntegerv(GL_STENCIL_REF, (GLint *)&m_StencilFunc.ref);
    glGetIntegerv(GL_STENCIL_VALUE_MASK, (GLint *)&m_StencilFunc.mask);
    glGetIntegerv(GL_STENCIL_BACK_FUNC, (GLint *)&m_StencilFuncBack.func);
    glGetIntegerv(GL_STENCIL_BACK_REF, (GLint *)&m_StencilFuncBack.ref);
    glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, (GLint *)&m_StencilFuncBack.mask);
    // glStencilOp(sfail, dpfail, dppass)
    glGetIntegerv(GL_STENCIL_FAIL, (GLint *)&m_StencilOp.sfail);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, (GLint *)&m_StencilOp.dpfail);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, (GLint *)&m_StencilOp.dppass);
    glGetIntegerv(GL_STENCIL_BACK_FAIL, (GLint *)&m_StencilOpBack.sfail);
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, (GLint *)&m_StencilOpBack.dpfail);
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, (GLint *)&m_StencilOpBack.dppass);
  }
  /**
   * Recording the scissors.
   */
  {
    GLint scissorBox[4];
    glGetIntegerv(GL_SCISSOR_BOX, scissorBox);
    m_ScissorBox = {scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3]};
  }

  // Blend funcs
  {
    GLenum sfactor, dfactor;
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint *)&sfactor);
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint *)&dfactor);
    m_BlendFunc.reset(sfactor, dfactor);
  }

  // Others
  {
    glGetFloatv(GL_LINE_WIDTH, &m_LineWidth);
    glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &m_PolygonOffset.factor);
    glGetFloatv(GL_POLYGON_OFFSET_UNITS, &m_PolygonOffset.units);
  }

  // Record the framebuffer state and save the depth buffer
  if (is_framebuffer_changed)
    onHostFramebufferChanged();
  saveDepthBufferFromHost();

  // Check for errors
  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
    DEBUG(LOG_TAG_ERROR, "Occurs an OpenGL error in recording %s context: 0x%04X", name(), error);
}

void ContextGLHost::recordTextureBindingFromHost()
{
  auto &binding = m_TextureBindingsWithUnit[m_LastActiveTextureUnit];
  if (binding != nullptr)
    return;

  GLuint texture;
  GLint beforeActiveUnit;
  glGetIntegerv(GL_ACTIVE_TEXTURE, &beforeActiveUnit);

  bool isActiveNotMatched = beforeActiveUnit != m_LastActiveTextureUnit;
  if (isActiveNotMatched)
    glActiveTexture(m_LastActiveTextureUnit);

  glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&texture);
  m_TextureBindingsWithUnit[m_LastActiveTextureUnit] = make_shared<OpenGLTextureBinding>(GL_TEXTURE_2D, texture);

  if (isActiveNotMatched)
    glActiveTexture(beforeActiveUnit);
}

void ContextGLHost::restore()
{
  restoreDepthBufferToHost();
  ContextGLStorage::restore();
}

void ContextGLHost::configureFramebuffer()
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
}

void ContextGLHost::restoreFramebuffer()
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
}

optional<GLuint> ContextGLHost::createRenderTarget()
{
  assert(host_color_attachment_.has_value() &&
         "Host color attachment must be set before creating a render target.");
  assert(shared_depth_stencil_attachment_ != nullptr &&
         "Shared depth-stencil attachment must be set before creating a render target.");

  GLuint new_render_target = 0;
  glGenFramebuffers(1, &new_render_target);
  if (new_render_target == 0)
  {
    DEBUG(LOG_TAG_ERROR, "Failed to create a new render target.");
    return nullopt;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, new_render_target);
  host_color_attachment_->bindToFramebuffer(GL_COLOR_ATTACHMENT0);
  shared_depth_stencil_attachment_->bindToFramebuffer(GL_DEPTH_STENCIL_ATTACHMENT);

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE)
  {
    DEBUG(LOG_TAG_ERROR, "Failed to create a render target: 0x%04X", status);
    glDeleteFramebuffers(1, &new_render_target);
    return nullopt;
  }
  return new_render_target;
}

#define FRAMEBUFFER_ATTACHMENT_TEXTURE_SAMPLES_EXT 0x8D6C
#define FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR 0x9630
#define FRAMEBUFFER_ATTACHMENT_TEXTURE_BASE_VIEW_INDEX_OVR 0x9632

optional<GLFramebufferAttachment> GLFramebufferAttachment::FromCurrent(GLenum attachment)
{
  assert(attachment == GL_COLOR_ATTACHMENT0 ||
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
    auto attachment_object = GLFramebufferAttachment(kRenderbuffer, attachment_rbo, num_samples, false);
    {
      // Read the renderbuffer's width and height
      glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &attachment_object.width_);
      glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &attachment_object.height_);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    return attachment_object;
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

    auto attachment_object = GLFramebufferAttachment(kTexture, attachment_texture, num_samples, false);

    // Check if the texture is a multiview texture
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                          attachment,
                                          FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR,
                                          &attachment_object.num_views_);

    GLint current_texture_binding = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &current_texture_binding);

    // Check if the texture is TEXTURE_2D_ARRAY or TEXTURE_2D
    // TODO(yorkie): multiview must use TEXTURE_2D_ARRAY?
    GLint is_texture_array = 0;
    glBindTexture(GL_TEXTURE_2D_ARRAY, attachment_texture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_DEPTH, &is_texture_array);
    glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_WIDTH, &attachment_object.width_);
    glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_HEIGHT, &attachment_object.height_);

    if (is_texture_array > 0)
      attachment_object.texture_target_ = GL_TEXTURE_2D_ARRAY;
    else
      attachment_object.texture_target_ = GL_TEXTURE_2D;

    glGetTexLevelParameteriv(attachment_object.texture_target_,
                             0,
                             GL_TEXTURE_INTERNAL_FORMAT,
                             &attachment_object.texture_internal_format_);

    glBindTexture(GL_TEXTURE_2D_ARRAY, current_texture_binding);
    return attachment_object;
  }
  else if (attachment_type == GL_NONE)
  {
    return nullopt;
  }

  // Unable to reach here.
  assert(false && "Fatal error: Unsupported framebuffer attachment type.");
  return nullopt;
}

unique_ptr<GLFramebufferAttachment> GLFramebufferAttachment::CreateNonColorAttachmentOnGPU(
  GLenum attachment, GLFramebufferAttachment &color)
{
  assert(attachment == GL_DEPTH_STENCIL_ATTACHMENT ||
         attachment == GL_DEPTH_ATTACHMENT ||
         attachment == GL_STENCIL_ATTACHMENT &&
           "Invalid framebuffer attachment type for non-color attachment.");
  assert(!color.isNone() && "Color attachment must be set before creating a non-color attachment.");

  GLuint attachment_texture;
  glGenTextures(1, &attachment_texture);
  assert(attachment_texture != 0 &&
         "Failed to create a texture for non-color framebuffer attachment.");

  auto attachment_object = unique_ptr<GLFramebufferAttachment>(
    new GLFramebufferAttachment(kTexture,
                                attachment_texture,
                                true,
                                color));
  {
    glBindTexture(attachment_object->texture_target_, attachment_object->id_);

    // Allocate storage for the texture
    if (attachment_object->texture_target_ == GL_TEXTURE_2D)
    {
      glTexStorage2D(GL_TEXTURE_2D,
                     1,
                     GL_DEPTH24_STENCIL8,
                     attachment_object->width_,
                     attachment_object->height_);
    }
    else if (attachment_object->texture_target_ == GL_TEXTURE_2D_ARRAY)
    {
      glTexStorage3D(GL_TEXTURE_2D_ARRAY,
                     1,
                     GL_DEPTH24_STENCIL8,
                     attachment_object->width_,
                     attachment_object->height_,
                     attachment_object->num_views_);
    }
    else
    {
      assert(false && "Unsupported texture target for non-color framebuffer attachment.");
    }

    // Configure texture parameters
    glTexParameteri(attachment_object->texture_target_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(attachment_object->texture_target_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(attachment_object->texture_target_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(attachment_object->texture_target_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(attachment_object->texture_target_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(attachment_object->texture_target_, 0);
  }
  return attachment_object;
}

void GLFramebufferAttachment::bindToFramebuffer(GLenum attachment) const
{
  if (isRenderbuffer())
  {
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, id_);
  }
  else if (isTexture())
  {
    if (isMultiview())
    {
#if defined(__ANDROID__)
      static PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC framebufferTextureMultiview =
        (PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC)eglGetProcAddress("glFramebufferTextureMultiviewOVR");
      static PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVRPROC framebufferTextureMultisampleMultiview =
        (PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVRPROC)eglGetProcAddress("glFramebufferTextureMultisampleMultiviewOVR");
      assert(framebufferTextureMultiview != nullptr &&
             "glFramebufferTextureMultiviewOVR is not available on this device.");
      assert(framebufferTextureMultisampleMultiview != nullptr &&
             "glFramebufferTextureMultisampleMultiviewOVR is not available on this device.");

      if (isMultisample())
        framebufferTextureMultisampleMultiview(GL_FRAMEBUFFER, attachment, id_, 0, samples_, 0, num_views_);
      else
        framebufferTextureMultiview(GL_FRAMEBUFFER, attachment, id_, 0, 0, num_views_);
#elif
      DEBUG(LOG_TAG_ERROR, "Skipped to bind: multiview framebuffer attachment is not supported on Android.");
#endif
    }
    else
    {
      glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, id_, 0);
    }
  }
  else
  {
    DEBUG(LOG_TAG_ERROR, "Invalid framebuffer attachment type.");
  }
}

void GLFramebufferAttachment::print(const char *prefix) const
{
  if (isTexture())
    DEBUG(LOG_TAG_RENDERER,
          "[%s]: FramebufferAttachment(Texture(%u, %s, [%dx%d]), multiview(%s), samples(%d))",
          prefix,
          id_,
          texture_target_ == GL_TEXTURE_2D ? "TEXTURE_2D" : "TEXTURE_2D_ARRAY",
          width_,
          height_,
          isMultiview() ? "Yes" : "No",
          samples_);
  else if (isRenderbuffer())
    DEBUG(LOG_TAG_RENDERER,
          "[%s]: FramebufferAttachment(Renderbuffer(%u, [%dx%d]), multiview(%s), samples(%d))",
          prefix,
          id_,
          width_,
          height_,
          isMultiview() ? "Yes" : "No",
          samples_);
  else
    DEBUG(LOG_TAG_RENDERER, "[%s]: FramebufferAttachment(None)", prefix);
}

void ContextGLHost::onHostFramebufferChanged()
{
  host_color_attachment_ = GLFramebufferAttachment::FromCurrent(GL_COLOR_ATTACHMENT0);
  host_depth_stencil_attachment_ = GLFramebufferAttachment::FromCurrent(GL_DEPTH_STENCIL_ATTACHMENT);
  shared_depth_stencil_attachment_ = GLFramebufferAttachment::CreateNonColorAttachmentOnGPU(
    GL_DEPTH_STENCIL_ATTACHMENT, host_color_attachment_.value());

  DEBUG(LOG_TAG_RENDERER, "Host framebuffer(%d) changed:", m_FramebufferId);
  if (host_color_attachment_.has_value())
    host_color_attachment_->print("Host Color Attachment");
  if (host_depth_stencil_attachment_.has_value())
    host_depth_stencil_attachment_->print("Host Depth-Stencil Attachment");
  if (shared_depth_stencil_attachment_ != nullptr)
    shared_depth_stencil_attachment_->print("Shared Depth-Stencil Attachment");

  if (host_color_attachment_.has_value() && shared_depth_stencil_attachment_ != nullptr)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, shared_framebuffer_);
    host_color_attachment_->bindToFramebuffer(GL_COLOR_ATTACHMENT0);
    shared_depth_stencil_attachment_->bindToFramebuffer(GL_DEPTH_STENCIL_ATTACHMENT);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(status == GL_FRAMEBUFFER_COMPLETE &&
           "Failed to create a shared framebuffer with host color and depth-stencil attachments.");
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
  }
}

void ContextGLHost::saveDepthBufferFromHost()
{
  // Only copy depth buffer if the host framebuffer owns a depth-stencil attachment.
  if (isHostFramebufferColorOnly())
    return;

  // Blit the depth-stencil buffer from the host framebuffer to the shared depth-stencil attachment.
}

void ContextGLHost::restoreDepthBufferToHost()
{
  // Clear the depth and stencil buffers of the shared framebuffer.
  assert(shared_framebuffer_ != 0 &&
         "Shared framebuffer must be created before restoring the depth buffer.");
  glBindFramebuffer(GL_FRAMEBUFFER, shared_framebuffer_);
  glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Don't restore depth buffer if the host framebuffer is color-only.
  if (isHostFramebufferColorOnly())
    return;

  // Blit the depth-stencil buffer from the shared depth-stencil attachment to the host framebuffer.
  // This is to enable the post passes to use the depth buffer updated by the JSAR applications.
}
