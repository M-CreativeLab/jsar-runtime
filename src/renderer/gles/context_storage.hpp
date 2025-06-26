#pragma once

#include <string>
#include <map>
#include <memory>
#include <common/viewport.hpp>

#include "./common.hpp"
#include "./object_manager.hpp"

class OpenGLTextureBinding
{
public:
  OpenGLTextureBinding(GLenum target, GLuint texture)
      : target_(target)
      , texture_(texture)
  {
  }
  OpenGLTextureBinding(OpenGLTextureBinding &) = default;

  inline void reset(GLenum target, GLuint texture)
  {
    target_ = target;
    texture_ = texture;
  }
  inline GLenum target()
  {
    return target_;
  }
  inline GLint texture()
  {
    return texture_;
  }

private:
  GLenum target_;
  GLuint texture_;
};

class OpenGLBlendingFunc
{
public:
  OpenGLBlendingFunc()
      : src_(GL_ONE)
      , dst_(GL_ZERO)
      , is_separate_(false)
  {
  }
  OpenGLBlendingFunc(OpenGLBlendingFunc *from)
      : src_(from->src_)
      , dst_(from->dst_)
      , src_alpha_(from->src_alpha_)
      , dst_alpha_(from->dst_alpha_)
  {
  }

public:
  void setToCurrentContext()
  {
    if (is_separate_)
      glBlendFuncSeparate(src_, dst_, src_alpha_, dst_alpha_);
    else
      glBlendFunc(src_, dst_);
  }

  inline bool isSeparate()
  {
    return is_separate_;
  }
  inline GLenum src()
  {
    return src_;
  }
  inline GLenum dst()
  {
    return dst_;
  }
  inline GLenum srcRGB()
  {
    return src_;
  }
  inline GLenum dstRGB()
  {
    return dst_;
  }
  inline GLenum srcAlpha()
  {
    return src_alpha_;
  }
  inline GLenum dstAlpha()
  {
    return dst_alpha_;
  }
  inline void reset(GLenum src, GLenum dst)
  {
    src_ = src;
    dst_ = dst;
    is_separate_ = false;
  }
  inline void reset(GLenum srcRgb, GLenum dstRgb, GLenum srcAlpha, GLenum dstAlpha)
  {
    src_ = srcRgb;
    dst_ = dstRgb;
    src_alpha_ = srcAlpha;
    dst_alpha_ = dstAlpha;
    is_separate_ = true;
  }
  void print()
  {
    DEBUG(DEBUG_TAG, "OpenGLBlendingFunc");
    DEBUG(DEBUG_TAG, "  Separate: %s", is_separate_ ? "Yes" : "No");
    if (is_separate_)
    {
      DEBUG(DEBUG_TAG, "  SrcRGB: %d", src_);
      DEBUG(DEBUG_TAG, "  DstRGB: %d", dst_);
      DEBUG(DEBUG_TAG, "  SrcAlpha: %d", src_alpha_);
      DEBUG(DEBUG_TAG, "  DstAlpha: %d", dst_alpha_);
    }
    else
    {
      DEBUG(DEBUG_TAG, "  SrcRGB: %d", src_);
      DEBUG(DEBUG_TAG, "  DstRGB: %d", dst_);
    }

    DEBUG(DEBUG_TAG, "  Current OpenGL states:");
    GLint blend_dst_alpha;
    glGetIntegerv(GL_BLEND_DST_ALPHA, &blend_dst_alpha);
    GLint blend_dst_rgb;
    glGetIntegerv(GL_BLEND_DST_RGB, &blend_dst_rgb);
    DEBUG(DEBUG_TAG, "  DstAlpha=%s", gles::glBlendFuncToString(blend_dst_alpha).c_str());
    DEBUG(DEBUG_TAG, "  DstRGB=%s", gles::glBlendFuncToString(blend_dst_rgb).c_str());

    GLint blend_src_alpha;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &blend_src_alpha);
    GLint blend_src_rgb;
    glGetIntegerv(GL_BLEND_SRC_RGB, &blend_src_rgb);
    DEBUG(DEBUG_TAG, "  SrcAlpha=%s", gles::glBlendFuncToString(blend_src_alpha).c_str());
    DEBUG(DEBUG_TAG, "  SrcRGB=%s", gles::glBlendFuncToString(blend_src_rgb).c_str());
  }

private:
  bool is_separate_ = false;
  GLenum src_;
  GLenum dst_;
  GLenum src_alpha_;
  GLenum dst_alpha_;
};

/**
 * The `ContextGLStorage` class is used to store the OpenGL or OpenGL ES context state, and it can be used to restore
 * the context state.
 */
class ContextGLStorage
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
  ContextGLStorage(std::string name)
      : name_(name)
  {
    glGetBooleanv(GL_CULL_FACE, &m_CullFaceEnabled);
    glGetBooleanv(GL_DEPTH_TEST, &m_DepthTestEnabled);
  }
  ContextGLStorage(std::string name, ContextGLStorage *from)
      : name_(name)
  {
    // Viewport
    viewport_[0] = from->viewport_[0];
    viewport_[1] = from->viewport_[1];
    viewport_[2] = from->viewport_[2];
    viewport_[3] = from->viewport_[3];

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
  ~ContextGLStorage()
  {
  }

  void onViewportChanged(int x, int y, int w, int h);
  void onCapabilityEnabled(GLenum cap, bool enabled);
  void onSetCullFace(GLenum mode);
  void onSetFrontFace(GLenum mode);
  void onSetColorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a);
  void onSetDepthMask(GLboolean enabled);
  void onSetBlendFunc(GLenum sfactor, GLenum dfactor);
  void onSetBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
  void onSetStencilMask(GLenum face, GLuint mask);
  void onSetStencilFunc(GLenum face, GLenum func, GLint ref, GLuint mask);
  void onSetStencilOp(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);

  const char *name()
  {
    return name_.c_str();
  }
  TrViewport viewport()
  {
    return TrViewport(viewport_[2], viewport_[3], viewport_[0], viewport_[1]);
  }
  GLint program()
  {
    return m_ProgramId;
  }
  GLint arrayBuffer()
  {
    return m_ArrayBufferId;
  }
  GLint elementArrayBuffer()
  {
    return m_ElementArrayBufferId;
  }
  GLint framebuffer()
  {
    return m_FramebufferId;
  }
  GLint renderbuffer()
  {
    return m_RenderbufferId;
  }
  GLint vertexArrayObject()
  {
    return m_VertexArrayObjectId;
  }
  GLenum activeTextureUnit()
  {
    return m_LastActiveTextureUnit;
  }

  void resetProgram(int programToReset);
  void restore();
  void print();
  void clearTextureBindings();

protected:
  std::string name_;
  GLint viewport_[4] = {-1, -1, -1, -1};
  bool is_force_changed_ = false;

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

#include "./context_host.hpp"
#include "./context_app.hpp"
