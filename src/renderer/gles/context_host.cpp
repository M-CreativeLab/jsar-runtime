#include "./context_host.hpp"

#ifndef GL_COMPUTE_SHADER
#define GL_COMPUTE_SHADER 0x91B9
#endif

ContextGLHost::ContextGLHost()
    : ContextGLStorage("Host")
{
  recordFromHost();
}

void ContextGLHost::recordFromHost()
{
  glGetIntegerv(GL_VIEWPORT, viewport_);

  // Record the framebuffer state and check if it has changed
  GLint current_framebuffer;
  bool is_framebuffer_changed = false;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current_framebuffer);

  if (!m_FramebufferId.has_value() ||
      m_FramebufferId != current_framebuffer)
  {
    m_FramebufferId = current_framebuffer;
    is_framebuffer_changed = true;
  }

  // Record clear values
  {
    GLfloat clear_color[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clear_color);
    m_ClearColor = {clear_color[0], clear_color[1], clear_color[2], clear_color[3]};

    GLfloat clear_depth;
    glGetFloatv(GL_DEPTH_CLEAR_VALUE, &clear_depth);
    m_ClearDepth = clear_depth;

    GLint clear_stencil;
    glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &clear_stencil);
    m_ClearStencil = clear_stencil;
  }

  // Record objects
  glGetIntegerv(GL_CURRENT_PROGRAM, &m_ProgramId);
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &m_ArrayBufferId);
  glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &m_ElementArrayBufferId);
  {
    // Read the current rbo binding
    GLint current_renderbuffer;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &current_renderbuffer);
    m_RenderbufferId = current_renderbuffer;
  }
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
  ContextGLStorage::restore();
  glClear(GL_STENCIL_BUFFER_BIT);
}

void ContextGLHost::onHostFramebufferChanged()
{
  assert(m_FramebufferId.has_value() &&
         "Framebuffer ID must be set before calling onHostFramebufferChanged.");
  auto current_framebuffer_id = m_FramebufferId.value();
  if (current_framebuffer_id == 0)
    return;

  if (host_framebuffers_.find(current_framebuffer_id) == host_framebuffers_.end())
  {
    auto framebuffer = make_shared<GLFramebuffer>(current_framebuffer_id, false);
    framebuffer->setupAttachmentsFromCurrent();
    host_framebuffers_[current_framebuffer_id] = framebuffer;

    if (host_framebuffers_.size() > 10)
    {
      DEBUG(LOG_TAG_ERROR,
            "Too many host framebuffers(%zu) are recorded, clearing the oldest one.",
            host_framebuffers_.size());
    }
  }
}
