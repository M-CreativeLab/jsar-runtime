#pragma once

#include <string>
#include <map>
#include <memory>

#include "common/viewport.hpp"
#include "./common.hpp"
#include "./object_manager.hpp"

class OpenGLTextureBinding
{
public:
  OpenGLTextureBinding(GLenum target, GLuint texture)
      : m_Target(target)
      , m_Texture(texture)
  {
  }
  OpenGLTextureBinding(OpenGLTextureBinding &from)
      : m_Target(from.m_Target)
      , m_Texture(from.m_Texture)
  {
  }
  inline void Reset(GLenum target, GLuint texture)
  {
    m_Target = target;
    m_Texture = texture;
  }
  inline GLenum GetTarget()
  {
    return m_Target;
  }
  inline GLint GetTexture()
  {
    return m_Texture;
  }

public:
  GLenum m_Target;
  GLuint m_Texture;
};

class OpenGLBlendingFunc
{
public:
  OpenGLBlendingFunc()
      : m_Src(GL_ONE)
      , m_Dst(GL_ZERO)
      , m_IsSeparate(false)
  {
  }
  OpenGLBlendingFunc(OpenGLBlendingFunc *from)
      : m_Src(from->m_Src)
      , m_Dst(from->m_Dst)
      , m_SrcAlpha(from->m_SrcAlpha)
      , m_DstAlpha(from->m_DstAlpha)
  {
  }

public:
  inline bool IsSeparate()
  {
    return m_IsSeparate;
  }
  inline GLenum GetSrc()
  {
    return m_Src;
  }
  inline GLenum GetDst()
  {
    return m_Dst;
  }
  inline GLenum GetSrcRgb()
  {
    return m_Src;
  }
  inline GLenum GetDstRgb()
  {
    return m_Dst;
  }
  inline GLenum GetSrcAlpha()
  {
    return m_SrcAlpha;
  }
  inline GLenum GetDstAlpha()
  {
    return m_DstAlpha;
  }
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
  void Print()
  {
    DEBUG(DEBUG_TAG, "OpenGLBlendingFunc");
    DEBUG(DEBUG_TAG, "  Separate: %s", m_IsSeparate ? "Yes" : "No");
    if (m_IsSeparate)
    {
      DEBUG(DEBUG_TAG, "  SrcRGB: %d", m_Src);
      DEBUG(DEBUG_TAG, "  DstRGB: %d", m_Dst);
      DEBUG(DEBUG_TAG, "  SrcAlpha: %d", m_SrcAlpha);
      DEBUG(DEBUG_TAG, "  DstAlpha: %d", m_DstAlpha);
    }
    else
    {
      DEBUG(DEBUG_TAG, "  SrcRGB: %d", m_Src);
      DEBUG(DEBUG_TAG, "  DstRGB: %d", m_Dst);
    }

    DEBUG(DEBUG_TAG, "  Current OpenGL states:");
    GLint blendDstAlpha;
    glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDstAlpha);
    GLint blendDstRGB;
    glGetIntegerv(GL_BLEND_DST_RGB, &blendDstRGB);
    DEBUG(DEBUG_TAG, "  DstAlpha=%s", gles::glBlendFuncToString(blendDstAlpha).c_str());
    DEBUG(DEBUG_TAG, "  DstRGB=%s", gles::glBlendFuncToString(blendDstRGB).c_str());

    GLint blendSrcAlpha;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrcAlpha);
    GLint blendSrcRGB;
    glGetIntegerv(GL_BLEND_SRC_RGB, &blendSrcRGB);
    DEBUG(DEBUG_TAG, "  SrcAlpha=%s", gles::glBlendFuncToString(blendSrcAlpha).c_str());
    DEBUG(DEBUG_TAG, "  SrcRGB=%s", gles::glBlendFuncToString(blendSrcRGB).c_str());
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
  struct Rect
  {
    GLint x = 0;
    GLint y = 0;
    GLsizei width = 0;
    GLsizei height = 0;
  };
  struct StencilFuncParameters
  {
    GLenum func = GL_ALWAYS;
    GLint ref = 0;
    GLuint mask = 1;

    bool operator==(const StencilFuncParameters &other) const
    {
      return (func == other.func) && (ref == other.ref) && (mask == other.mask);
    }
    bool operator!=(const StencilFuncParameters &other) const
    {
      return !(*this == other);
    }
  };
  struct StencilOpParameters
  {
    GLenum sfail = GL_KEEP;
    GLenum dpfail = GL_KEEP;
    GLenum dppass = GL_KEEP;

    bool operator==(const StencilOpParameters &other) const
    {
      return (sfail == other.sfail) && (dpfail == other.dpfail) && (dppass == other.dppass);
    }
    bool operator!=(const StencilOpParameters &other) const
    {
      return !(*this == other);
    }
  };
  struct PolygonOffsetParameters
  {
    GLfloat factor = 0.0f;
    GLfloat units = 0.0f;
  };

public:
  OpenGLContextStorage(std::string name)
      : m_Name(name)
  {
    glGetBooleanv(GL_CULL_FACE, &m_CullFaceEnabled);
    glGetBooleanv(GL_DEPTH_TEST, &m_DepthTestEnabled);
  }
  OpenGLContextStorage(std::string name, OpenGLContextStorage *from)
      : m_Name(name)
  {
    // Viewport
    m_Viewport[0] = from->m_Viewport[0];
    m_Viewport[1] = from->m_Viewport[1];
    m_Viewport[2] = from->m_Viewport[2];
    m_Viewport[3] = from->m_Viewport[3];

    // States
    m_CullFaceEnabled = from->m_CullFaceEnabled;
    {
      m_ColorMask[0] = from->m_ColorMask[0];
      m_ColorMask[1] = from->m_ColorMask[1];
      m_ColorMask[2] = from->m_ColorMask[2];
      m_ColorMask[3] = from->m_ColorMask[3];
    }
    m_DepthTestEnabled = from->m_DepthTestEnabled;
    m_DepthMask = from->m_DepthMask;
    m_DepthFunc = from->m_DepthFunc;
    m_DepthRange[0] = from->m_DepthRange[0];
    m_DepthRange[1] = from->m_DepthRange[1];
    m_DitherEnabled = from->m_DitherEnabled;
    m_BlendEnabled = from->m_BlendEnabled;
    m_BlendFunc = OpenGLBlendingFunc(&from->m_BlendFunc);
    m_StencilTestEnabled = from->m_StencilTestEnabled;
    m_StencilMask = from->m_StencilMask;
    m_StencilMaskBack = from->m_StencilMaskBack;
    m_StencilFunc = from->m_StencilFunc;
    m_StencilFuncBack = from->m_StencilFuncBack;
    m_StencilOp = from->m_StencilOp;
    m_StencilOpBack = from->m_StencilOpBack;
    m_ScissorTestEnabled = from->m_ScissorTestEnabled;
    m_ScissorBox = from->m_ScissorBox;
    m_LineWidth = from->m_LineWidth;
    m_PolygonOffset = from->m_PolygonOffset;

    // Objects
    m_ProgramId = from->m_ProgramId;
    m_ArrayBufferId = from->m_ArrayBufferId;
    m_ElementArrayBufferId = from->m_ElementArrayBufferId;
    m_FramebufferId = from->m_FramebufferId;
    m_RenderbufferId = from->m_RenderbufferId;
    m_VertexArrayObjectId = from->m_VertexArrayObjectId;
    m_LastActiveTextureUnit = from->m_LastActiveTextureUnit;
    for (auto it = from->m_TextureBindingsWithUnit.begin(); it != from->m_TextureBindingsWithUnit.end(); it++)
      m_TextureBindingsWithUnit[it->first] = std::make_shared<OpenGLTextureBinding>(*it->second);
  }
  ~OpenGLContextStorage()
  {
  }

  void RecordViewport(int x, int y, int w, int h);
  void RecordCapability(GLenum cap, bool enabled);
  void RecordCullFace(GLenum mode);
  void RecordFrontFace(GLenum mode);
  void RecordColorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a);
  void RecordDepthMask(GLboolean enabled);
  void RecordBlendFunc(GLenum sfactor, GLenum dfactor);
  void RecordBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
  void RecordStencilMask(GLenum face, GLuint mask);
  void RecordStencilFunc(GLenum face, GLenum func, GLint ref, GLuint mask);
  void RecordStencilOp(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
  void RecordProgram(int program);
  void RecordArrayBuffer(int buffer);
  void RecordElementArrayBuffer(int buffer);
  void RecordFramebuffer(int buffer);
  void RecordRenderbuffer(int buffer);
  void RecordVertexArrayObject(int vao);
  void RecordActiveTextureUnit(int unit);
  void RecordTextureBindingWithUnit(GLenum target, GLuint texture);

  const char *GetName()
  {
    return m_Name.c_str();
  }
  TrViewport GetViewport()
  {
    return TrViewport(m_Viewport[2], m_Viewport[3], m_Viewport[0], m_Viewport[1]);
  }
  GLint GetProgram()
  {
    return m_ProgramId;
  }
  GLint GetArrayBuffer()
  {
    return m_ArrayBufferId;
  }
  GLint GetElementArrayBuffer()
  {
    return m_ElementArrayBufferId;
  }
  GLint GetFramebuffer()
  {
    return m_FramebufferId;
  }
  GLint GetRenderbuffer()
  {
    return m_RenderbufferId;
  }
  GLint GetVertexArrayObject()
  {
    return m_VertexArrayObjectId;
  }
  GLenum GetActiveTextureUnit()
  {
    return m_LastActiveTextureUnit;
  }

  void ResetProgram(int programToReset);
  void Restore();
  void Print();
  void ClearTextureBindings();

protected:
  std::string m_Name;
  GLint m_Viewport[4] = {-1, -1, -1, -1};
  bool m_ForceChanged = false;

protected: /** Global States */
  // Culling & face
  GLboolean m_CullFaceEnabled;
  GLenum m_CullFace;
  GLenum m_FrontFace;
  // Color
  GLboolean m_ColorMask[4]; // [reg, green, blue, alpha]
  // Depth
  GLboolean m_DepthTestEnabled;
  GLboolean m_DepthMask; // If depth buffer writing is enabled
  GLenum m_DepthFunc = GL_LEQUAL;
  GLfloat m_DepthRange[2] = {0.0f, 1.0f};
  // Dither
  GLboolean m_DitherEnabled;
  // Blending
  GLboolean m_BlendEnabled;
  OpenGLBlendingFunc m_BlendFunc;
  // Stencil
  GLboolean m_StencilTestEnabled;
  GLuint m_StencilMask;
  GLuint m_StencilMaskBack;
  StencilFuncParameters m_StencilFunc;
  StencilFuncParameters m_StencilFuncBack;
  StencilOpParameters m_StencilOp;
  StencilOpParameters m_StencilOpBack;
  // Scissor
  GLboolean m_ScissorTestEnabled;
  Rect m_ScissorBox;
  // Others
  GLfloat m_LineWidth = 1.0f;
  PolygonOffsetParameters m_PolygonOffset;

protected: /** OpenGLES objects */
  GLint m_ProgramId = 0;
  GLint m_ArrayBufferId = 0;
  GLint m_ElementArrayBufferId = 0;
  GLint m_FramebufferId = 0;
  GLint m_RenderbufferId = 0;
  GLint m_VertexArrayObjectId = 0;
  GLenum m_LastActiveTextureUnit = GL_TEXTURE0;
  std::map<GLenum, std::shared_ptr<OpenGLTextureBinding>> m_TextureBindingsWithUnit;
};

class OpenGLHostContextStorage : public OpenGLContextStorage
{
public:
  OpenGLHostContextStorage()
      : OpenGLContextStorage("Host")
  {
    Record();
  }

public:
  void Record();
  void RecordTextureBindingFromHost();
  void ConfigureFramebuffer();
  void RestoreFramebuffer();
};

class OpenGLNamesStorage : public std::map<GLuint, bool>
{
public:
  OpenGLNamesStorage()
      : std::map<GLuint, bool>()
  {
  }
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
  OpenGLAppContextStorage(std::string name);
  OpenGLAppContextStorage(std::string name, OpenGLAppContextStorage *from);

public:
  void RecordViewport(int x, int y, int w, int h);
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

public:
  gles::GLObjectManager &ObjectManagerRef()
  {
    return *m_GLObjectManager;
  }

private:
  bool m_Dirty = false;
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
