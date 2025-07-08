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
