#include "context_storage.hpp"

void OpenGLContextStorage::RecordViewport(int x, int y, int w, int h)
{
  m_Viewport[0] = x;
  m_Viewport[1] = y;
  m_Viewport[2] = w;
  m_Viewport[3] = h;
}

void OpenGLContextStorage::RecordCapability(GLenum cap, bool enabled)
{
  if (cap == GL_CULL_FACE)
    m_CullFaceEnabled = enabled;
  else if (cap == GL_DEPTH_TEST)
    m_DepthTestEnabled = enabled;
  else if (cap == GL_BLEND)
    m_BlendEnabled = enabled;
}

void OpenGLContextStorage::RecordCullFace(GLenum mode)
{
  m_CullFace = mode;
}

void OpenGLContextStorage::RecordFrontFace(GLenum mode)
{
  m_FrontFace = mode;
}

void OpenGLContextStorage::RecordDepthMask(bool enabled)
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

  auto binding = m_TextureBindingsWithUnit[activeUnit];
  if (binding == nullptr)
    m_TextureBindingsWithUnit[activeUnit] = new OpenGLTextureBinding(target, texture);
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
  m_BlendEnabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);

  /**
   * FIXME: When the cull face is enabled, restore the cullface and frontface states.
   */
  if (m_CullFaceEnabled)
    glCullFace(m_CullFace);
  if (m_FrontFace == GL_CW || m_FrontFace == GL_CCW)
    glFrontFace(m_FrontFace);

  glDepthMask(m_DepthMask);
  if (m_DepthTestEnabled)
    glDepthFunc(m_DepthFunc); // TODO: valid depth func enum?

  if (m_BlendEnabled)
  {
    if (!m_BlendFunc.IsSeparate())
      glBlendFunc(m_BlendFunc.GetSrc(), m_BlendFunc.GetDst());
    else
      glBlendFuncSeparate(m_BlendFunc.GetSrcRgb(), m_BlendFunc.GetDstRgb(),
                          m_BlendFunc.GetSrcAlpha(), m_BlendFunc.GetDstAlpha());
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
    auto binding = it->second;
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
    DEBUG(LOG_TAG_ERROR, "Occurs an error in glViewport(%d, %d, %d, %d) when restoring %s context: 0x%04X",
          m_Viewport[0], m_Viewport[1], m_Viewport[2], m_Viewport[3], GetName(), setViewportError);
  if (useProgramError != GL_NO_ERROR)
    DEBUG(LOG_TAG_ERROR, "Occurs an error in glUseProgram(%d) when restoring %s context: 0x%04X",
          m_ProgramId, GetName(), useProgramError);
  if (bindBuffersError != GL_NO_ERROR)
    DEBUG(LOG_TAG_ERROR, "Occurs an error in buffers binding when restoring %s context: 0x%04X",
          GetName(), bindBuffersError);
  if (bindTextureError != GL_NO_ERROR)
    DEBUG(LOG_TAG_ERROR, "Occurs an error in texture bindings when restoring %s context: 0x%04X",
          GetName(), bindTextureError);

  // Check for OpenGL errors
  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
    DEBUG(LOG_TAG_ERROR, "Occurs an OpenGL error in restoring %s context: 0x%04X", error, GetName());
}

void OpenGLContextStorage::Print()
{
  DEBUG(DEBUG_TAG, "%s program(%d), viewport(%d, %d, %d, %d)", GetName(),
        m_ProgramId, m_Viewport[0], m_Viewport[1], m_Viewport[2], m_Viewport[3]);
  DEBUG(DEBUG_TAG, "%s ebo(%d), vao(%d), framebuffer(%d), activeTexture(%d)", GetName(),
        m_ElementArrayBufferId, m_VertexArrayObjectId, m_FramebufferId, m_LastActiveTextureUnit - GL_TEXTURE0);
}

void OpenGLContextStorage::ClearTextureBindings()
{
  for (auto it = m_TextureBindingsWithUnit.begin(); it != m_TextureBindingsWithUnit.end(); it++)
    delete it->second;
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
    m_TextureBindingsWithUnit[i] = new OpenGLTextureBinding(GL_TEXTURE_2D, texture);
  }
  glActiveTexture(m_LastActiveTextureUnit);

  // Enable or disable
  m_CullFaceEnabled = glIsEnabled(GL_CULL_FACE);
  m_DepthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
  m_BlendEnabled = glIsEnabled(GL_BLEND);

  // States
  glGetIntegerv(GL_CULL_FACE_MODE, (GLint *)&m_CullFace);
  glGetIntegerv(GL_FRONT_FACE, (GLint *)&m_FrontFace);
  glGetIntegerv(GL_DEPTH_FUNC, (GLint *)&m_DepthFunc);
  glGetBooleanv(GL_DEPTH_WRITEMASK, &m_DepthMask);

  // Blend funcs
  {
    GLenum sfactor, dfactor;
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint *)&sfactor);
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint *)&dfactor);
    m_BlendFunc.Reset(sfactor, dfactor);
  }

  // Check for errors
  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
    DEBUG(LOG_TAG_ERROR, "Occurs an OpenGL error in recording %s context: 0x%04X", GetName(), error);
}

void OpenGLHostContextStorage::RecordTextureBindingFromHost()
{
  auto binding = m_TextureBindingsWithUnit[m_LastActiveTextureUnit];
  if (binding != nullptr)
    return;

  GLuint texture;
  GLint beforeActiveUnit;
  glGetIntegerv(GL_ACTIVE_TEXTURE, &beforeActiveUnit);

  bool isActiveNotMatched = beforeActiveUnit != m_LastActiveTextureUnit;
  if (isActiveNotMatched)
    glActiveTexture(m_LastActiveTextureUnit);

  glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&texture);
  m_TextureBindingsWithUnit[m_LastActiveTextureUnit] = new OpenGLTextureBinding(GL_TEXTURE_2D, texture);

  if (isActiveNotMatched)
    glActiveTexture(beforeActiveUnit);
}

void OpenGLAppContextStorage::RecordProgramOnCreated(GLuint program)
{
  if (program == 0)
    return;
  if (m_Programs.find(program) != m_Programs.end())
    return; // Already recorded
  m_Programs.insert(std::pair<GLuint, bool>(program, true));
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
  m_Shaders.insert(std::pair<GLuint, bool>(shader, true));
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
  m_Buffers.insert(std::pair<GLuint, bool>(buffer, true));
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
  m_Framebuffers.insert(std::pair<GLuint, bool>(buffer, true));
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
  m_Renderbuffers.insert(std::pair<GLuint, bool>(buffer, true));
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
  m_VertexArrayObjects.insert(std::pair<GLuint, bool>(vao, true));
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
  m_Textures.insert(std::pair<GLuint, bool>(texture, true));
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
  m_Samplers.insert(std::pair<GLuint, bool>(sampler, true));
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
  if (m_ProgramId != other->m_ProgramId)
    return true;
  if (m_ArrayBufferId != other->m_ArrayBufferId)
    return true;
  if (m_ElementArrayBufferId != other->m_ElementArrayBufferId)
    return true;
  if (m_FramebufferId != other->m_FramebufferId)
    return true;
  if (m_RenderbufferId != other->m_RenderbufferId)
    return true;
  if (m_VertexArrayObjectId != other->m_VertexArrayObjectId)
    return true;
  if (m_LastActiveTextureUnit != other->m_LastActiveTextureUnit)
    return true;

  if (m_Programs.IsChanged(&other->m_Programs))
    return true;
  if (m_Shaders.IsChanged(&other->m_Shaders))
    return true;
  if (m_Buffers.IsChanged(&other->m_Buffers))
    return true;
  if (m_Framebuffers.IsChanged(&other->m_Framebuffers))
    return true;
  if (m_Renderbuffers.IsChanged(&other->m_Renderbuffers))
    return true;
  if (m_VertexArrayObjects.IsChanged(&other->m_VertexArrayObjects))
    return true;
  if (m_Textures.IsChanged(&other->m_Textures))
    return true;
  if (m_Samplers.IsChanged(&other->m_Samplers))
    return true;

  return false;
}
