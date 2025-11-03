#include <common/command_buffers/webgl_constants.hpp>
#include <renderer/content_renderer.hpp>
#include <renderer/renderer.hpp>
#include <renderer/render_api.hpp>

#include "./context_app.hpp"
#include "./context_host.hpp"
#include "./gpu_command_buffer_impl.hpp"
#include "./gpu_pipeline_impl.hpp"

using namespace std;

ContextGLApp::ContextGLApp(string name, shared_ptr<renderer::TrContentRenderer> content_renderer)
    : ContextGLStorage(name)
    , m_ContentRenderer(content_renderer)
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
    m_BlendFunc.reset(GL_ONE, GL_ZERO);
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

ContextGLApp::ContextGLApp(string name, ContextGLApp *from, optional<GLuint> defaultRenderTarget)
    : ContextGLStorage(name, from)
    , m_ContentRenderer(from->m_ContentRenderer)
    , m_CurrentDefaultRenderTarget(defaultRenderTarget.value_or(from->m_CurrentDefaultRenderTarget))
    , m_GLObjectManager(from->m_GLObjectManager)
{
  m_FramebufferId = m_CurrentDefaultRenderTarget;
  m_ProgramId = from->m_ProgramId;

  m_Programs = OpenGLNamesStorage(&from->m_Programs);
  m_Shaders = OpenGLNamesStorage(&from->m_Shaders);
  m_Buffers = OpenGLNamesStorage(&from->m_Buffers);
  m_Framebuffers = OpenGLNamesStorage(&from->m_Framebuffers);
  m_Renderbuffers = OpenGLNamesStorage(&from->m_Renderbuffers);
  m_VertexArrayObjects = OpenGLNamesStorage(&from->m_VertexArrayObjects);
  m_Textures = OpenGLNamesStorage(&from->m_Textures);
  m_Samplers = OpenGLNamesStorage(&from->m_Samplers);
}

void ContextGLApp::initializeContext(ContextGLHost *host_context)
{
  assert(host_context != nullptr && "Host context must not be null");
}

GLuint ContextGLApp::currentDefaultRenderTarget() const
{
  return m_CurrentDefaultRenderTarget;
}

void ContextGLApp::onFrameWillStart(ContextGLHost *host_context)
{
  m_CurrentDefaultRenderTarget = host_context->currentFramebufferId();
  glBindFramebuffer(GL_FRAMEBUFFER, m_CurrentDefaultRenderTarget);
  glClear(GL_STENCIL_BUFFER_BIT);

  ContextGLStorage::restore();
}

void ContextGLApp::onFrameEnded(ContextGLHost *host_context)
{
}

void ContextGLApp::onProgramChanged(int program)
{
  m_ProgramId = program;
}

void ContextGLApp::onArrayBufferChanged(int vao)
{
  m_ArrayBufferId = vao;
}

void ContextGLApp::onElementBufferChanged(int ebo)
{
  m_ElementArrayBufferId = ebo;
}

void ContextGLApp::onFramebufferChanged(int fbo)
{
  if (!m_FramebufferId.has_value() || m_FramebufferId != fbo)
    m_FramebufferId = fbo;
}

void ContextGLApp::onRenderbufferChanged(int rbo)
{
  m_RenderbufferId = rbo;
}

void ContextGLApp::onVertexArrayObjectChanged(int vao)
{
  m_VertexArrayObjectId = vao;
}

void ContextGLApp::onActiveTextureUnitChanged(int active_unit)
{
  m_LastActiveTextureUnit = active_unit;
}

void ContextGLApp::onTextureBindingChanged(GLenum target, GLuint texture)
{
  auto it = m_TextureBindings.find(m_LastActiveTextureUnit);
  if (it == m_TextureBindings.end())
    m_TextureBindings.emplace(m_LastActiveTextureUnit, GLTextureUnitBinding());
  m_TextureBindings[m_LastActiveTextureUnit].setBinding(target, texture);
}

void ContextGLApp::RecordProgramOnCreated(GLuint program)
{
  if (program == 0)
    return;
  if (m_Programs.find(program) != m_Programs.end())
    return; // Already recorded
  m_Programs.insert(pair<GLuint, bool>(program, true));
}

void ContextGLApp::RecordProgramOnDeleted(GLuint program)
{
  if (program == 0)
    return;
  if (m_Programs.find(program) == m_Programs.end())
    return; // Not recorded
  m_Programs.erase(program);
  // FIXME: Reset the current program if it is deleted?
}

void ContextGLApp::RecordShaderOnCreated(GLuint shader)
{
  if (shader == 0)
    return;
  if (m_Shaders.find(shader) != m_Shaders.end())
    return; // Already recorded
  m_Shaders.insert(pair<GLuint, bool>(shader, true));
}

void ContextGLApp::RecordShaderOnDeleted(GLuint shader)
{
  if (shader == 0)
    return;
  if (m_Shaders.find(shader) == m_Shaders.end())
    return; // Not recorded
  m_Shaders.erase(shader);
}

void ContextGLApp::RecordBufferOnCreated(GLuint buffer)
{
  if (buffer == 0)
    return;
  if (m_Buffers.find(buffer) != m_Buffers.end())
    return; // Already recorded
  m_Buffers.insert(pair<GLuint, bool>(buffer, true));
}

void ContextGLApp::RecordBufferOnDeleted(GLuint buffer)
{
  if (buffer == 0)
    return;
  if (m_Buffers.find(buffer) == m_Buffers.end())
    return; // Not recorded
  m_Buffers.erase(buffer);
}

void ContextGLApp::RecordFramebufferOnCreated(GLuint buffer)
{
  if (buffer == 0)
    return;
  if (m_Framebuffers.find(buffer) != m_Framebuffers.end())
    return; // Already recorded
  m_Framebuffers.insert(pair<GLuint, bool>(buffer, true));
}

void ContextGLApp::RecordFramebufferOnDeleted(GLuint buffer)
{
  if (buffer == 0)
    return;
  if (m_Framebuffers.find(buffer) == m_Framebuffers.end())
    return; // Not recorded
  m_Framebuffers.erase(buffer);
}

void ContextGLApp::RecordRenderbufferOnCreated(GLuint buffer)
{
  if (buffer == 0)
    return;
  if (m_Renderbuffers.find(buffer) != m_Renderbuffers.end())
    return; // Already recorded
  m_Renderbuffers.insert(pair<GLuint, bool>(buffer, true));
}

void ContextGLApp::RecordRenderbufferOnDeleted(GLuint buffer)
{
  if (buffer == 0)
    return;
  if (m_Renderbuffers.find(buffer) == m_Renderbuffers.end())
    return; // Not recorded
  m_Renderbuffers.erase(buffer);
}

void ContextGLApp::RecordVertexArrayObjectOnCreated(GLuint vao)
{
  if (vao == 0)
    return;
  if (m_VertexArrayObjects.find(vao) != m_VertexArrayObjects.end())
    return; // Already recorded
  m_VertexArrayObjects.insert(pair<GLuint, bool>(vao, true));
}

void ContextGLApp::RecordVertexArrayObjectOnDeleted(GLuint vao)
{
  if (vao == 0)
    return;
  if (m_VertexArrayObjects.find(vao) == m_VertexArrayObjects.end())
    return; // Not recorded
  m_VertexArrayObjects.erase(vao);
}

void ContextGLApp::RecordTextureOnCreated(GLuint texture)
{
  if (texture == 0)
    return;
  if (m_Textures.find(texture) != m_Textures.end())
    return; // Already recorded
  m_Textures.insert(pair<GLuint, bool>(texture, true));
}

void ContextGLApp::RecordTextureOnDeleted(GLuint texture)
{
  if (texture == 0)
    return;
  if (m_Textures.find(texture) == m_Textures.end())
    return; // Not recorded
  m_Textures.erase(texture);
}

void ContextGLApp::RecordSamplerOnCreated(GLuint sampler)
{
  if (sampler == 0)
    return;
  if (m_Samplers.find(sampler) != m_Samplers.end())
    return; // Already recorded
  m_Samplers.insert(pair<GLuint, bool>(sampler, true));
}

void ContextGLApp::RecordSamplerOnDeleted(GLuint sampler)
{
  if (sampler == 0)
    return;
  if (m_Samplers.find(sampler) == m_Samplers.end())
    return; // Not recorded
  m_Samplers.erase(sampler);
}

void ContextGLApp::setViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  glViewport(x, y, width, height);
  onViewportChanged(x, y, width, height);
}

void ContextGLApp::setScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
  glScissor(x, y, width, height);
}

void ContextGLApp::setClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
  glClearColor(red, green, blue, alpha);
  m_ClearColor = {red, green, blue, alpha};
}

void ContextGLApp::setClearDepth(GLfloat depth)
{
  glClearDepthf(depth);
  m_ClearDepth = depth;
}

void ContextGLApp::setClearStencil(GLint s)
{
  glClearStencil(s);
  m_ClearStencil = s;
}

GLuint ContextGLApp::createProgram(uint32_t id)
{
  GLuint program = ObjectManagerRef().CreateProgram(id);
  RecordProgramOnCreated(program);
  return program;
}

void ContextGLApp::deleteProgram(uint32_t id, GLuint &program)
{
  program = ObjectManagerRef().FindProgram(id);
  ObjectManagerRef().DeleteProgram(id);

  /**
   * Reset the program in both "AppGlobal" and "XRFrame" when we receiving a delete program command to avoid the
   * context using the deleted program.
   */
  resetProgram(program);
  RecordProgramOnDeleted(program);
}

void ContextGLApp::useProgram(uint32_t id, GLuint &program)
{
  program = ObjectManagerRef().FindProgram(id);
  glUseProgram(program);
  onProgramChanged(program);
}

void ContextGLApp::bindFramebuffer(GLenum target, optional<uint32_t> id, GLuint &framebuffer)
{
  if (!id.has_value())
  {
    framebuffer = currentDefaultRenderTarget();
  }
  else
  {
    framebuffer = ObjectManagerRef().FindFramebuffer(id.value());
  }

  glBindFramebuffer(target, framebuffer);
  onFramebufferChanged(framebuffer);
}

void ContextGLApp::activeTexture(GLenum unit)
{
  glActiveTexture(unit);
  onActiveTextureUnitChanged(unit);
}

void ContextGLApp::bindTexture(GLenum target, uint32_t id, GLuint &texture)
{
  if (id == 0)
  {
    texture = 0;
  }
  else
  {
    texture = ObjectManagerRef().FindTexture(id);
  }
  glBindTexture(target, texture);
  onTextureBindingChanged(target, texture);
}

void ContextGLApp::drawArrays(GLenum mode, GLint first, GLsizei count)
{
  if (shouldExecuteDrawOnCurrent(count)) [[likely]]
  {
    glDrawArrays(mode, first, count);
    onAfterDraw(count);
  }
}

void ContextGLApp::drawElements(GLenum mode, GLsizei count, GLenum type, const void *indices)
{
  if (shouldExecuteDrawOnCurrent(count)) [[likely]]
  {
    glDrawElements(mode, count, type, indices);
    onAfterDraw(count);
  }
}

void ContextGLApp::drawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{
  if (shouldExecuteDrawOnCurrent(count)) [[likely]]
  {
    glDrawArraysInstanced(mode, first, count, instancecount);
    onAfterDraw(count);
  }
}

void ContextGLApp::drawElementsInstanced(GLenum mode,
                                         GLsizei count,
                                         GLenum type,
                                         const GLvoid *indices,
                                         GLsizei instancecount)
{
  if (shouldExecuteDrawOnCurrent(count)) [[likely]]
  {
    glDrawElementsInstanced(mode, count, type, indices, instancecount);
    onAfterDraw(count);
  }
}

void ContextGLApp::drawRangeElements(GLenum mode,
                                     GLuint start,
                                     GLuint end,
                                     GLsizei count,
                                     GLenum type,
                                     const GLvoid *indices)
{
  if (shouldExecuteDrawOnCurrent(count)) [[likely]]
  {
    glDrawRangeElements(mode, start, end, count, type, indices);
    onAfterDraw(count);
  }
}

std::optional<GLint> ContextGLApp::getAttribLoc(commandbuffers::SetVertexAttribCommandBufferRequestBase *req) const
{
  optional<GLint> loc = nullopt;
  if (req->locationAvailable)
  {
    loc = req->location;
  }
  else
  {
    GLuint program = ObjectManagerRef().FindProgram(req->program);
    if (program != 0) [[likely]]
    {
      loc = glGetAttribLocation(program, req->locationQueryName.c_str());
      if (loc == -1)
        loc = nullopt; // If the location is not found, return nullopt
    }
  }
  return loc;
}

optional<GLint> ContextGLApp::getUniformLoc(commandbuffers::SetUniformCommandBufferRequestBase *req) const
{
  optional<GLint> loc = nullopt;
  GLuint program = ObjectManagerRef().FindProgram(req->program);
  if (program != 0) [[likely]]
  {
    loc = glGetUniformLocation(program, req->locationQueryName.c_str());
  }
  return loc;
}

void ContextGLApp::MarkAsDirty()
{
  m_Dirty = true;
}

bool ContextGLApp::IsDirty()
{
  auto dirty = m_Dirty;
  m_Dirty = false;
  return dirty;
}

bool ContextGLApp::IsChanged(ContextGLApp *other)
{
  if (m_Programs.IsChanged(&other->m_Programs))
  {
#ifdef TR_RENDERER_ENABLE_VERBOSE
    DEBUG(LOG_TAG_RENDERER, "Programs are changed in %s", name());
#endif
    return true;
  }
  if (m_Shaders.IsChanged(&other->m_Shaders))
  {
#ifdef TR_RENDERER_ENABLE_VERBOSE
    DEBUG(LOG_TAG_RENDERER, "Shaders are changed in %s", name());
#endif
    return true;
  }
  if (m_Buffers.IsChanged(&other->m_Buffers))
  {
#ifdef TR_RENDERER_ENABLE_VERBOSE
    DEBUG(LOG_TAG_RENDERER, "Buffers are changed in %s", name());
#endif
    return true;
  }
  if (m_Framebuffers.IsChanged(&other->m_Framebuffers))
  {
#ifdef TR_RENDERER_ENABLE_VERBOSE
    DEBUG(LOG_TAG_RENDERER, "Framebuffers are changed in %s", name());
#endif
    return true;
  }
  if (m_Renderbuffers.IsChanged(&other->m_Renderbuffers))
  {
#ifdef TR_RENDERER_ENABLE_VERBOSE
    DEBUG(LOG_TAG_RENDERER, "Renderbuffers are changed in %s", name());
#endif
    return true;
  }
  if (m_VertexArrayObjects.IsChanged(&other->m_VertexArrayObjects))
  {
#ifdef TR_RENDERER_ENABLE_VERBOSE
    DEBUG(LOG_TAG_RENDERER, "Vertex array objects are changed in %s", name());
#endif
    return true;
  }
  if (m_Textures.IsChanged(&other->m_Textures))
  {
#ifdef TR_RENDERER_ENABLE_VERBOSE
    DEBUG(LOG_TAG_RENDERER, "Textures are changed in %s", name());
#endif
    return true;
  }
  if (m_Samplers.IsChanged(&other->m_Samplers))
  {
#ifdef TR_RENDERER_ENABLE_VERBOSE
    DEBUG(LOG_TAG_RENDERER, "Samplers are changed in %s", name());
#endif
    return true;
  }

  // No changes
  return false;
}

bool ContextGLApp::IsDefaultRenderTargetBinding() const
{
  return !m_FramebufferId.has_value() ||
         m_FramebufferId == m_CurrentDefaultRenderTarget;
  return true;
}

renderer::TrContentRenderer &ContextGLApp::contentRendererChecked() const
{
  auto contentRenderer = m_ContentRenderer.lock();
  assert(contentRenderer != nullptr && "Content renderer must not be null");
  return *contentRenderer;
}

bool ContextGLApp::shouldExecuteDrawOnCurrent(GLsizei count)
{
  assert(count < WEBGL_MAX_COUNT_PER_DRAWCALL);
  if (m_FramebufferId.has_value() &&
      m_FramebufferId.value() == 0)
  {
    DEBUG(LOG_TAG_ERROR, "Skip this draw: the framebuffer is not set.");
    return false;
  }
  return true;
}

void ContextGLApp::onAfterDraw(int draw_count)
{
  contentRendererChecked().increaseDrawCallsCount(draw_count);
}
