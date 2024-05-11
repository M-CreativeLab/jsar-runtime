#include "context_storage.hpp"

void OpenGLContextStorage::RecordViewport(int x, int y, int w, int h)
{
  m_Viewport[0] = x;
  m_Viewport[1] = y;
  m_Viewport[2] = w;
  m_Viewport[3] = h;
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
  {
    binding->Reset(target, texture);
  }
}

void OpenGLContextStorage::ResetProgram(int programToReset)
{
  if (m_ProgramId == programToReset)
    m_ProgramId = 0;
}

void OpenGLContextStorage::Restore()
{
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
  if (m_ProgramId >= 0)
    glUseProgram(m_ProgramId);
  else
    glUseProgram(0);
  useProgramError = glGetError();

  if (m_ArrayBufferId >= 0)
    glBindBuffer(GL_ARRAY_BUFFER, m_ArrayBufferId);
  if (m_ElementArrayBufferId >= 0)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementArrayBufferId);
  if (m_FramebufferId >= 0)
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
  if (m_RenderbufferId >= 0)
    glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferId);
  bindBuffersError = glGetError();

  if (m_VertexArrayObjectId >= 0)
    glBindVertexArray(m_VertexArrayObjectId);

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
    DEBUG(DEBUG_TAG, "EGL context lost, need to reload the context.");
  else if (eglError != EGL_SUCCESS)
    DEBUG(DEBUG_TAG, "Occurs an EGL error: 0x%04X", eglError);
#endif

  if (useProgramError != GL_NO_ERROR)
    DEBUG(DEBUG_TAG, "Occurs an error in glUseProgram(%d) when restoring %s context: 0x%04X",
          m_ProgramId, GetName(), useProgramError);
  if (bindBuffersError != GL_NO_ERROR)
    DEBUG(DEBUG_TAG, "Occurs an error in buffers binding when restoring %s context: 0x%04X",
          GetName(), bindBuffersError);
  if (bindTextureError != GL_NO_ERROR)
    DEBUG(DEBUG_TAG, "Occurs an error in texture bindings when restoring %s context: 0x%04X",
          GetName(), bindTextureError);

  // Check for OpenGL errors
  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
    DEBUG(DEBUG_TAG, "Occurs an OpenGL error in restoring %s context: 0x%04X", error, GetName());
}

void OpenGLContextStorage::Print()
{
  DEBUG(DEBUG_TAG, "%s program(%d), framebuffer(%d)", GetName(), m_ProgramId, m_FramebufferId);
}

void OpenGLContextStorage::ClearTextureBindings()
{
  for (auto it = m_TextureBindingsWithUnit.begin(); it != m_TextureBindingsWithUnit.end(); it++)
    delete it->second;
  m_TextureBindingsWithUnit.clear();
}

void OpenGLHostContextStorage::Restore()
{
  OpenGLContextStorage::Restore();
  glFrontFace(m_FrontFace);
}

void OpenGLHostContextStorage::Record()
{
  glGetIntegerv(GL_VIEWPORT, m_Viewport);
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
    GLint texture;
    glActiveTexture(i);
    // TODO: how to support other texture targets?
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture);
    m_TextureBindingsWithUnit[i] = new OpenGLTextureBinding(GL_TEXTURE_2D, texture);
  }
  glActiveTexture(m_LastActiveTextureUnit);

  // Enable or disable
  m_CullFaceEnabled = glIsEnabled(GL_CULL_FACE);

  // States
  glGetIntegerv(GL_CULL_FACE_MODE, (GLint *)&m_CullFace);
  glGetIntegerv(GL_FRONT_FACE, (GLint *)&m_FrontFace);

  // Check for errors
  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
    DEBUG(DEBUG_TAG, "Occurs an OpenGL error in recording %s context: 0x%04X", error, GetName());
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
