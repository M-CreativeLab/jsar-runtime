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

  public:
    int cap;
  };

  class EnableCommandBufferRequest final
      : public CapabilityCommandBufferRequest<EnableCommandBufferRequest, COMMAND_BUFFER_ENABLE_REQ>
  {
  public:
    using CapabilityCommandBufferRequest::CapabilityCommandBufferRequest;
  };

  class DisableCommandBufferRequest final
      : public CapabilityCommandBufferRequest<DisableCommandBufferRequest, COMMAND_BUFFER_DISABLE_REQ>
  {
  public:
    using CapabilityCommandBufferRequest::CapabilityCommandBufferRequest;
  };
}
