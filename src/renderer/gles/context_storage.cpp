#include "context_storage.hpp"

using namespace std;

void ContextGLStorage::onViewportChanged(int x, int y, int w, int h)
{
  if (viewport_[0] == x &&
      viewport_[1] == y &&
      viewport_[2] == w &&
      viewport_[3] == h)
    return;
  viewport_[0] = x;
  viewport_[1] = y;
  viewport_[2] = w;
  viewport_[3] = h;
}

void ContextGLStorage::onCapabilityEnabled(GLenum cap, bool enabled)
{
  switch (cap)
  {
  case GL_BLEND:
    m_BlendEnabled = enabled;
    break;
  case GL_DITHER:
    m_DitherEnabled = enabled;
    break;
  case GL_CULL_FACE:
    m_CullFaceEnabled = enabled;
    break;
  case GL_DEPTH_TEST:
    m_DepthTestEnabled = enabled;
    break;
  case GL_STENCIL_TEST:
    m_StencilTestEnabled = enabled;
    break;
  case GL_SCISSOR_TEST:
    m_ScissorTestEnabled = enabled;
    break;
  default:
    break;
  }
}

void ContextGLStorage::onSetCullFace(GLenum mode)
{
  m_CullFace = mode;
}

void ContextGLStorage::onSetFrontFace(GLenum mode)
{
  m_FrontFace = mode;
}

void ContextGLStorage::onSetColorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a)
{
  m_ColorMask[0] = r;
  m_ColorMask[1] = g;
  m_ColorMask[2] = b;
  m_ColorMask[3] = a;
}

void ContextGLStorage::onSetDepthMask(GLboolean enabled)
{
  m_DepthMask = enabled;
}

void ContextGLStorage::onSetBlendFunc(GLenum sfactor, GLenum dfactor)
{
  m_BlendFunc.reset(sfactor, dfactor);
}

void ContextGLStorage::onSetBlendFuncSeparate(GLenum srcRgb, GLenum dstRgb, GLenum srcAlpha, GLenum dstAlpha)
{
  m_BlendFunc.reset(srcRgb, dstRgb, srcAlpha, dstAlpha);
}

void ContextGLStorage::onSetStencilMask(GLenum face, GLuint mask)
{
  if (face == GL_FRONT_AND_BACK || face == GL_FRONT)
    m_StencilMask = mask;
  if (face == GL_FRONT_AND_BACK || face == GL_BACK)
    m_StencilMaskBack = mask;
}

void ContextGLStorage::onSetStencilFunc(GLenum face, GLenum func, GLint ref, GLuint mask)
{
  if (face == GL_FRONT_AND_BACK || face == GL_FRONT)
  {
    m_StencilFunc.func = func;
    m_StencilFunc.ref = ref;
    m_StencilFunc.mask = mask;
  }
  if (face == GL_FRONT_AND_BACK || face == GL_BACK)
  {
    m_StencilFuncBack.func = func;
    m_StencilFuncBack.ref = ref;
    m_StencilFuncBack.mask = mask;
  }
}

void ContextGLStorage::onSetStencilOp(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
  if (face == GL_FRONT_AND_BACK || face == GL_FRONT)
  {
    m_StencilOp.sfail = sfail;
    m_StencilOp.dpfail = dpfail;
    m_StencilOp.dppass = dppass;
  }
  if (face == GL_FRONT_AND_BACK || face == GL_BACK)
  {
    m_StencilOpBack.sfail = sfail;
    m_StencilOpBack.dpfail = dpfail;
    m_StencilOpBack.dppass = dppass;
  }
}

void ContextGLStorage::resetProgram(int programToReset)
{
  if (m_ProgramId == programToReset)
    m_ProgramId = 0;
}

void ContextGLStorage::restore()
{
  GLenum setViewportError;
  GLenum useProgramError;
  GLenum bindBuffersError;
  GLenum bindTextureError;

  if (
    viewport_[0] != -1 &&
    viewport_[1] != -1 &&
    viewport_[2] != -1 &&
    viewport_[3] != -1)
  {
    glViewport(viewport_[0], viewport_[1], viewport_[2], viewport_[3]);
  }
  setViewportError = glGetError();

  // Restore the capabilities
  m_CullFaceEnabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
  m_DepthTestEnabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
  m_DitherEnabled ? glEnable(GL_DITHER) : glDisable(GL_DITHER);
  m_BlendEnabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
  m_StencilTestEnabled ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
  m_ScissorTestEnabled ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);

  // Blend state restore
  /**
   * NOTE: The blend function state should not depend on the blend state, it causes the host blend state would be passed to
   * the app context when the blend state is disabled.
   */
  if (!m_BlendFunc.isSeparate())
    glBlendFunc(m_BlendFunc.src(), m_BlendFunc.dst());
  else
    glBlendFuncSeparate(m_BlendFunc.srcRGB(), m_BlendFunc.dstRGB(), m_BlendFunc.srcAlpha(), m_BlendFunc.dstAlpha());

  // Cull state restore
  glCullFace(m_CullFace);
  if (m_FrontFace == GL_CW || m_FrontFace == GL_CCW)
    glFrontFace(m_FrontFace);

  // Color state restore
  glColorMask(m_ColorMask[0], m_ColorMask[1], m_ColorMask[2], m_ColorMask[3]);

  // Depth state restore
  glDepthMask(m_DepthMask);
  glDepthFunc(m_DepthFunc); // TODO: valid depth func enum?
  glDepthRangef(m_DepthRange[0], m_DepthRange[1]);

  // Stencil state restore
  {
    glStencilMask(m_StencilMask);
    if (m_StencilMask != m_StencilMaskBack)
      glStencilMaskSeparate(GL_BACK, m_StencilMaskBack);
    glStencilFunc(m_StencilFunc.func, m_StencilFunc.ref, m_StencilFunc.mask);
    if (m_StencilFunc != m_StencilFuncBack)
      glStencilFuncSeparate(GL_BACK, m_StencilFuncBack.func, m_StencilFuncBack.ref, m_StencilFuncBack.mask);
    glStencilOp(m_StencilOp.sfail, m_StencilOp.dpfail, m_StencilOp.dppass);
    if (m_StencilOp != m_StencilOpBack)
      glStencilOpSeparate(GL_BACK, m_StencilOpBack.sfail, m_StencilOpBack.dpfail, m_StencilOpBack.dppass);
  }

  // Scissor state restore
  glScissor(m_ScissorBox.x, m_ScissorBox.y, m_ScissorBox.width, m_ScissorBox.height);

  // Restoring other states
  {
    glLineWidth(m_LineWidth);
    glPolygonOffset(m_PolygonOffset.factor, m_PolygonOffset.units);
  }

  // Restore the program, buffers, framebuffer, renderbuffer, vertex array object, and active texture unit
  if (m_ProgramId >= 0)
    glUseProgram(m_ProgramId);
  else
    glUseProgram(0);
  useProgramError = glGetError();

  /**
   * Restore the array buffer, it's still global state which the vao just set the pointer to a buffer object for each
   * attribute.
   */
  if (m_ArrayBufferId >= 0)
    glBindBuffer(GL_ARRAY_BUFFER, m_ArrayBufferId);

  /**
   * VAO records the ebo, it will update the GL_ELEMENT_ARRAY_BUFFER_BINDING state when bind a specific VAO.
   */
  if (m_VertexArrayObjectId >= 0)
    glBindVertexArray(m_VertexArrayObjectId);
  else
  {
    /**
     * Only if the vao is not called we need to restore the ebo.
     */
    if (m_ElementArrayBufferId >= 0)
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementArrayBufferId);
  }

  if (m_FramebufferId.has_value())
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId.value());
  if (m_RenderbufferId >= 0)
    glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferId.value());
  bindBuffersError = glGetError();

  for (auto it = m_TextureBindingsWithUnit.begin(); it != m_TextureBindingsWithUnit.end(); it++)
  {
    auto unit = it->first;
    auto &binding = it->second;
    auto target = binding->target();
    auto texture = binding->texture();
    glActiveTexture(unit);
    glBindTexture(target, texture);
  }
  if (m_LastActiveTextureUnit >= GL_TEXTURE0 && m_LastActiveTextureUnit <= GL_TEXTURE31)
    glActiveTexture(m_LastActiveTextureUnit);
  else
    glActiveTexture(GL_TEXTURE0);
  bindTextureError = glGetError();

#if UNITY_ANDROID || UNITY_WEBGL
  EGLint eglError = eglGetError();
  if (eglError == EGL_CONTEXT_LOST)
    DEBUG(LOG_TAG_ERROR, "EGL context lost, need to reload the context.");
  else if (eglError != EGL_SUCCESS)
    DEBUG(LOG_TAG_ERROR, "Occurs an EGL error: 0x%04X", eglError);
#endif

  if (setViewportError != GL_NO_ERROR)
  {
    DEBUG(LOG_TAG_ERROR,
          "Occurs an error in glViewport(%d, %d, %d, %d) when restoring %s context: 0x%04X",
          viewport_[0],
          viewport_[1],
          viewport_[2],
          viewport_[3],
          name(),
          setViewportError);
  }
  if (useProgramError != GL_NO_ERROR)
  {
    DEBUG(LOG_TAG_ERROR,
          "Occurs an error in glUseProgram(%d) when restoring %s context: 0x%04X",
          m_ProgramId,
          name(),
          useProgramError);
  }
  if (bindBuffersError != GL_NO_ERROR)
  {
    DEBUG(LOG_TAG_ERROR,
          "Occurs an error in buffers binding when restoring %s context: 0x%04X",
          name(),
          bindBuffersError);
  }
  if (bindTextureError != GL_NO_ERROR)
  {
    DEBUG(LOG_TAG_ERROR,
          "Occurs an error in texture bindings when restoring %s context: 0x%04X",
          name(),
          bindTextureError);
  }

  // Check for OpenGL errors
  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
    DEBUG(LOG_TAG_ERROR,
          "Occurs an OpenGL error in restoring %s context: 0x%04X",
          error,
          name());
}

void ContextGLStorage::print()
{
  DEBUG(DEBUG_TAG,
        "%s program(%d), viewport(%d, %d, %d, %d)",
        name(),
        m_ProgramId,
        viewport_[0],
        viewport_[1],
        viewport_[2],
        viewport_[3]);
  DEBUG(DEBUG_TAG,
        "%s ebo(%d), vao(%d), framebuffer(%d), activeTexture(%d)",
        name(),
        m_ElementArrayBufferId,
        m_VertexArrayObjectId,
        m_FramebufferId,
        m_LastActiveTextureUnit - GL_TEXTURE0);
}

void ContextGLStorage::clearTextureBindings()
{
  m_TextureBindingsWithUnit.clear();
}
