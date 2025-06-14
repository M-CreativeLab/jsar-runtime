#include "context_storage.hpp"

using namespace std;

void OpenGLContextStorage::RecordViewport(int x, int y, int w, int h)
{
  m_Viewport[0] = x;
  m_Viewport[1] = y;
  m_Viewport[2] = w;
  m_Viewport[3] = h;
}

void OpenGLContextStorage::RecordCapability(GLenum cap, bool enabled)
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

void OpenGLContextStorage::RecordCullFace(GLenum mode)
{
  m_CullFace = mode;
}

void OpenGLContextStorage::RecordFrontFace(GLenum mode)
{
  m_FrontFace = mode;
}

void OpenGLContextStorage::RecordColorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a)
{
  m_ColorMask[0] = r;
  m_ColorMask[1] = g;
  m_ColorMask[2] = b;
  m_ColorMask[3] = a;
}

void OpenGLContextStorage::RecordDepthMask(GLboolean enabled)
{
  m_DepthMask = enabled;
}

void OpenGLContextStorage::RecordBlendFunc(GLenum sfactor, GLenum dfactor)
{
  m_BlendFunc.Reset(sfactor, dfactor);
}

void OpenGLContextStorage::RecordBlendFuncSeparate(GLenum srcRgb, GLenum dstRgb, GLenum srcAlpha, GLenum dstAlpha)
{
  m_BlendFunc.Reset(srcRgb, dstRgb, srcAlpha, dstAlpha);
}

void OpenGLContextStorage::RecordStencilMask(GLenum face, GLuint mask)
{
  if (face == GL_FRONT_AND_BACK || face == GL_FRONT)
    m_StencilMask = mask;
  if (face == GL_FRONT_AND_BACK || face == GL_BACK)
    m_StencilMaskBack = mask;
}

void OpenGLContextStorage::RecordStencilFunc(GLenum face, GLenum func, GLint ref, GLuint mask)
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

void OpenGLContextStorage::RecordStencilOp(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
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

void OpenGLContextStorage::RecordProgram(int program)
{
  m_ProgramId = program;
}

void OpenGLContextStorage::RecordArrayBuffer(int buffer)
{
  m_ArrayBufferId = buffer;
}

void OpenGLContextStorage::RecordElementArrayBuffer(int buffer)
{
  m_ElementArrayBufferId = buffer;
}

void OpenGLContextStorage::RecordFramebuffer(int buffer)
{
  m_FramebufferId = buffer;
}

void OpenGLContextStorage::RecordRenderbuffer(int buffer)
{
  m_RenderbufferId = buffer;
}

void OpenGLContextStorage::RecordVertexArrayObject(int vao)
{
  m_VertexArrayObjectId = vao;
}

void OpenGLContextStorage::RecordActiveTextureUnit(int unit)
{
  m_LastActiveTextureUnit = unit;
}

void OpenGLContextStorage::RecordTextureBindingWithUnit(GLenum target, GLuint texture)
{
  GLint activeUnit;
  glGetIntegerv(GL_ACTIVE_TEXTURE, &activeUnit);

  auto &binding = m_TextureBindingsWithUnit[activeUnit];
  if (binding == nullptr)
    m_TextureBindingsWithUnit[activeUnit] = make_shared<OpenGLTextureBinding>(target, texture);
  else
    binding->Reset(target, texture);
}

void OpenGLContextStorage::ResetProgram(int programToReset)
{
  if (m_ProgramId == programToReset)
    m_ProgramId = 0;
}

void OpenGLContextStorage::Restore()
{
  GLenum setViewportError;
  GLenum useProgramError;
  GLenum bindBuffersError;
  GLenum bindTextureError;

  if (
    m_Viewport[0] != -1 &&
    m_Viewport[1] != -1 &&
    m_Viewport[2] != -1 &&
    m_Viewport[3] != -1)
  {
    glViewport(m_Viewport[0], m_Viewport[1], m_Viewport[2], m_Viewport[3]);
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
  if (!m_BlendFunc.IsSeparate())
    glBlendFunc(m_BlendFunc.GetSrc(), m_BlendFunc.GetDst());
  else
    glBlendFuncSeparate(m_BlendFunc.GetSrcRgb(), m_BlendFunc.GetDstRgb(), m_BlendFunc.GetSrcAlpha(), m_BlendFunc.GetDstAlpha());

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

  if (m_FramebufferId >= 0)
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
  if (m_RenderbufferId >= 0)
    glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferId);
  bindBuffersError = glGetError();

  for (auto it = m_TextureBindingsWithUnit.begin(); it != m_TextureBindingsWithUnit.end(); it++)
  {
    auto unit = it->first;
    auto &binding = it->second;
    auto target = binding->GetTarget();
    auto texture = binding->GetTexture();
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
    DEBUG(LOG_TAG_ERROR, "Occurs an error in glViewport(%d, %d, %d, %d) when restoring %s context: 0x%04X", m_Viewport[0], m_Viewport[1], m_Viewport[2], m_Viewport[3], GetName(), setViewportError);
  if (useProgramError != GL_NO_ERROR)
    DEBUG(LOG_TAG_ERROR, "Occurs an error in glUseProgram(%d) when restoring %s context: 0x%04X", m_ProgramId, GetName(), useProgramError);
  if (bindBuffersError != GL_NO_ERROR)
    DEBUG(LOG_TAG_ERROR, "Occurs an error in buffers binding when restoring %s context: 0x%04X", GetName(), bindBuffersError);
  if (bindTextureError != GL_NO_ERROR)
    DEBUG(LOG_TAG_ERROR, "Occurs an error in texture bindings when restoring %s context: 0x%04X", GetName(), bindTextureError);

  // Check for OpenGL errors
  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
    DEBUG(LOG_TAG_ERROR, "Occurs an OpenGL error in restoring %s context: 0x%04X", error, GetName());
}

void OpenGLContextStorage::Print()
{
  DEBUG(DEBUG_TAG, "%s program(%d), viewport(%d, %d, %d, %d)", GetName(), m_ProgramId, m_Viewport[0], m_Viewport[1], m_Viewport[2], m_Viewport[3]);
  DEBUG(DEBUG_TAG, "%s ebo(%d), vao(%d), framebuffer(%d), activeTexture(%d)", GetName(), m_ElementArrayBufferId, m_VertexArrayObjectId, m_FramebufferId, m_LastActiveTextureUnit - GL_TEXTURE0);
}

void OpenGLContextStorage::ClearTextureBindings()
{
  m_TextureBindingsWithUnit.clear();
}

void OpenGLHostContextStorage::Record()
{
  glGetIntegerv(GL_VIEWPORT, m_Viewport);

  // Record objects
  glGetIntegerv(GL_CURRENT_PROGRAM, &m_ProgramId);
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &m_ArrayBufferId);
  glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &m_ElementArrayBufferId);
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_FramebufferId);
  glGetIntegerv(GL_RENDERBUFFER_BINDING, &m_RenderbufferId);
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &m_VertexArrayObjectId);
  glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint *)&m_LastActiveTextureUnit);

  ClearTextureBindings();
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
    m_BlendFunc.Reset(sfactor, dfactor);
  }

  // Others
  {
    glGetFloatv(GL_LINE_WIDTH, &m_LineWidth);
    glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &m_PolygonOffset.factor);
    glGetFloatv(GL_POLYGON_OFFSET_UNITS, &m_PolygonOffset.units);
  }

  // Check for errors
  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
    DEBUG(LOG_TAG_ERROR, "Occurs an OpenGL error in recording %s context: 0x%04X", GetName(), error);
}

void OpenGLHostContextStorage::RecordTextureBindingFromHost()
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

void OpenGLHostContextStorage::ConfigureFramebuffer()
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
}

void OpenGLHostContextStorage::RestoreFramebuffer()
{
}

OpenGLAppContextStorage::OpenGLAppContextStorage(string name)
    : OpenGLContextStorage(name)
    , m_GLObjectManager(make_unique<gles::GLObjectManager>(name))
{
  /**
   * Initial values for WebGL or OpenGLES.
   */
  m_CullFaceEnabled = GL_FALSE;
  m_CullFace = GL_BACK;
  m_FrontFace = GL_CCW;

  // Color mask
  {
    m_ColorMask[0] = GL_TRUE;
    m_ColorMask[1] = GL_TRUE;
    m_ColorMask[2] = GL_TRUE;
    m_ColorMask[3] = GL_TRUE;
  }

  // Depth
  {
    m_DepthTestEnabled = GL_FALSE;
    m_DepthMask = GL_TRUE;
    m_DepthFunc = GL_LESS;
  }

  // Dither
  m_DitherEnabled = GL_TRUE;

  // Blending
  {
    m_BlendEnabled = GL_FALSE;
    m_BlendFunc.Reset(GL_ONE, GL_ZERO);
  }

  // Stencil
  {
    m_StencilTestEnabled = GL_FALSE;
    m_StencilMask = 0x01;
    m_StencilMaskBack = 0x01;
  }

  // Scissor
  {
    m_ScissorTestEnabled = GL_FALSE;
    // Use viewport's size as the default scissor box
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    m_ScissorBox = {0, 0, viewport[2], viewport[3]};
  }
}

OpenGLAppContextStorage::OpenGLAppContextStorage(string name, OpenGLAppContextStorage *from)
    : OpenGLContextStorage(name, from)
    , m_GLObjectManager(from->m_GLObjectManager)
{
  m_Programs = OpenGLNamesStorage(&from->m_Programs);
  m_Shaders = OpenGLNamesStorage(&from->m_Shaders);
  m_Buffers = OpenGLNamesStorage(&from->m_Buffers);
  m_Framebuffers = OpenGLNamesStorage(&from->m_Framebuffers);
  m_Renderbuffers = OpenGLNamesStorage(&from->m_Renderbuffers);
  m_VertexArrayObjects = OpenGLNamesStorage(&from->m_VertexArrayObjects);
  m_Textures = OpenGLNamesStorage(&from->m_Textures);
  m_Samplers = OpenGLNamesStorage(&from->m_Samplers);
}

void OpenGLAppContextStorage::RecordViewport(int x, int y, int w, int h)
{
  if (m_Viewport[0] == x && m_Viewport[1] == y && m_Viewport[2] == w && m_Viewport[3] == h)
    return;
  m_Viewport[0] = x;
  m_Viewport[1] = y;
  m_Viewport[2] = w;
  m_Viewport[3] = h;
}

void OpenGLAppContextStorage::RecordProgramOnCreated(GLuint program)
{
  if (program == 0)
    return;
  if (m_Programs.find(program) != m_Programs.end())
    return; // Already recorded
  m_Programs.insert(pair<GLuint, bool>(program, true));
}

void OpenGLAppContextStorage::RecordProgramOnDeleted(GLuint program)
{
  if (program == 0)
    return;
  if (m_Programs.find(program) == m_Programs.end())
    return; // Not recorded
  m_Programs.erase(program);
  // FIXME: Reset the current program if it is deleted?
}

void OpenGLAppContextStorage::RecordShaderOnCreated(GLuint shader)
{
  if (shader == 0)
    return;
  if (m_Shaders.find(shader) != m_Shaders.end())
    return; // Already recorded
  m_Shaders.insert(pair<GLuint, bool>(shader, true));
}

void OpenGLAppContextStorage::RecordShaderOnDeleted(GLuint shader)
{
  if (shader == 0)
    return;
  if (m_Shaders.find(shader) == m_Shaders.end())
    return; // Not recorded
  m_Shaders.erase(shader);
}

void OpenGLAppContextStorage::RecordBufferOnCreated(GLuint buffer)
{
  if (buffer == 0)
    return;
  if (m_Buffers.find(buffer) != m_Buffers.end())
    return; // Already recorded
  m_Buffers.insert(pair<GLuint, bool>(buffer, true));
}

void OpenGLAppContextStorage::RecordBufferOnDeleted(GLuint buffer)
{
  if (buffer == 0)
    return;
  if (m_Buffers.find(buffer) == m_Buffers.end())
    return; // Not recorded
  m_Buffers.erase(buffer);
}

void OpenGLAppContextStorage::RecordFramebufferOnCreated(GLuint buffer)
{
  if (buffer == 0)
    return;
  if (m_Framebuffers.find(buffer) != m_Framebuffers.end())
    return; // Already recorded
  m_Framebuffers.insert(pair<GLuint, bool>(buffer, true));
}

void OpenGLAppContextStorage::RecordFramebufferOnDeleted(GLuint buffer)
{
  if (buffer == 0)
    return;
  if (m_Framebuffers.find(buffer) == m_Framebuffers.end())
    return; // Not recorded
  m_Framebuffers.erase(buffer);
}

void OpenGLAppContextStorage::RecordRenderbufferOnCreated(GLuint buffer)
{
  if (buffer == 0)
    return;
  if (m_Renderbuffers.find(buffer) != m_Renderbuffers.end())
    return; // Already recorded
  m_Renderbuffers.insert(pair<GLuint, bool>(buffer, true));
}

void OpenGLAppContextStorage::RecordRenderbufferOnDeleted(GLuint buffer)
{
  if (buffer == 0)
    return;
  if (m_Renderbuffers.find(buffer) == m_Renderbuffers.end())
    return; // Not recorded
  m_Renderbuffers.erase(buffer);
}

void OpenGLAppContextStorage::RecordVertexArrayObjectOnCreated(GLuint vao)
{
  if (vao == 0)
    return;
  if (m_VertexArrayObjects.find(vao) != m_VertexArrayObjects.end())
    return; // Already recorded
  m_VertexArrayObjects.insert(pair<GLuint, bool>(vao, true));
}

void OpenGLAppContextStorage::RecordVertexArrayObjectOnDeleted(GLuint vao)
{
  if (vao == 0)
    return;
  if (m_VertexArrayObjects.find(vao) == m_VertexArrayObjects.end())
    return; // Not recorded
  m_VertexArrayObjects.erase(vao);
}

void OpenGLAppContextStorage::RecordTextureOnCreated(GLuint texture)
{
  if (texture == 0)
    return;
  if (m_Textures.find(texture) != m_Textures.end())
    return; // Already recorded
  m_Textures.insert(pair<GLuint, bool>(texture, true));
}

void OpenGLAppContextStorage::RecordTextureOnDeleted(GLuint texture)
{
  if (texture == 0)
    return;
  if (m_Textures.find(texture) == m_Textures.end())
    return; // Not recorded
  m_Textures.erase(texture);
}

void OpenGLAppContextStorage::RecordSamplerOnCreated(GLuint sampler)
{
  if (sampler == 0)
    return;
  if (m_Samplers.find(sampler) != m_Samplers.end())
    return; // Already recorded
  m_Samplers.insert(pair<GLuint, bool>(sampler, true));
}

void OpenGLAppContextStorage::RecordSamplerOnDeleted(GLuint sampler)
{
  if (sampler == 0)
    return;
  if (m_Samplers.find(sampler) == m_Samplers.end())
    return; // Not recorded
  m_Samplers.erase(sampler);
}

void OpenGLAppContextStorage::MarkAsDirty()
{
  m_Dirty = true;
}

bool OpenGLAppContextStorage::IsDirty()
{
  auto dirty = m_Dirty;
  m_Dirty = false;
  return dirty;
}

bool OpenGLAppContextStorage::IsChanged(OpenGLAppContextStorage *other)
{
  if (m_Programs.IsChanged(&other->m_Programs))
  {
#ifdef TR_RENDERER_ENABLE_VERBOSE
    DEBUG(LOG_TAG_RENDERER, "Programs are changed in %s", GetName());
#endif
    return true;
  }
  if (m_Shaders.IsChanged(&other->m_Shaders))
  {
#ifdef TR_RENDERER_ENABLE_VERBOSE
    DEBUG(LOG_TAG_RENDERER, "Shaders are changed in %s", GetName());
#endif
    return true;
  }
  if (m_Buffers.IsChanged(&other->m_Buffers))
  {
#ifdef TR_RENDERER_ENABLE_VERBOSE
    DEBUG(LOG_TAG_RENDERER, "Buffers are changed in %s", GetName());
#endif
    return true;
  }
  if (m_Framebuffers.IsChanged(&other->m_Framebuffers))
  {
#ifdef TR_RENDERER_ENABLE_VERBOSE
    DEBUG(LOG_TAG_RENDERER, "Framebuffers are changed in %s", GetName());
#endif
    return true;
  }
  if (m_Renderbuffers.IsChanged(&other->m_Renderbuffers))
  {
#ifdef TR_RENDERER_ENABLE_VERBOSE
    DEBUG(LOG_TAG_RENDERER, "Renderbuffers are changed in %s", GetName());
#endif
    return true;
  }
  if (m_VertexArrayObjects.IsChanged(&other->m_VertexArrayObjects))
  {
#ifdef TR_RENDERER_ENABLE_VERBOSE
    DEBUG(LOG_TAG_RENDERER, "Vertex array objects are changed in %s", GetName());
#endif
    return true;
  }
  if (m_Textures.IsChanged(&other->m_Textures))
  {
#ifdef TR_RENDERER_ENABLE_VERBOSE
    DEBUG(LOG_TAG_RENDERER, "Textures are changed in %s", GetName());
#endif
    return true;
  }
  if (m_Samplers.IsChanged(&other->m_Samplers))
  {
#ifdef TR_RENDERER_ENABLE_VERBOSE
    DEBUG(LOG_TAG_RENDERER, "Samplers are changed in %s", GetName());
#endif
    return true;
  }

  // No changes
  return false;
}
