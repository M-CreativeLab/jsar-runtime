#pragma once

#include <string>
#include <map>

#include "common/viewport.hpp"
#include "./common.hpp"

class OpenGLTextureBinding
{
public:
  OpenGLTextureBinding(GLenum target, GLuint texture) : m_Target(target), m_Texture(texture) {}
  OpenGLTextureBinding(OpenGLTextureBinding *from) : m_Target(from->m_Target), m_Texture(from->m_Texture) {}
  inline void Reset(GLenum target, GLuint texture)
  {
    m_Target = target;
    m_Texture = texture;
  }
  inline GLenum GetTarget() { return m_Target; }
  inline GLint GetTexture() { return m_Texture; }

public:
  GLenum m_Target;
  GLuint m_Texture;
};

class OpenGLBlendingFunc
{
public:
  OpenGLBlendingFunc() : m_Src(GL_ONE), m_Dst(GL_ZERO), m_IsSeparate(false) {}
  OpenGLBlendingFunc(OpenGLBlendingFunc *from)
      : m_Src(from->m_Src),
        m_Dst(from->m_Dst),
        m_SrcAlpha(from->m_SrcAlpha),
        m_DstAlpha(from->m_DstAlpha)
  {
  }

public:
  inline bool IsSeparate() { return m_IsSeparate; }
  inline GLenum GetSrc() { return m_Src; }
  inline GLenum GetDst() { return m_Dst; }
  inline GLenum GetSrcRgb() { return m_Src; }
  inline GLenum GetDstRgb() { return m_Dst; }
  inline GLenum GetSrcAlpha() { return m_SrcAlpha; }
  inline GLenum GetDstAlpha() { return m_DstAlpha; }
  inline void Reset(GLenum src, GLenum dst)
  {
    m_Src = src;
    m_Dst = dst;
    m_IsSeparate = false;
  }
  inline void Reset(GLenum srcRgb, GLenum dstRgb, GLenum srcAlpha, GLenum dstAlpha)
  {
    m_Src = srcRgb;
    m_Dst = dstRgb;
    m_SrcAlpha = srcAlpha;
    m_DstAlpha = dstAlpha;
    m_IsSeparate = true;
  }

private:
  bool m_IsSeparate = false;
  GLenum m_Src;
  GLenum m_Dst;
  GLenum m_SrcAlpha;
  GLenum m_DstAlpha;
};

class OpenGLContextStorage
{
public:
  OpenGLContextStorage(std::string name) : m_Name(name)
  {
    glGetBooleanv(GL_CULL_FACE, &m_CullFaceEnabled);
    glGetBooleanv(GL_DEPTH_TEST, &m_DepthTestEnabled);
  }
  OpenGLContextStorage(std::string name, OpenGLContextStorage *from) : m_Name(name)
  {
    m_Viewport[0] = from->m_Viewport[0];
    m_Viewport[1] = from->m_Viewport[1];
    m_Viewport[2] = from->m_Viewport[2];
    m_Viewport[3] = from->m_Viewport[3];
    m_CullFaceEnabled = from->m_CullFaceEnabled;
    m_DepthTestEnabled = from->m_DepthTestEnabled;
    m_DepthMask = from->m_DepthMask;
    m_ProgramId = from->m_ProgramId;
    m_ArrayBufferId = from->m_ArrayBufferId;
    m_ElementArrayBufferId = from->m_ElementArrayBufferId;
    m_FramebufferId = from->m_FramebufferId;
    m_RenderbufferId = from->m_RenderbufferId;
    m_VertexArrayObjectId = from->m_VertexArrayObjectId;
    m_LastActiveTextureUnit = from->m_LastActiveTextureUnit;
    for (auto it = from->m_TextureBindingsWithUnit.begin(); it != from->m_TextureBindingsWithUnit.end(); it++)
      m_TextureBindingsWithUnit[it->first] = new OpenGLTextureBinding(it->second);
  }
  ~OpenGLContextStorage()
  {
    ClearTextureBindings();
  }

  void RecordViewport(int x, int y, int w, int h);
  void RecordCapability(GLenum cap, bool enabled);
  void RecordCullFace(GLenum mode);
  void RecordFrontFace(GLenum mode);
  void RecordDepthMask(bool enabled);
  void RecordBlendFunc(GLenum sfactor, GLenum dfactor);
  void RecordBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
  void RecordProgram(int program);
  void RecordArrayBuffer(int buffer);
  void RecordElementArrayBuffer(int buffer);
  void RecordFramebuffer(int buffer);
  void RecordRenderbuffer(int buffer);
  void RecordVertexArrayObject(int vao);
  void RecordActiveTextureUnit(int unit);
  void RecordTextureBindingWithUnit(GLenum target, GLuint texture);

  const char *GetName() { return m_Name.c_str(); }
  TrViewport GetViewport() { return TrViewport(m_Viewport[2], m_Viewport[3], m_Viewport[0], m_Viewport[1]); }
  GLint GetProgram() { return m_ProgramId; }
  GLint GetArrayBuffer() { return m_ArrayBufferId; }
  GLint GetElementArrayBuffer() { return m_ElementArrayBufferId; }
  GLint GetFramebuffer() { return m_FramebufferId; }
  GLint GetRenderbuffer() { return m_RenderbufferId; }
  GLint GetVertexArrayObject() { return m_VertexArrayObjectId; }
  GLenum GetActiveTextureUnit() { return m_LastActiveTextureUnit; }

  void ResetProgram(int programToReset);
  void Restore();
  void Print();
  void ClearTextureBindings();

protected:
  std::string m_Name;
  GLint m_Viewport[4] = {-1, -1, -1, -1};
  /** States */
  // Culling & face
  GLboolean m_CullFaceEnabled;
  GLenum m_CullFace;
  GLenum m_FrontFace;
  // Depth
  GLboolean m_DepthTestEnabled;
  GLboolean m_DepthMask; // If depth buffer writing is enabled
  GLenum m_DepthFunc = GL_LEQUAL;
  // Blending
  GLboolean m_BlendEnabled;
  OpenGLBlendingFunc m_BlendFunc;
  /** Program */
  GLint m_ProgramId = 0;
  /** Buffers */
  GLint m_ArrayBufferId = 0;
  GLint m_ElementArrayBufferId = 0;
  GLint m_FramebufferId = 0;
  GLint m_RenderbufferId = 0;
  /** Vertex Array Object */
  GLint m_VertexArrayObjectId = 0;
  /** Textures */
  GLenum m_LastActiveTextureUnit = GL_TEXTURE0;
  std::map<GLenum, OpenGLTextureBinding *> m_TextureBindingsWithUnit;
  bool m_ForceChanged = false;
};

class OpenGLHostContextStorage : public OpenGLContextStorage
{
public:
  OpenGLHostContextStorage() : OpenGLContextStorage("Host")
  {
    Record();
  }

public:
  void Restore();
  void Record();
  void RecordTextureBindingFromHost();
};

class OpenGLNamesStorage : public std::map<GLuint, bool>
{
public:
  OpenGLNamesStorage() : std::map<GLuint, bool>() {}
  OpenGLNamesStorage(OpenGLNamesStorage *from)
  {
    for (auto it = from->begin(); it != from->end(); it++)
      insert(std::pair<GLuint, bool>(it->first, it->second));
  }

public:
  bool IsChanged(OpenGLNamesStorage *other)
  {
    if (size() != other->size())
      return true;
    for (auto it = begin(); it != end(); it++)
    {
      if (other->find(it->first) == other->end())
        return true;
    }
    return false;
  }
};

class OpenGLAppContextStorage : public OpenGLContextStorage
{
public:
  OpenGLAppContextStorage(std::string name) : OpenGLContextStorage(name)
  {
    /**
     * Initial values for WebGL or OpenGLES.
     */
    m_CullFaceEnabled = true;
    m_CullFace = GL_BACK;
    m_FrontFace = GL_CCW;
  }
  OpenGLAppContextStorage(std::string name, OpenGLAppContextStorage *from) : OpenGLContextStorage(name, from)
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

public:
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

public:
  void MarkAsDirty();
  bool IsDirty();
  bool IsChanged(OpenGLAppContextStorage *other);

private:
  bool m_Dirty = false;
  OpenGLNamesStorage m_Programs;
  OpenGLNamesStorage m_Shaders;
  OpenGLNamesStorage m_Buffers;
  OpenGLNamesStorage m_Framebuffers;
  OpenGLNamesStorage m_Renderbuffers;
  OpenGLNamesStorage m_VertexArrayObjects;
  OpenGLNamesStorage m_Textures;
  OpenGLNamesStorage m_Samplers;
};
