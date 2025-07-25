#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class SetViewportCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<SetViewportCommandBufferRequest,
                                            COMMAND_BUFFER_SET_VIEWPORT_REQ>
  {
  public:
    SetViewportCommandBufferRequest() = delete;
    SetViewportCommandBufferRequest(int x, int y, int width, int height)
        : TrCommandBufferSimpleRequest()
        , x(x)
        , y(y)
        , width(width)
        , height(height)
    {
    }
    SetViewportCommandBufferRequest(const SetViewportCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , x(that.x)
        , y(that.y)
        , width(that.width)
        , height(that.height)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << x << ", "
         << y << ", "
         << width << ", "
         << height << ")";
      return ss.str();
    }

    /**
     * Serialize the command buffer to a JSON object with detailed viewport setup information.
     * Viewport commands are critical for understanding rendering output dimensions and clipping.
     * 
     * @param allocator The JSON allocator to use for creating the JSON object
     * @returns A JSON object containing base command info plus viewport details
     */
    rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const override
    {
      // Get base command information
      rapidjson::Value cmdInfo = TrCommandBufferBase::toJson(allocator);
      
      // Add viewport specific details
      rapidjson::Value viewportDetails(rapidjson::kObjectType);
      viewportDetails.AddMember("x", rapidjson::Value().SetInt(x), allocator);
      viewportDetails.AddMember("y", rapidjson::Value().SetInt(y), allocator);
      viewportDetails.AddMember("width", rapidjson::Value().SetInt(width), allocator);
      viewportDetails.AddMember("height", rapidjson::Value().SetInt(height), allocator);
      viewportDetails.AddMember("operation", rapidjson::Value().SetString("setViewport", allocator), allocator);
      viewportDetails.AddMember("aspectRatio", rapidjson::Value().SetDouble(height > 0 ? (double)width / height : 0.0), allocator);
      
      cmdInfo.AddMember("viewportDetails", viewportDetails, allocator);
      
      return cmdInfo;
    }

  public:
    int x;
    int y;
    int width;
    int height;
  };

  class SetScissorCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<SetScissorCommandBufferRequest,
                                            COMMAND_BUFFER_SET_SCISSOR_REQ>
  {
  public:
    SetScissorCommandBufferRequest() = delete;
    SetScissorCommandBufferRequest(int x, int y, int width, int height)
        : TrCommandBufferSimpleRequest()
        , x(x)
        , y(y)
        , width(width)
        , height(height)
    {
    }
    SetScissorCommandBufferRequest(const SetScissorCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , x(that.x)
        , y(that.y)
        , width(that.width)
        , height(that.height)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << x << ", "
         << y << ", "
         << width << ", "
         << height << ")";
      return ss.str();
    }

    /**
     * Serialize the command buffer to a JSON object with detailed scissor test setup information.
     * Scissor commands control pixel-level clipping and are important for rendering optimization.
     * 
     * @param allocator The JSON allocator to use for creating the JSON object
     * @returns A JSON object containing base command info plus scissor details
     */
    rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const override
    {
      // Get base command information
      rapidjson::Value cmdInfo = TrCommandBufferBase::toJson(allocator);
      
      // Add scissor specific details
      rapidjson::Value scissorDetails(rapidjson::kObjectType);
      scissorDetails.AddMember("x", rapidjson::Value().SetInt(x), allocator);
      scissorDetails.AddMember("y", rapidjson::Value().SetInt(y), allocator);
      scissorDetails.AddMember("width", rapidjson::Value().SetInt(width), allocator);
      scissorDetails.AddMember("height", rapidjson::Value().SetInt(height), allocator);
      scissorDetails.AddMember("operation", rapidjson::Value().SetString("setScissor", allocator), allocator);
      scissorDetails.AddMember("area", rapidjson::Value().SetInt(width * height), allocator);
      
      cmdInfo.AddMember("scissorDetails", scissorDetails, allocator);
      
      return cmdInfo;
    }

  public:
    int x;
    int y;
    int width;
    int height;
  };

  class HintCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<HintCommandBufferRequest, COMMAND_BUFFER_HINT_REQ>
  {
  public:
    HintCommandBufferRequest() = delete;
    HintCommandBufferRequest(int target, int mode)
        : TrCommandBufferSimpleRequest()
        , target(target)
        , mode(mode)
    {
    }
    HintCommandBufferRequest(const HintCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , target(that.target)
        , mode(that.mode)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << target << ", "
         << mode << ")";
      return ss.str();
    }

  public:
    int target;
    int mode;
  };

  class LineWidthCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<LineWidthCommandBufferRequest, COMMAND_BUFFER_LINE_WIDTH_REQ>
  {
  public:
    LineWidthCommandBufferRequest() = delete;
    LineWidthCommandBufferRequest(float width)
        : TrCommandBufferSimpleRequest()
        , width(width)
    {
    }
    LineWidthCommandBufferRequest(const LineWidthCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , width(that.width)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << width << ")";
      return ss.str();
    }

  public:
    float width;
  };

  class PixelStoreiCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<PixelStoreiCommandBufferRequest, COMMAND_BUFFER_PIXEL_STOREI_REQ>
  {
  public:
    PixelStoreiCommandBufferRequest() = delete;
    PixelStoreiCommandBufferRequest(int pname, int param)
        : TrCommandBufferSimpleRequest()
        , pname(pname)
        , param(param)
    {
    }
    PixelStoreiCommandBufferRequest(const PixelStoreiCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , pname(that.pname)
        , param(that.param)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << pname << ", "
         << param << ")";
      return ss.str();
    }

  public:
    int pname;
    int param;
  };

  class PolygonOffsetCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<PolygonOffsetCommandBufferRequest, COMMAND_BUFFER_POLYGON_OFFSET_REQ>
  {
  public:
    PolygonOffsetCommandBufferRequest() = delete;
    PolygonOffsetCommandBufferRequest(float factor, float units)
        : TrCommandBufferSimpleRequest()
        , factor(factor)
        , units(units)
    {
    }
    PolygonOffsetCommandBufferRequest(const PolygonOffsetCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , factor(that.factor)
        , units(that.units)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << factor << ", "
         << units << ")";
      return ss.str();
    }

  public:
    float factor;
    float units;
  };

  class DepthMaskCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<DepthMaskCommandBufferRequest, COMMAND_BUFFER_DEPTH_MASK_REQ>
  {
  public:
    DepthMaskCommandBufferRequest() = delete;
    DepthMaskCommandBufferRequest(bool flag)
        : TrCommandBufferSimpleRequest()
        , flag(flag)
    {
    }
    DepthMaskCommandBufferRequest(const DepthMaskCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , flag(that.flag)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << (flag ? "true" : "false") << ")";
      return ss.str();
    }

  public:
    bool flag;
  };

  class DepthFuncCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<DepthFuncCommandBufferRequest, COMMAND_BUFFER_DEPTH_FUNC_REQ>
  {
  public:
    DepthFuncCommandBufferRequest() = delete;
    DepthFuncCommandBufferRequest(int func)
        : TrCommandBufferSimpleRequest()
        , func(func)
    {
    }
    DepthFuncCommandBufferRequest(const DepthFuncCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , func(that.func)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "(" << func << ")";
      return ss.str();
    }

  public:
    int func;
  };

  class DepthRangeCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<DepthRangeCommandBufferRequest, COMMAND_BUFFER_DEPTH_RANGE_REQ>
  {
  public:
    DepthRangeCommandBufferRequest() = delete;
    DepthRangeCommandBufferRequest(float n, float f)
        : TrCommandBufferSimpleRequest()
        , n(n)
        , f(f)
    {
    }
    DepthRangeCommandBufferRequest(const DepthRangeCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , n(that.n)
        , f(that.f)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << n << ", "
         << f << ")";
      return ss.str();
    }

  public:
    float n;
    float f;
  };

  class StencilFuncCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<StencilFuncCommandBufferRequest, COMMAND_BUFFER_STENCIL_FUNC_REQ>
  {
  public:
    StencilFuncCommandBufferRequest() = delete;
    StencilFuncCommandBufferRequest(int func, int ref, int mask)
        : TrCommandBufferSimpleRequest()
        , func(func)
        , ref(ref)
        , mask(mask)
    {
    }
    StencilFuncCommandBufferRequest(const StencilFuncCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , func(that.func)
        , ref(that.ref)
        , mask(that.mask)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << func << ", "
         << ref << ", "
         << mask << ")";
      return ss.str();
    }

  public:
    int func;
    int ref;
    int mask;
  };

  class StencilFuncSeparateCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<StencilFuncSeparateCommandBufferRequest,
                                            COMMAND_BUFFER_STENCIL_FUNC_SEPARATE_REQ>
  {
  public:
    StencilFuncSeparateCommandBufferRequest() = delete;
    StencilFuncSeparateCommandBufferRequest(int face, int func, int ref, int mask)
        : TrCommandBufferSimpleRequest()
        , face(face)
        , func(func)
        , ref(ref)
        , mask(mask)
    {
    }
    StencilFuncSeparateCommandBufferRequest(const StencilFuncSeparateCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , face(that.face)
        , func(that.func)
        , ref(that.ref)
        , mask(that.mask)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << face << ", "
         << func << ", "
         << ref << ", "
         << mask << ")";
      return ss.str();
    }

  public:
    int face;
    int func;
    int ref;
    int mask;
  };

  class StencilMaskCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<StencilMaskCommandBufferRequest,
                                            COMMAND_BUFFER_STENCIL_MASK_REQ>
  {
  public:
    StencilMaskCommandBufferRequest() = delete;
    StencilMaskCommandBufferRequest(int mask)
        : TrCommandBufferSimpleRequest()
        , mask(mask)
    {
    }
    StencilMaskCommandBufferRequest(const StencilMaskCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , mask(that.mask)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "(" << mask << ")";
      return ss.str();
    }

  public:
    int mask;
  };

  class StencilMaskSeparateCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<StencilMaskSeparateCommandBufferRequest,
                                            COMMAND_BUFFER_STENCIL_MASK_SEPARATE_REQ>
  {
  public:
    StencilMaskSeparateCommandBufferRequest() = delete;
    StencilMaskSeparateCommandBufferRequest(int face, int mask)
        : TrCommandBufferSimpleRequest()
        , face(face)
        , mask(mask)
    {
    }
    StencilMaskSeparateCommandBufferRequest(const StencilMaskSeparateCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , face(that.face)
        , mask(that.mask)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << face << ", "
         << mask << ")";
      return ss.str();
    }

  public:
    int face;
    int mask;
  };

  class StencilOpCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<StencilOpCommandBufferRequest, COMMAND_BUFFER_STENCIL_OP_REQ>
  {
  public:
    StencilOpCommandBufferRequest() = delete;
    StencilOpCommandBufferRequest(int fail, int zfail, int zpass)
        : TrCommandBufferSimpleRequest()
        , fail(fail)
        , zfail(zfail)
        , zpass(zpass)
    {
    }
    StencilOpCommandBufferRequest(const StencilOpCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , fail(that.fail)
        , zfail(that.zfail)
        , zpass(that.zpass)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << fail << ", "
         << zfail << ", "
         << zpass << ")";
      return ss.str();
    }

  public:
    int fail;
    int zfail;
    int zpass;
  };

  class StencilOpSeparateCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<StencilOpSeparateCommandBufferRequest,
                                            COMMAND_BUFFER_STENCIL_OP_SEPARATE_REQ>
  {
  public:
    StencilOpSeparateCommandBufferRequest() = delete;
    StencilOpSeparateCommandBufferRequest(int face, int fail, int zfail, int zpass)
        : TrCommandBufferSimpleRequest()
        , face(face)
        , fail(fail)
        , zfail(zfail)
        , zpass(zpass)
    {
    }
    StencilOpSeparateCommandBufferRequest(const StencilOpSeparateCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , face(that.face)
        , fail(that.fail)
        , zfail(that.zfail)
        , zpass(that.zpass)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << face << ", "
         << fail << ", "
         << zfail << ", "
         << zpass << ")";
      return ss.str();
    }

  public:
    int face;
    int fail;
    int zfail;
    int zpass;
  };

  class BlendColorCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<BlendColorCommandBufferRequest, COMMAND_BUFFER_BLEND_COLOR_REQ>
  {
  public:
    BlendColorCommandBufferRequest() = delete;
    BlendColorCommandBufferRequest(float red, float green, float blue, float alpha)
        : TrCommandBufferSimpleRequest()
        , red(red)
        , green(green)
        , blue(blue)
        , alpha(alpha)
    {
    }
    BlendColorCommandBufferRequest(const BlendColorCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , red(that.red)
        , green(that.green)
        , blue(that.blue)
        , alpha(that.alpha)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << red << ", "
         << green << ", "
         << blue << ", "
         << alpha << ")";
      return ss.str();
    }

  public:
    float red;
    float green;
    float blue;
    float alpha;
  };

  class BlendEquationCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<BlendEquationCommandBufferRequest, COMMAND_BUFFER_BLEND_EQUATION_REQ>
  {
  public:
    BlendEquationCommandBufferRequest() = delete;
    BlendEquationCommandBufferRequest(int mode)
        : TrCommandBufferSimpleRequest()
        , mode(mode)
    {
    }
    BlendEquationCommandBufferRequest(const BlendEquationCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , mode(that.mode)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "(" << mode << ")";
      return ss.str();
    }

  public:
    int mode;
  };

  class BlendEquationSeparateCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<BlendEquationSeparateCommandBufferRequest,
                                            COMMAND_BUFFER_BLEND_EQUATION_SEPARATE_REQ>
  {
  public:
    BlendEquationSeparateCommandBufferRequest() = delete;
    BlendEquationSeparateCommandBufferRequest(int modeRGB, int modeAlpha)
        : TrCommandBufferSimpleRequest()
        , modeRGB(modeRGB)
        , modeAlpha(modeAlpha)
    {
    }
    BlendEquationSeparateCommandBufferRequest(const BlendEquationSeparateCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , modeRGB(that.modeRGB)
        , modeAlpha(that.modeAlpha)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << modeRGB << ", "
         << modeAlpha << ")";
      return ss.str();
    }

  public:
    int modeRGB;
    int modeAlpha;
  };

  class BlendFuncCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<BlendFuncCommandBufferRequest, COMMAND_BUFFER_BLEND_FUNC_REQ>
  {
  public:
    BlendFuncCommandBufferRequest() = delete;
    BlendFuncCommandBufferRequest(int sfactor, int dfactor)
        : TrCommandBufferSimpleRequest()
        , sfactor(sfactor)
        , dfactor(dfactor)
    {
    }
    BlendFuncCommandBufferRequest(const BlendFuncCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , sfactor(that.sfactor)
        , dfactor(that.dfactor)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << sfactor << ", "
         << dfactor << ")";
      return ss.str();
    }

  public:
    int sfactor;
    int dfactor;
  };

  class BlendFuncSeparateCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<BlendFuncSeparateCommandBufferRequest,
                                            COMMAND_BUFFER_BLEND_FUNC_SEPARATE_REQ>
  {
  public:
    BlendFuncSeparateCommandBufferRequest() = delete;
    BlendFuncSeparateCommandBufferRequest(int srcRGB, int dstRGB, int srcAlpha, int dstAlpha)
        : TrCommandBufferSimpleRequest()
        , srcRGB(srcRGB)
        , dstRGB(dstRGB)
        , srcAlpha(srcAlpha)
        , dstAlpha(dstAlpha)
    {
    }
    BlendFuncSeparateCommandBufferRequest(const BlendFuncSeparateCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , srcRGB(that.srcRGB)
        , dstRGB(that.dstRGB)
        , srcAlpha(that.srcAlpha)
        , dstAlpha(that.dstAlpha)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << srcRGB << ", "
         << dstRGB << ", "
         << srcAlpha << ", "
         << dstAlpha << ")";
      return ss.str();
    }

  public:
    int srcRGB;
    int dstRGB;
    int srcAlpha;
    int dstAlpha;
  };

  class ColorMaskCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<ColorMaskCommandBufferRequest,
                                            COMMAND_BUFFER_COLOR_MASK_REQ>
  {
  public:
    ColorMaskCommandBufferRequest() = delete;
    ColorMaskCommandBufferRequest(bool red, bool green, bool blue, bool alpha)
        : TrCommandBufferSimpleRequest()
        , red(red)
        , green(green)
        , blue(blue)
        , alpha(alpha)
    {
    }
    ColorMaskCommandBufferRequest(const ColorMaskCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , red(that.red)
        , green(that.green)
        , blue(that.blue)
        , alpha(that.alpha)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << (red ? "true" : "false") << ", "
         << (green ? "true" : "false") << ", "
         << (blue ? "true" : "false") << ", "
         << (alpha ? "true" : "false") << ")";
      return ss.str();
    }

  public:
    bool red;
    bool green;
    bool blue;
    bool alpha;
  };

  class CullFaceCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<CullFaceCommandBufferRequest, COMMAND_BUFFER_CULL_FACE_REQ>
  {
  public:
    CullFaceCommandBufferRequest() = delete;
    CullFaceCommandBufferRequest(int mode)
        : TrCommandBufferSimpleRequest()
        , mode(mode)
    {
    }
    CullFaceCommandBufferRequest(const CullFaceCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , mode(that.mode)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "(" << mode << ")";
      return ss.str();
    }

  public:
    int mode;
  };

  class FrontFaceCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<FrontFaceCommandBufferRequest, COMMAND_BUFFER_FRONT_FACE_REQ>
  {
  public:
    FrontFaceCommandBufferRequest() = delete;
    FrontFaceCommandBufferRequest(int mode)
        : TrCommandBufferSimpleRequest()
        , mode(mode)
    {
    }
    FrontFaceCommandBufferRequest(const FrontFaceCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , mode(that.mode)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "(" << mode << ")";
      return ss.str();
    }

  public:
    int mode;
  };

  template <typename Derived, CommandBufferType Type>
  class CapabilityCommandBufferRequest
      : public TrCommandBufferSimpleRequest<Derived, Type>
  {
  public:
    CapabilityCommandBufferRequest() = delete;
    CapabilityCommandBufferRequest(int cap)
        : TrCommandBufferSimpleRequest<Derived, Type>()
        , cap(cap)
    {
    }
    CapabilityCommandBufferRequest(const CapabilityCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest<Derived, Type>(that, clone)
        , cap(that.cap)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest<Derived, Type>::toString(line_prefix) << "("
         << WebGLHelper::WebGLEnumToString(cap)
         << ")";
      return ss.str();
    }

  public:
    int cap;
  };

  class EnableCommandBufferRequest final
      : public CapabilityCommandBufferRequest<EnableCommandBufferRequest, COMMAND_BUFFER_ENABLE_REQ>
  {
    using CapabilityCommandBufferRequest::CapabilityCommandBufferRequest;
  };

  class DisableCommandBufferRequest final
      : public CapabilityCommandBufferRequest<DisableCommandBufferRequest, COMMAND_BUFFER_DISABLE_REQ>
  {
    using CapabilityCommandBufferRequest::CapabilityCommandBufferRequest;
  };
}
