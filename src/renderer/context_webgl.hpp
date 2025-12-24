#pragma once

#include <array>
#include <cassert>
#include <functional>
#include <type_traits>
#include <variant>
#include <vector>
#include <unordered_map>
#include <cmath>

#include <common/utility.hpp>
#include <common/command_buffers/base.hpp>
#include <common/command_buffers/webgl_constants.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>

namespace renderer
{
  class TrContentRenderer;
  class TrRenderResource;
  class TrContextWebGL;
  class TrRenderPass;

  namespace details
  {
    class ObjectTargetBase
    {
    public:
      ObjectTargetBase(WebGLenum target)
          : target_(target)
      {
      }

      inline WebGLenum value() const
      {
        return target_;
      }

      inline bool operator==(const ObjectTargetBase &rhs) const
      {
        return target_ == rhs.target_;
      }

      struct HashKey
      {
        size_t operator()(const ObjectTargetBase &t) const noexcept
        {
          return std::hash<WebGLenum>{}(t.value());
        }
      };

    protected:
      WebGLenum target_;
    };

    class BufferTarget final : public ObjectTargetBase
    {
    public:
      enum
      {
        kArrayBuffer = WEBGL_ARRAY_BUFFER,
        kElementArrayBuffer = WEBGL_ELEMENT_ARRAY_BUFFER,
      };

      BufferTarget(WebGLenum target)
          : ObjectTargetBase(target)
      {
        assert(target_ == kArrayBuffer ||
               target_ == kElementArrayBuffer);
      }
    };

    class TextureTarget final : public ObjectTargetBase
    {
    public:
      enum
      {
        k2D = WEBGL_TEXTURE_2D,
        k3D = WEBGL2_TEXTURE_3D,
        k2DArray = WEBGL2_TEXTURE_2D_ARRAY,
      };

      TextureTarget(WebGLenum target)
          : ObjectTargetBase(target)
      {
        assert(target_ == k2D ||
               target_ == k3D ||
               target_ == k2DArray);
      }
    };

    class FramebufferTarget final : public ObjectTargetBase
    {
    public:
      enum
      {
        kFramebuffer = WEBGL_FRAMEBUFFER,
        kReadFramebuffer = WEBGL2_READ_FRAMEBUFFER,
        kDrawFramebuffer = WEBGL2_DRAW_FRAMEBUFFER,
      };

      FramebufferTarget(WebGLenum target)
          : ObjectTargetBase(target)
      {
        assert(target_ == kFramebuffer ||
               target_ == kReadFramebuffer ||
               target_ == kDrawFramebuffer);
      }
    };

    class RenderbufferTarget final : public ObjectTargetBase
    {
    public:
      enum
      {
        kRenderbuffer = WEBGL_RENDERBUFFER,
      };

      RenderbufferTarget(WebGLenum target)
          : ObjectTargetBase(target)
      {
        assert(target_ == kRenderbuffer);
      }
    };

    class ObjectBase
    {
    public:
      ObjectBase() = default;
      ObjectBase(WebGLuint id);
      virtual ~ObjectBase() = default;

      virtual bool isTexture() const;
      virtual bool isBuffer() const;
      virtual bool isFramebuffer() const;
      virtual bool isRenderbuffer() const;
      virtual bool isVertexArrayObject() const;
      virtual std::string toString() const;

      void set(WebGLuint id);
      WebGLuint id;
    };

    struct BindableObject : public ObjectBase
    {
      using ObjectBase::ObjectBase;

    public:
      void setTarget(const ObjectTargetBase &);

      WebGLenum target;
    };

    class Shader final : public ObjectBase
    {
    public:
      Shader(WebGLuint id, WebGLenum type);
      std::string toString() const;

      WebGLenum type;
      std::string source;
    };

    using SingleFloatValue = std::array<WebGLfloat, 1>;
    using TwoFloatValue = std::array<WebGLfloat, 2>;
    using ThreeFloatValue = std::array<WebGLfloat, 3>;
    using FourFloatValue = std::array<WebGLfloat, 4>;
    using FloatValues = std::vector<WebGLfloat>;

    using SingleIntValue = std::array<WebGLint, 1>;
    using TwoIntValue = std::array<WebGLint, 2>;
    using ThreeIntValue = std::array<WebGLint, 3>;
    using FourIntValue = std::array<WebGLint, 4>;
    using IntValues = std::vector<WebGLint>;

    using SingleUintValue = std::array<WebGLuint, 1>;
    using TwoUintValue = std::array<WebGLuint, 2>;
    using ThreeUintValue = std::array<WebGLuint, 3>;
    using FourUintValue = std::array<WebGLuint, 4>;
    using UintValues = std::vector<WebGLuint>;

    using UniformValue = std::variant<
      SingleFloatValue,
      TwoFloatValue,
      ThreeFloatValue,
      FourFloatValue,
      FloatValues,
      SingleIntValue,
      TwoIntValue,
      ThreeIntValue,
      FourIntValue,
      IntValues,
      SingleUintValue,
      TwoUintValue,
      ThreeUintValue,
      FourUintValue,
      UintValues>;
    class Uniforms : public std::unordered_map<WebGLint, UniformValue>
    {
      using std::unordered_map<WebGLint, UniformValue>::unordered_map;

    public:
      void set(WebGLint loc, WebGLfloat v0);
      void set(WebGLint loc, WebGLfloat v0, WebGLfloat v1);
      void set(WebGLint loc, WebGLfloat v0, WebGLfloat v1, WebGLfloat v2);
      void set(WebGLint loc, WebGLfloat v0, WebGLfloat v1, WebGLfloat v2, WebGLfloat v3);
      void set(WebGLint loc, const FloatValues &values);

      void set(WebGLint loc, WebGLint v0);
      void set(WebGLint loc, WebGLint v0, WebGLint v1);
      void set(WebGLint loc, WebGLint v0, WebGLint v1, WebGLint v2);
      void set(WebGLint loc, WebGLint v0, WebGLint v1, WebGLint v2, WebGLint v3);
      void set(WebGLint loc, const IntValues &values);

      void set(WebGLint loc, WebGLuint v0);
      void set(WebGLint loc, WebGLuint v0, WebGLuint v1);
      void set(WebGLint loc, WebGLuint v0, WebGLuint v1, WebGLuint v2);
      void set(WebGLint loc, WebGLuint v0, WebGLuint v1, WebGLuint v2, WebGLuint v3);
      void set(WebGLint loc, const UintValues &values);
    };

    class Program final : public ObjectBase
    {
    public:
      Program(WebGLuint id);

      Ref<Shader> vertexShader;
      Ref<Shader> fragmentShader;
      Uniforms uniforms;
      bool linked = false;
      std::unordered_map<std::string, WebGLuint> attrib_locations;
      std::unordered_map<std::string, WebGLuint> uniform_locations;
    };

    class Texture final : public BindableObject
    {
      using BindableObject::BindableObject;

    public:
      bool isTexture() const override
      {
        return true;
      }
      std::string toString() const override;
      void setSize(WebGLsizei width, WebGLsizei height, WebGLsizei depth = 0);

      int gpu_id = -1;
      WebGLsizei size[3];
      WebGLsizei mipLevels;
      WebGLenum internalformat;
      WebGLenum compressedInternalformat;
      std::unordered_map<WebGLenum, WebGLint> params_i;
      std::unordered_map<WebGLenum, WebGLfloat> params_f;
    };

    class Buffer final : public BindableObject
    {
      using BindableObject::BindableObject;

    public:
      bool isBuffer() const override
      {
        return true;
      }

      int gpu_id = -1;
      WebGLenum usage_hint = 0;
      WebGLsizei size;
    };

    class Renderbuffer final : public BindableObject
    {
      using BindableObject::BindableObject;

    public:
      bool isRenderbuffer() const override
      {
        return true;
      }
    };

    struct FramebufferAttachment
    {
      Ref<Texture> texture;
      Ref<Renderbuffer> renderbuffer;
    };

    class Framebuffer final : public BindableObject
    {
      using BindableObject::BindableObject;

    public:
      bool isFramebuffer() const override
      {
        return true;
      }

      Ref<FramebufferAttachment> colorAttachment;
      Ref<FramebufferAttachment> depthAttachment;
      Ref<FramebufferAttachment> stencilAttachment;
    };

    class VertexArrayObject final : public BindableObject
    {
      using BindableObject::BindableObject;

    public:
      bool isVertexArrayObject() const override
      {
        return true;
      }

      commandbuffers::GPUVertexState vertex_state{};
      std::vector<commandbuffers::GPUVertexBufferLayout> buffer_layouts{};
      std::vector<commandbuffers::GPUVertexAttribute> attributes{};
      std::unordered_map<WebGLuint, bool> attrib_enabled;
      std::unordered_map<WebGLuint, bool> attrib_integer;
      std::unordered_map<WebGLuint, WebGLint> attrib_divisor;
    };

    class Capabilities
    {
    public:
      using Map = std::unordered_map<WebGLenum, WebGLboolean>;

      inline void attach(class TrContextWebGL *owner)
      {
        owner_ = owner;
      }

      inline void enable(WebGLenum cap)
      {
        caps_[cap] = true;
        applyEnable(cap);
      }

      inline void disable(WebGLenum cap)
      {
        caps_[cap] = false;
        applyDisable(cap);
      }

      inline WebGLboolean isEnabled(WebGLenum cap) const
      {
        auto it = caps_.find(cap);
        return it != caps_.end() ? it->second : false;
      }

      // Accessors for mirrored WebGPU states if needed externally
      inline const commandbuffers::GPUPrimitiveState &primitive() const
      {
        return primitive_state_;
      }
      inline const commandbuffers::GPUDepthStencilState &depthStencil() const
      {
        return depth_stencil_state_;
      }
      inline const commandbuffers::GPUMultisampleState &multisample() const
      {
        return multisample_state_;
      }
      inline const commandbuffers::GPUBlendState &blendState() const
      {
        return blend_state_;
      }
      inline const commandbuffers::GPUColorTargetState &colorTarget() const
      {
        return color_target_state_;
      }

      // Explicit apply methods for state changes not gated by a capability
      void applyColorMask();
      void refresh(WebGLenum cap);

    private:
      // Helpers to map WebGL enums to WebGPU
      static inline commandbuffers::GPUBlendFactor MapBlendFactor(WebGLenum f)
      {
        using namespace commandbuffers;
        switch (f)
        {
        case WEBGL_ZERO:
          return GPUBlendFactor::kZero;
        case WEBGL_ONE:
          return GPUBlendFactor::kOne;
        case WEBGL_SRC_COLOR:
          return GPUBlendFactor::kSrc;
        case WEBGL_ONE_MINUS_SRC_COLOR:
          return GPUBlendFactor::kOneMinusSrc;
        case WEBGL_SRC_ALPHA:
          return GPUBlendFactor::kSrcAlpha;
        case WEBGL_ONE_MINUS_SRC_ALPHA:
          return GPUBlendFactor::kOneMinusSrcAlpha;
        case WEBGL_DST_COLOR:
          return GPUBlendFactor::kDst;
        case WEBGL_ONE_MINUS_DST_COLOR:
          return GPUBlendFactor::kOneMinusDst;
        case WEBGL_DST_ALPHA:
          return GPUBlendFactor::kDstAlpha;
        case WEBGL_ONE_MINUS_DST_ALPHA:
          return GPUBlendFactor::kOneMinusDstAlpha;
        case WEBGL_SRC_ALPHA_SATURATE:
          return GPUBlendFactor::kSrcAlphaSaturated;
        case WEBGL_CONSTANT_COLOR:
          return GPUBlendFactor::kConstant;
        case WEBGL_ONE_MINUS_CONSTANT_COLOR:
          return GPUBlendFactor::kOneMinusConstant;
        case WEBGL_CONSTANT_ALPHA:
          return GPUBlendFactor::kConstant;
        case WEBGL_ONE_MINUS_CONSTANT_ALPHA:
          return GPUBlendFactor::kOneMinusConstant;
        default:
          return GPUBlendFactor::kUndefined;
        }
      }

      static inline commandbuffers::GPUBlendOperation MapBlendOp(WebGLenum op)
      {
        using namespace commandbuffers;
        switch (op)
        {
        case WEBGL_FUNC_ADD:
          return GPUBlendOperation::kAdd;
        case WEBGL_FUNC_SUBTRACT:
          return GPUBlendOperation::kSubtract;
        case WEBGL_FUNC_REVERSE_SUBTRACT:
          return GPUBlendOperation::kReverseSubtract;
        default:
          return GPUBlendOperation::kUndefined;
        }
      }

      static inline commandbuffers::GPUCompareFunction MapCompare(WebGLenum func)
      {
        using namespace commandbuffers;
        switch (func)
        {
        case WEBGL_NEVER:
          return GPUCompareFunction::kNever;
        case WEBGL_LESS:
          return GPUCompareFunction::kLess;
        case WEBGL_EQUAL:
          return GPUCompareFunction::kEqual;
        case WEBGL_LEQUAL:
          return GPUCompareFunction::kLessEqual;
        case WEBGL_GREATER:
          return GPUCompareFunction::kGreater;
        case WEBGL_NOTEQUAL:
          return GPUCompareFunction::kNotEqual;
        case WEBGL_GEQUAL:
          return GPUCompareFunction::kGreaterEqual;
        case WEBGL_ALWAYS:
          return GPUCompareFunction::kAlways;
        default:
          return GPUCompareFunction::kUndefined;
        }
      }

      static inline commandbuffers::GPUCullMode MapCull(WebGLenum mode)
      {
        using namespace commandbuffers;
        switch (mode)
        {
        case WEBGL_FRONT:
          return GPUCullMode::kFront;
        case WEBGL_BACK:
          return GPUCullMode::kBack;
        case WEBGL_FRONT_AND_BACK:
          return GPUCullMode::kBack;
        default:
          return GPUCullMode::kNone;
        }
      }

      static inline commandbuffers::GPUFrontFace MapFrontFace(WebGLenum mode)
      {
        using namespace commandbuffers;
        switch (mode)
        {
        case WEBGL_CW:
          return GPUFrontFace::kCW;
        case WEBGL_CCW:
          return GPUFrontFace::kCCW;
        default:
          return GPUFrontFace::kUndefined;
        }
      }

      static inline commandbuffers::GPUStencilOperation MapStencilOp(WebGLenum op)
      {
        using namespace commandbuffers;
        switch (op)
        {
        case WEBGL_KEEP:
          return GPUStencilOperation::kKeep;
        case WEBGL_ZERO:
          return GPUStencilOperation::kZero;
        case WEBGL_REPLACE:
          return GPUStencilOperation::kReplace;
        case WEBGL_INCR:
          return GPUStencilOperation::kIncrementClamp;
        case WEBGL_INCR_WRAP:
          return GPUStencilOperation::kIncrementWrap;
        case WEBGL_DECR:
          return GPUStencilOperation::kDecrementClamp;
        case WEBGL_DECR_WRAP:
          return GPUStencilOperation::kDecrementWrap;
        case WEBGL_INVERT:
          return GPUStencilOperation::kInvert;
        default:
          return GPUStencilOperation::kUndefined;
        }
      }

      void applyDisable(WebGLenum cap);
      void applyEnable(WebGLenum cap);

      Map caps_;
      class TrContextWebGL *owner_ = nullptr;

      // WebGPU pipeline-related states mirrored from WebGL
      commandbuffers::GPUPrimitiveState primitive_state_{};
      commandbuffers::GPUDepthStencilState depth_stencil_state_{};
      commandbuffers::GPUMultisampleState multisample_state_{};
      commandbuffers::GPUBlendState blend_state_{};
      commandbuffers::GPUColorTargetState color_target_state_{};
    };
  }

  class TrContextWebGL
  {
  public:
    TrContextWebGL(Ref<TrContentRenderer> content_renderer);
    ~TrContextWebGL();

    void receiveIncomingCall(const commandbuffers::TrCommandBufferRequest &);

  private:
    friend class details::Capabilities;
    /**
     * @brief Convert the request to the given type.
     * 
     * @tparam T The type to convert.
     * @param req The request to convert.
     * @return const T& The converted request.
     */
    template <typename T>
    static const T &To(const commandbuffers::TrCommandBufferRequest &req)
    {
      return dynamic_cast<const T &>(req);
    }

    // Textures
    void glActiveTexture(WebGLenum texture);
    void glBindTexture(WebGLenum target, WebGLuint texture);
    void glCompressedTexImage2D(WebGLenum target,
                                WebGLint level,
                                WebGLenum internalformat,
                                WebGLsizei width,
                                WebGLsizei height,
                                WebGLsizei border,
                                WebGLsizei imageSize,
                                const WebGLvoid *data);
    void glCompressedTexImage3D(WebGLenum target,
                                WebGLint level,
                                WebGLenum internalformat,
                                WebGLsizei width,
                                WebGLsizei height,
                                WebGLsizei depth,
                                WebGLsizei border,
                                WebGLsizei imageSize,
                                const WebGLvoid *data);
    void glCompressedTexSubImage2D(WebGLenum target,
                                   WebGLint level,
                                   WebGLint xoffset,
                                   WebGLint yoffset,
                                   WebGLsizei width,
                                   WebGLsizei height,
                                   WebGLenum format,
                                   WebGLsizei imageSize,
                                   const WebGLvoid *data);
    void glCompressedTexSubImage3D(WebGLenum target,
                                   WebGLint level,
                                   WebGLint xoffset,
                                   WebGLint yoffset,
                                   WebGLint zoffset,
                                   WebGLsizei width,
                                   WebGLsizei height,
                                   WebGLsizei depth,
                                   WebGLenum format,
                                   WebGLsizei imageSize,
                                   const WebGLvoid *data);
    void glCopyTexImage2D(WebGLenum target,
                          WebGLenum internalformat,
                          WebGLint level,
                          WebGLint x,
                          WebGLint y,
                          WebGLsizei width,
                          WebGLsizei height,
                          WebGLint border);
    void glCopyTexSubImage2D(WebGLenum target,
                             WebGLint level,
                             WebGLint xoffset,
                             WebGLint yoffset,
                             WebGLint x,
                             WebGLint y,
                             WebGLsizei width,
                             WebGLsizei height);
    void glDeleteTextures(WebGLsizei n, const WebGLuint *textures);
    void glGenTextures(WebGLsizei n, WebGLuint *textures);
    void glGetTexParameter(WebGLenum target, WebGLenum pname, WebGLint *params);
    WebGLboolean glIsTexture(WebGLuint texture);
    void glTexImage2D(WebGLenum target,
                      WebGLint level,
                      WebGLenum internalformat,
                      WebGLsizei width,
                      WebGLsizei height,
                      WebGLsizei border,
                      WebGLenum format,
                      WebGLenum type,
                      const WebGLvoid *data);
    void glTexImage3D(WebGLenum target,
                      WebGLint level,
                      WebGLenum internalformat,
                      WebGLsizei width,
                      WebGLsizei height,
                      WebGLsizei depth,
                      WebGLsizei border,
                      WebGLenum format,
                      WebGLenum type,
                      const WebGLvoid *data);
    void glTexParameterf(WebGLenum target, WebGLenum pname, WebGLfloat param);
    void glTexParameteri(WebGLenum target, WebGLenum pname, WebGLint param);
    void glTexParameterfv(WebGLenum target, WebGLenum pname, const WebGLfloat *params);
    void glTexParameteriv(WebGLenum target, WebGLenum pname, const WebGLint *params);
    void glTexStorage2D(WebGLenum target,
                        WebGLint levels,
                        WebGLenum internalformat,
                        WebGLsizei width,
                        WebGLsizei height);
    void glTexStorage3D(WebGLenum target,
                        WebGLint levels,
                        WebGLenum internalformat,
                        WebGLsizei width,
                        WebGLsizei height,
                        WebGLsizei depth);
    void glTexSubImage2D(WebGLenum target,
                         WebGLint level,
                         WebGLint xoffset,
                         WebGLint yoffset,
                         WebGLsizei width,
                         WebGLsizei height,
                         WebGLenum format,
                         WebGLenum type,
                         const WebGLvoid *data);
    void glTexSubImage3D(WebGLenum target,
                         WebGLint level,
                         WebGLint xoffset,
                         WebGLint yoffset,
                         WebGLint zoffset,
                         WebGLsizei width,
                         WebGLsizei height,
                         WebGLsizei depth,
                         WebGLenum format,
                         WebGLenum type,
                         const WebGLvoid *data);

    // Rendering
    void glClear(WebGLbitfield mask);
    void glClearBufferiv(WebGLenum buffer, WebGLint drawbuffer, const WebGLint *value);
    void glClearBufferuiv(WebGLenum buffer, WebGLint drawbuffer, const WebGLuint *value);
    void glClearBufferfv(WebGLenum buffer, WebGLint drawbuffer, const WebGLfloat *value);
    void glClearBufferfi(WebGLenum buffer, WebGLint drawbuffer, WebGLfloat depth, WebGLint stencil);
    void glClearColor(WebGLfloat red, WebGLfloat green, WebGLfloat blue, WebGLfloat alpha);
    void glClearDepthf(WebGLfloat depth);
    void glClearStencil(WebGLint s);
    void glFinish();
    void glFlush();
    void glReadBuffer(WebGLenum buffer);
    void glReadPixels(WebGLint x, WebGLint y, WebGLsizei width, WebGLsizei height, WebGLenum format, WebGLenum type, WebGLvoid *pixels);

    // Frame Buffers
    void glBindFramebuffer(WebGLenum target, WebGLuint framebuffer);
    void glBindRenderbuffer(WebGLenum target, WebGLuint renderbuffer);
    void glBlitFramebuffer(WebGLint srcX0,
                           WebGLint srcY0,
                           WebGLint srcX1,
                           WebGLint srcY1,
                           WebGLint dstX0,
                           WebGLint dstY0,
                           WebGLint dstX1,
                           WebGLint dstY1,
                           WebGLbitfield mask,
                           WebGLenum filter);
    void glCheckFramebufferStatus(WebGLenum target);
    void glDeleteFramebuffers(WebGLsizei n, const WebGLuint *framebuffers);
    void glDeleteRenderbuffers(WebGLsizei n, const WebGLuint *renderbuffers);
    void glDrawBuffers(WebGLsizei n, const WebGLenum *buffers);
    void glFramebufferRenderbuffer(WebGLenum target, WebGLenum attachment, WebGLenum renderbuffertarget, WebGLuint renderbuffer);
    void glFramebufferTexture2D(WebGLenum target, WebGLenum attachment, WebGLenum textarget, WebGLuint texture, WebGLint level);
    void glFramebufferTextureLayer(WebGLenum target, WebGLenum attachment, WebGLuint texture, WebGLint level, WebGLint layer);
    void glGenFramebuffers(WebGLsizei n, WebGLuint *framebuffers);
    void glGenRenderbuffers(WebGLsizei n, WebGLuint *renderbuffers);
    void glGenerateMipmap(WebGLenum target);
    void glGetFramebufferAttachmentParameteriv(WebGLenum target, WebGLenum attachment, WebGLenum pname, WebGLint *params);
    void glGetRenderbufferParameteriv(WebGLenum target, WebGLenum pname, WebGLint *params);
    void glInvalidateFramebuffer(WebGLenum target, WebGLsizei n, const WebGLenum *attachments);
    void glInvalidateSubFramebuffer(WebGLenum target,
                                    WebGLsizei n,
                                    const WebGLenum *attachments,
                                    WebGLint x,
                                    WebGLint y,
                                    WebGLsizei width,
                                    WebGLsizei height);
    WebGLboolean glIsFramebuffer(WebGLuint framebuffer);
    WebGLboolean glIsRenderbuffer(WebGLuint renderbuffer);
    void glRenderbufferStorage(WebGLenum target, WebGLenum internalformat, WebGLsizei width, WebGLsizei height);
    void glRenderbufferStorageMultisample(WebGLenum target,
                                          WebGLsizei samples,
                                          WebGLenum internalformat,
                                          WebGLsizei width,
                                          WebGLsizei height);

    // Shaders
    void glBindAttribLocation(WebGLuint program, WebGLuint index, const WebGLchar *name);
    void glCompileShader(WebGLuint shader);
    WebGLuint glCreateProgram();
    WebGLuint glCreateShader(WebGLenum type);
    void glDeleteProgram(WebGLuint program);
    void glDeleteShader(WebGLuint shader);
    void glAttachShader(WebGLuint program, WebGLuint shader);
    void glDetachShader(WebGLuint program, WebGLuint shader);
    void glGetActiveAttrib(WebGLuint program,
                           WebGLuint index,
                           WebGLsizei maxLength,
                           WebGLsizei *length,
                           WebGLint *size,
                           WebGLenum *type,
                           WebGLchar *name);
    void glGetActiveUniform(WebGLuint program,
                            WebGLuint index,
                            WebGLsizei maxLength,
                            WebGLsizei *length,
                            WebGLint *size,
                            WebGLenum *type,
                            WebGLchar *name);
    void glGetActiveUniformBlockName(WebGLuint program,
                                     WebGLuint index,
                                     WebGLsizei maxLength,
                                     WebGLsizei *length,
                                     WebGLchar *name);
    void glGetActiveUniformBlockiv(WebGLuint program,
                                   WebGLuint index,
                                   WebGLenum pname,
                                   WebGLint *params);
    void glGetActiveUniformsiv(WebGLuint program,
                               WebGLsizei count,
                               const WebGLuint *uniforms,
                               WebGLenum pname,
                               WebGLint *params);
    void glGetAttachedShaders(WebGLuint program, WebGLsizei maxCount, WebGLsizei *count, WebGLuint *shaders);
    void glGetAttribLocation(WebGLuint program, const WebGLchar *name);
    void glGetFragDataLocation(WebGLuint program, const WebGLchar *name);
    void glGetProgramBinary(WebGLuint program,
                            WebGLsizei maxLength,
                            WebGLsizei *length,
                            WebGLenum binaryFormat,
                            WebGLsizei *binaryLength,
                            WebGLbyte *binary);
    void glGetProgramInfoLog(WebGLuint program, WebGLsizei maxLength, WebGLsizei *length, WebGLchar *infoLog);
    void glGetProgramiv(WebGLuint program, WebGLenum pname, WebGLint *params);
    void glGetShaderInfoLog(WebGLuint shader, WebGLsizei maxLength, WebGLsizei *length, WebGLchar *infoLog);
    void glGetShaderPrecisionFormat(WebGLenum shadertype, WebGLenum precisiontype, WebGLint *range, WebGLint *precision);
    void glGetShaderSource(WebGLuint shader, WebGLsizei maxLength, WebGLsizei *length, WebGLchar *source);
    void glGetShaderiv(WebGLuint shader, WebGLenum pname, WebGLint *params);
    void glGetUniformfv(WebGLuint program, WebGLuint location, WebGLsizei count, WebGLfloat *params);
    void glGetUniformiv(WebGLuint program, WebGLuint location, WebGLsizei count, WebGLint *params);
    void glGetUniformuiv(WebGLuint program, WebGLuint location, WebGLsizei count, WebGLuint *params);
    void glGetUniformBlockIndex(WebGLuint program, const WebGLchar *name);
    void glGetUniformIndices(WebGLuint program, WebGLsizei count, const WebGLchar **names, WebGLuint *indices);
    void glGetUniformLocation(WebGLuint program, const WebGLchar *name);
    WebGLboolean glIsProgram(WebGLuint program);
    WebGLboolean glIsShader(WebGLuint shader);
    void glLinkProgram(WebGLuint program);
    void glProgramBinary(WebGLuint program, WebGLenum binaryFormat, const WebGLbyte *binary, WebGLsizei binaryLength);
    void glProgramParameteri(WebGLuint program, WebGLenum pname, WebGLint param);
    void glReleaseShaderCompiler();
    void glShaderBinary(WebGLuint shader, WebGLenum binaryFormat, const WebGLbyte *binary, WebGLsizei binaryLength);
    void glShaderSource(WebGLuint shader, WebGLsizei count, const WebGLchar **string, const WebGLint *length);
    void glUniform1f(WebGLuint location, WebGLfloat v0);
    void glUniform2f(WebGLuint location, WebGLfloat v0, WebGLfloat v1);
    void glUniform3f(WebGLuint location, WebGLfloat v0, WebGLfloat v1, WebGLfloat v2);
    void glUniform4f(WebGLuint location, WebGLfloat v0, WebGLfloat v1, WebGLfloat v2, WebGLfloat v3);
    void glUniform1i(WebGLuint location, WebGLint v0);
    void glUniform2i(WebGLuint location, WebGLint v0, WebGLint v1);
    void glUniform3i(WebGLuint location, WebGLint v0, WebGLint v1, WebGLint v2);
    void glUniform4i(WebGLuint location, WebGLint v0, WebGLint v1, WebGLint v2, WebGLint v3);
    void glUniform1ui(WebGLuint location, WebGLuint v0);
    void glUniform2ui(WebGLuint location, WebGLuint v0, WebGLuint v1);
    void glUniform3ui(WebGLuint location, WebGLuint v0, WebGLuint v1, WebGLuint v2);
    void glUniform4ui(WebGLuint location, WebGLuint v0, WebGLuint v1, WebGLuint v2, WebGLuint v3);
    void glUniform1fv(WebGLuint location, WebGLsizei count, const WebGLfloat *value);
    void glUniform2fv(WebGLuint location, WebGLsizei count, const WebGLfloat *value);
    void glUniform3fv(WebGLuint location, WebGLsizei count, const WebGLfloat *value);
    void glUniform4fv(WebGLuint location, WebGLsizei count, const WebGLfloat *value);
    void glUniform1iv(WebGLuint location, WebGLsizei count, const WebGLint *value);
    void glUniform2iv(WebGLuint location, WebGLsizei count, const WebGLint *value);
    void glUniform3iv(WebGLuint location, WebGLsizei count, const WebGLint *value);
    void glUniform4iv(WebGLuint location, WebGLsizei count, const WebGLint *value);
    void glUniform1uiv(WebGLuint location, WebGLsizei count, const WebGLuint *value);
    void glUniform2uiv(WebGLuint location, WebGLsizei count, const WebGLuint *value);
    void glUniform3uiv(WebGLuint location, WebGLsizei count, const WebGLuint *value);
    void glUniform4uiv(WebGLuint location, WebGLsizei count, const WebGLuint *value);
    void glUniformMatrix2fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value);
    void glUniformMatrix3fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value);
    void glUniformMatrix4fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value);
    void glUniformMatrix2x3fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value);
    void glUniformMatrix3x2fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value);
    void glUniformMatrix2x4fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value);
    void glUniformMatrix4x2fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value);
    void glUniformMatrix3x4fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value);
    void glUniformMatrix4x3fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value);
    void glUniformBlockBinding(WebGLuint program, WebGLuint uniformBlockIndex, WebGLuint bindingPoint);
    void glUseProgram(WebGLuint program);
    void glValidateProgram(WebGLuint program);

    // Buffer Objects
    void glBindBuffer(WebGLenum target, WebGLuint buffer);
    void glBindBufferBase(WebGLenum target, WebGLuint bindingPoint, WebGLuint buffer);
    void glBindBufferRange(WebGLenum target, WebGLuint bindingPoint, WebGLuint buffer, WebGLintptr offset, WebGLsizeiptr size);
    void glBufferData(WebGLenum target, WebGLsizeiptr size, const WebGLvoid *data, WebGLenum usage);
    void glBufferSubData(WebGLenum target, WebGLintptr offset, WebGLsizeiptr size, const WebGLvoid *data);
    void glCopyBufferSubData(WebGLenum readTarget,
                             WebGLenum writeTarget,
                             WebGLintptr readOffset,
                             WebGLintptr writeOffset,
                             WebGLsizeiptr size);
    void glDeleteBuffers(WebGLsizei count, const WebGLuint *buffers);
    void glDisableVertexAttribArray(WebGLuint index);
    void glDrawArrays(WebGLenum mode, WebGLint first, WebGLsizei count);
    void glDrawArraysInstanced(WebGLenum mode, WebGLint first, WebGLsizei count, WebGLsizei instanceCount);
    void glDrawElements(WebGLenum mode, WebGLsizei count, WebGLenum type, const WebGLvoid *indices);
    void glDrawElementsInstanced(WebGLenum mode, WebGLsizei count, WebGLenum type, const WebGLvoid *indices, WebGLsizei instanceCount);
    void glDrawRangeElements(WebGLenum mode, WebGLuint start, WebGLuint end, WebGLsizei count, WebGLenum type, const WebGLvoid *indices);
    void glEnableVertexAttribArray(WebGLuint index);
    void glFlushMappedBufferRange(WebGLenum target, WebGLintptr offset, WebGLsizeiptr size);
    void glGenBuffers(WebGLsizei n, WebGLuint *buffers);
    void glGetBufferParameter(WebGLenum target, WebGLenum pname, WebGLint *params);
    void glGetBufferParameteriv(WebGLenum target, WebGLenum pname, WebGLint *params);
    void glGetBufferPointerv(WebGLenum target, WebGLenum pname, WebGLvoid **params);
    void glGetVertexAttrib(WebGLuint index, WebGLenum pname, WebGLint *params);
    void glGetVertexAttribPointerv(WebGLuint index, WebGLenum pname, WebGLvoid **pointer);
    WebGLboolean glIsBuffer(WebGLuint buffer);
    void glMapBufferRange(WebGLenum target, WebGLintptr offset, WebGLsizeiptr length, WebGLbitfield access);
    void glUnmapBuffer(WebGLenum target);
    void glVertexAttrib1f(WebGLuint index, WebGLfloat x);
    void glVertexAttrib2f(WebGLuint index, WebGLfloat x, WebGLfloat y);
    void glVertexAttrib3f(WebGLuint index, WebGLfloat x, WebGLfloat y, WebGLfloat z);
    void glVertexAttrib4f(WebGLuint index, WebGLfloat x, WebGLfloat y, WebGLfloat z, WebGLfloat w);
    void glVertexAttrib1fv(WebGLuint index, const WebGLfloat *v);
    void glVertexAttrib2fv(WebGLuint index, const WebGLfloat *v);
    void glVertexAttrib3fv(WebGLuint index, const WebGLfloat *v);
    void glVertexAttrib4fv(WebGLuint index, const WebGLfloat *v);
    void glVertexAttribDivisor(WebGLuint index, WebGLuint divisor);
    void glVertexAttribPointer(WebGLuint index,
                               WebGLint size,
                               WebGLenum type,
                               WebGLboolean normalized,
                               WebGLsizei stride,
                               WebGLintptr offset);
    void glVertexAttribIPointer(WebGLuint index,
                                WebGLint size,
                                WebGLenum type,
                                WebGLsizei stride,
                                WebGLintptr offset);

    // State Management
    void glBlendColor(WebGLfloat red, WebGLfloat green, WebGLfloat blue, WebGLfloat alpha);
    void glBlendEquation(WebGLenum mode);
    void glBlendEquationSeparate(WebGLenum modeRGB, WebGLenum modeAlpha);
    void glBlendFunc(WebGLenum sfactor, WebGLenum dfactor);
    void glBlendFuncSeparate(WebGLenum srcRGB, WebGLenum dstRGB, WebGLenum srcAlpha, WebGLenum dstAlpha);
    void glColorMask(WebGLboolean red, WebGLboolean green, WebGLboolean blue, WebGLboolean alpha);
    void glCullFace(WebGLenum mode);
    void glDepthFunc(WebGLenum func);
    void glDepthMask(WebGLboolean flag);
    void glDepthRangef(WebGLfloat near, WebGLfloat far);
    void glDisable(WebGLenum cap);
    void glEnable(WebGLenum cap);
    void glFrontFace(WebGLenum mode);
    void glGet(WebGLenum pname, WebGLint *params);
    WebGLenum glGetError();
    void glHint(WebGLenum target, WebGLenum mode);
    WebGLboolean glIsEnabled(WebGLenum cap);
    void glLineWidth(WebGLfloat width);
    void glPixelStorei(WebGLenum pname, WebGLint param);
    void glPolygonOffset(WebGLfloat factor, WebGLfloat units);
    void glSampleCoverage(WebGLfloat value, WebGLboolean invert);
    void glScissor(WebGLint x, WebGLint y, WebGLsizei width, WebGLsizei height);
    void glStencilFunc(WebGLenum func, WebGLint ref, WebGLuint mask);
    void glStencilFuncSeparate(WebGLenum face, WebGLenum func, WebGLint ref, WebGLuint mask);
    void glStencilMask(WebGLuint mask);
    void glStencilMaskSeparate(WebGLenum face, WebGLuint mask);
    void glStencilOp(WebGLenum fail, WebGLenum zfail, WebGLenum zpass);
    void glStencilOpSeparate(WebGLenum face, WebGLenum fail, WebGLenum zfail, WebGLenum zpass);
    void glViewport(WebGLint x, WebGLint y, WebGLsizei width, WebGLsizei height);

    // Transform Feedback
    void glBeginTransformFeedback(WebGLenum primitiveMode);
    void glBindTransformFeedback(WebGLenum target, WebGLuint transformFeedback);
    void glDeleteTransformFeedbacks(WebGLsizei n, const WebGLuint *transformFeedbacks);
    void glEndTransformFeedback();
    void glGenTransformFeedbacks(WebGLsizei n, WebGLuint *transformFeedbacks);
    void glGetTransformFeedbackVarying(WebGLuint program,
                                       WebGLuint index,
                                       WebGLsizei bufSize,
                                       WebGLsizei *length,
                                       WebGLsizei *size,
                                       WebGLenum *type,
                                       WebGLchar *name);
    void glIsTransformFeedback(WebGLuint transformFeedback);
    void glPauseTransformFeedback();
    void glResumeTransformFeedback();
    void glTransformFeedbackVaryings(WebGLuint program,
                                     WebGLsizei count,
                                     const WebGLchar **varyings,
                                     WebGLenum bufferMode);

    // Utility
    void glGetInternalformativ(WebGLenum target, WebGLenum internalformat, WebGLsizei propCount, WebGLenum *props, WebGLint *params);
    void glGetString(WebGLenum pname, WebGLchar *params);
    void glGetStringi(WebGLenum pname, WebGLuint index);

    // Queries
    void glBeginQuery(WebGLenum target, WebGLuint id);
    void glDeleteQueries(WebGLsizei n, const WebGLuint *ids);
    void glEndQuery(WebGLenum target);
    void glGenQueries(WebGLsizei n, WebGLuint *ids);
    void glGetQueryObjectuiv(WebGLuint id, WebGLenum pname, WebGLuint *params);
    void glGetQueryiv(WebGLenum target, WebGLenum pname, WebGLint *params);
    void glIsQuery(WebGLuint id);

    // Syncing
    void glClientWaitSync(WebGLsync sync, WebGLbitfield flags, WebGLuint64 timeout);
    void glDeleteSync(WebGLsync sync);
    WebGLsync glFenceSync(WebGLenum condition, WebGLbitfield flags);
    void glGetSynciv(WebGLsync sync, WebGLenum pname, WebGLsizei bufSize, WebGLsizei *length, WebGLint *values);
    WebGLboolean glIsSync(WebGLsync sync);
    void glWaitSync(WebGLsync sync, WebGLbitfield flags, WebGLuint64 timeout);

    // Vertex Array Objects
    void glBindVertexArray(WebGLuint array);
    void glDeleteVertexArrays(WebGLsizei n, const WebGLuint *arrays);
    void glGenVertexArrays(WebGLsizei n, WebGLuint *arrays);
    WebGLboolean glIsVertexArray(WebGLuint array);

    // Samplers
    void glBindSampler(WebGLuint unit, WebGLuint sampler);
    void glDeleteSamplers(WebGLsizei n, const WebGLuint *samplers);
    void glGenSamplers(WebGLsizei n, WebGLuint *samplers);
    void glGetSamplerParameter(WebGLuint sampler, WebGLenum pname, WebGLint *params);
    void glIsSampler(WebGLuint sampler);
    void glSamplerParameter(WebGLuint sampler, WebGLenum pname, WebGLint param);

    // Internal Utilities
    template <typename ObjectType>
      requires std::is_base_of_v<details::ObjectBase, ObjectType>
    void glGenTypedObjects(std::vector<Ref<ObjectType>> &source_list,
                           WebGLsizei n,
                           WebGLuint *generated_list)
    {
      if (n <= 0 || generated_list == nullptr)
        return;

      size_t old_size = source_list.size();
      source_list.resize(old_size + static_cast<size_t>(n));

      for (size_t i = 0; i < n; i++)
      {
        Ref<ObjectType> object = AcquireRef(new ObjectType());
        source_list[old_size + i] = object;
        generated_list[i] = object->id;
      }
    }

    /**
     * A convenience function to create a WebGL object such as buffer, texture, framebuffer, renderbuffer in this context implementation.
     * 
     * @tparam ReqType 
     * @param source_list A list of WebGL objects to store the created object.
     * @param req A request to create a WebGL object.
     */
    template <typename ObjectType, typename ReqType>
      requires std::is_base_of_v<details::ObjectBase, ObjectType>
    void glCreateTypedObject(std::vector<Ref<ObjectType>> &source_list,
                             const commandbuffers::TrCommandBufferRequest &req)
    {
      const auto &typed_req = To<ReqType>(req);
      size_t size_before = source_list.size();
      {
        WebGLint obj;
        glGenTypedObjects<ObjectType>(source_list, 1, (WebGLuint *)&obj);
        assert(obj == 0 && "object must be the initial object");
      }

      Ref<ObjectType> created_object = source_list[size_before];
      assert(created_object != nullptr && "object must be created");
      created_object->set(req.id);
      debugPrint();
    }

    template <typename... Args>
      requires((sizeof...(Args) > 0) && (((std::is_same_v<Args, WebGLfloat>)&&...) ||
                                         ((std::is_same_v<Args, WebGLint>)&&...) ||
                                         ((std::is_same_v<Args, WebGLuint>)&&...)))
    void glSetUniform(WebGLuint location, Args... args)
    {
      if (current_program_ == nullptr) [[unlikely]]
      {
        last_error_ = WEBGL_INVALID_OPERATION;
        return;
      }
      current_program_->uniforms.set(location, args...);
    }

    // Debug Utilities
    template <typename ObjectType>
      requires std::is_base_of_v<details::ObjectBase, ObjectType>
    void debugPrintObjects(const std::string &label,
                           const std::vector<Ref<ObjectType>> &list,
                           int depth = 0)
    {
      const std::string prefix = std::string(depth, ' ');

      if (depth > 0) // Not printing [WebGL] prefix for nested.
        cerr << prefix;
      else
        cerr << "[WebGL] ";
      cerr << label << ": {";

      if (list.empty())
      {
        cerr << " (empty) }";
      }
      else
      {
        cerr << endl;
        int n = 0;
        for (const auto &obj : list)
          cerr << prefix << "  ." << n++ << " = " << obj->toString() << endl;
        cerr << prefix << "}";
      }
      cerr << endl;
    }

    void debugPrintPrograms(int depth = 0);
    void debugPrintShaderModules(int depth = 0);
    void debugPrintBuffers(int depth = 0);
    void debugPrintTextures(int depth = 0);
    void debugPrintFramebuffers(int depth = 0);
    void debugPrintRenderbuffers(int depth = 0);
    void debugPrint();

    Ref<TrContentRenderer> content_renderer_;
    Ref<TrRenderResource> getRenderResource();
    Ref<TrRenderPass> getCurrentRenderPass();

    template <typename ObjectType>
    class ObjectList : public std::vector<Ref<ObjectType>>
    {
      using Base = std::vector<Ref<ObjectType>>;
      using Base::Base;

    public:
      Ref<ObjectType> get(WebGLuint id)
      {
        for (const auto &obj : *this)
        {
          if (obj->id == id)
            return obj;
        }
        return nullptr;
      }

      bool has(WebGLuint id)
      {
        return get(id) != nullptr;
      }

      bool remove(WebGLuint id)
      {
        for (auto it = this->begin(); it != this->end(); it++)
        {
          if ((*it)->id == id)
          {
            this->erase(it);
            return true;
          }
        }
        return false;
      }
    };

    ObjectList<details::Shader> shaders_;
    ObjectList<details::Program> programs_;

    ObjectList<details::Buffer> buffers_;
    ObjectList<details::Texture> textures_;
    ObjectList<details::Framebuffer> framebuffers_;
    ObjectList<details::Renderbuffer> renderbuffers_;
    ObjectList<details::VertexArrayObject> vertex_array_objects_;

    template <typename Target, typename Type>
    class BindingMap : public std::unordered_map<Target, Ref<Type>, typename Target::HashKey>
    {
      using Base = std::unordered_map<Target, Ref<Type>, typename Target::HashKey>;
      using Base::Base;
    };
    using BufferBindingMap = BindingMap<details::BufferTarget, details::Buffer>;
    using TextureBindingMap = BindingMap<details::TextureTarget, details::Texture>;
    using FramebufferBindingMap = BindingMap<details::FramebufferTarget, details::Framebuffer>;
    using RenderbufferBindingMap = BindingMap<details::RenderbufferTarget, details::Renderbuffer>;

    BufferBindingMap buffer_bindings_;
    TextureBindingMap texture_bindings_;
    FramebufferBindingMap framebuffer_bindings_;
    RenderbufferBindingMap renderbuffer_bindings_;

    Ref<details::VertexArrayObject> current_vertex_array_object_;
    Ref<details::Program> current_program_;

    WebGLenum last_error_ = WEBGL_NO_ERROR;
    details::Capabilities caps_;
    WebGLenum active_texture_unit_ = WEBGL_TEXTURE0;

    WebGLfloat clear_color_[4];
    WebGLfloat clear_depth_;
    WebGLint clear_stencil_;

    WebGLfloat blend_color_[4];
    WebGLenum blend_equation_rgb_;
    WebGLenum blend_equation_alpha_;
    WebGLenum blend_sfactor_rgb_;
    WebGLenum blend_dfactor_rgb_;
    WebGLenum blend_sfactor_alpha_;
    WebGLenum blend_dfactor_alpha_;
    WebGLboolean color_mask_[4];

    WebGLenum cull_face_;
    WebGLenum front_face_;
    WebGLenum depth_func_;
    WebGLboolean depth_mask_;
    WebGLfloat depth_range_[2];

    WebGLenum stencil_func_;
    WebGLint stencil_ref_;
    WebGLuint stencil_mask_;
    WebGLenum stencil_fail_op_front_;
    WebGLenum stencil_zfail_op_front_;
    WebGLenum stencil_zpass_op_front_;
    WebGLenum stencil_fail_op_back_;
    WebGLenum stencil_zfail_op_back_;
    WebGLenum stencil_zpass_op_back_;

    WebGLfloat line_width_;
    WebGLfloat polygon_offset_factor_;
    WebGLfloat polygon_offset_units_;
    WebGLfloat sample_coverage_value_;
    WebGLboolean sample_coverage_invert_;
    WebGLint scissor_box_[4];
    WebGLint viewport_[4];

    // WebGPU pipeline-related states are maintained inside caps_
    Ref<TrRenderPass> current_render_pass_;
    std::unordered_map<WebGLuint, Ref<TrRenderPass>> framebuffer_renderpasses_;
  };
}
