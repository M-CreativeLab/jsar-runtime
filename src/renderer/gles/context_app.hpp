#pragma once

#include <optional>
#include <memory>
#include <unordered_map>
#include <common/classes.hpp>

#include "./context_storage.hpp"
#include "./framebuffer.hpp"
#include "./gpu_command_encoder_impl.hpp"

class ContextGLHost;
class ContextGLApp : public ContextGLStorage
{
public:
  ContextGLApp(std::string name, std::shared_ptr<renderer::TrContentRenderer>);
  ContextGLApp(std::string name, ContextGLApp *from);

public:
  // Initialize the render target(framebuffer) for this context.
  void initializeContext(ContextGLHost *);
  GLuint currentDefaultRenderTarget() const;

  void onFrameWillStart(ContextGLHost *);
  void onFrameEnded(ContextGLHost *);

  void onProgramChanged(int program);
  void onArrayBufferChanged(int vbo);
  void onElementBufferChanged(int ebo);
  void onFramebufferChanged(int fbo);
  void onRenderbufferChanged(int rbo);
  void onVertexArrayObjectChanged(int vao);
  void onActiveTextureUnitChanged(int unit);
  // Record the texture binding changed.
  void onTextureBindingChanged(GLenum target, GLuint texture);

  void RecordProgramOnCreated(GLuint program);
  void RecordProgramOnDeleted(GLuint program);
  void RecordShaderOnCreated(GLuint shader);
  void RecordShaderOnDeleted(GLuint shader);
  void RecordBufferOnCreated(GLuint buffer);
  void RecordBufferOnDeleted(GLuint buffer);
  void RecordFramebufferOnCreated(GLuint buffer);
  void RecordFramebufferOnDeleted(GLuint buffer);
  void RecordRenderbufferOnCreated(GLuint buffer);
  void RecordRenderbufferOnDeleted(GLuint buffer);
  void RecordVertexArrayObjectOnCreated(GLuint vao);
  void RecordVertexArrayObjectOnDeleted(GLuint vao);
  void RecordTextureOnCreated(GLuint texture);
  void RecordTextureOnDeleted(GLuint texture);
  void RecordSamplerOnCreated(GLuint sampler);
  void RecordSamplerOnDeleted(GLuint sampler);

public: // GLES Implementations
  GLuint createProgram(uint32_t id);
  void deleteProgram(uint32_t id, GLuint &program);
  void useProgram(uint32_t id, GLuint &program);

  // Framebuffer functions
  void bindFramebuffer(GLenum target, uint32_t id, GLuint &framebuffer);

  // Draw functions
  void drawArrays(GLenum mode, GLint first, GLsizei count);
  void drawElements(GLenum mode, GLsizei count, GLenum type, const void *indices);

public:
  void MarkAsDirty();
  bool IsDirty();
  bool IsChanged(ContextGLApp *other);

public:
  renderer::TrContentRenderer &contentRendererChecked() const;
  gles::GLObjectManager &ObjectManagerRef()
  {
    return *m_GLObjectManager;
  }

private:
  // This updates the current `GPUCommandEncoder` to be used for recording commands.
  void updateCurrentCommandEncoder();

  [[nodiscard]] bool shouleExecuteDrawOnCurrent(GLsizei count);
  void onAfterDraw(int drawCount);

private:
  bool m_Dirty = false;
  std::weak_ptr<renderer::TrContentRenderer> m_ContentRenderer;

  GLuint m_CurrentDefaultRenderTarget;
  std::shared_ptr<gles::GPUCommandEncoderImpl> m_CurrentCommandEncoder = nullptr;

  std::shared_ptr<gles::GLObjectManager> m_GLObjectManager;
  OpenGLNamesStorage m_Programs;
  OpenGLNamesStorage m_Shaders;
  OpenGLNamesStorage m_Buffers;
  OpenGLNamesStorage m_Framebuffers;
  OpenGLNamesStorage m_Renderbuffers;
  OpenGLNamesStorage m_VertexArrayObjects;
  OpenGLNamesStorage m_Textures;
  OpenGLNamesStorage m_Samplers;
};
