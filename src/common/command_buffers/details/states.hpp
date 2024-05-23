#pragma once

#include "../shared.hpp"
#include "../base.hpp"
#include "../message.hpp"

namespace commandbuffers
{
  class SetViewportCommandBufferRequest : public TrCommandBufferSimpleRequest<SetViewportCommandBufferRequest>
  {
  public:
    SetViewportCommandBufferRequest(int x, int y, int width, int height)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_SET_VIEWPORT_REQ),
          x(x),
          y(y),
          width(width),
          height(height)
    {
    }

  public:
    int x;
    int y;
    int width;
    int height;
  };

  class SetScissorCommandBufferRequest : public TrCommandBufferSimpleRequest<SetScissorCommandBufferRequest>
  {
  public:
    SetScissorCommandBufferRequest(int x, int y, int width, int height)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_SET_SCISSOR_REQ),
          x(x),
          y(y),
          width(width),
          height(height)
    {
    }

  public:
    int x;
    int y;
    int width;
    int height;
  };

  class PixelStoreiCommandBufferRequest : public TrCommandBufferSimpleRequest<PixelStoreiCommandBufferRequest>
  {
  public:
    PixelStoreiCommandBufferRequest(int pname, int param)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_PIXEL_STOREI_REQ),
          pname(pname),
          param(param)
    {
    }

  public:
    int pname;
    int param;
  };

  class PolygonOffsetCommandBufferRequest : public TrCommandBufferSimpleRequest<PolygonOffsetCommandBufferRequest>
  {
  public:
    PolygonOffsetCommandBufferRequest(float factor, float units)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_POLYGON_OFFSET_REQ),
          factor(factor),
          units(units)
    {
    }

  public:
    float factor;
    float units;
  };

  class DepthMaskCommandBufferRequest : public TrCommandBufferSimpleRequest<DepthMaskCommandBufferRequest>
  {
  public:
    DepthMaskCommandBufferRequest(bool flag)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DEPTH_MASK_REQ),
          flag(flag)
    {
    }

  public:
    bool flag;
  };

  class DepthFuncCommandBufferRequest : public TrCommandBufferSimpleRequest<DepthFuncCommandBufferRequest>
  {
  public:
    DepthFuncCommandBufferRequest(int func)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DEPTH_FUNC_REQ),
          func(func)
    {
    }

  public:
    int func;
  };

  class DepthRangeCommandBufferRequest : public TrCommandBufferSimpleRequest<DepthRangeCommandBufferRequest>
  {
  public:
    DepthRangeCommandBufferRequest(float near, float far)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DEPTH_RANGE_REQ),
          near(near),
          far(far)
    {
    }

  public:
    float near;
    float far;
  };

  class StencilFuncCommandBufferRequest : public TrCommandBufferSimpleRequest<StencilFuncCommandBufferRequest>
  {
  public:
    StencilFuncCommandBufferRequest(int func, int ref, int mask)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_STENCIL_FUNC_REQ),
          func(func),
          ref(ref),
          mask(mask)
    {
    }

  public:
    int func;
    int ref;
    int mask;
  };

  class StencilFuncSeparateCommandBufferRequest : public TrCommandBufferSimpleRequest<StencilFuncSeparateCommandBufferRequest>
  {
  public:
    StencilFuncSeparateCommandBufferRequest(int face, int func, int ref, int mask)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_STENCIL_FUNC_SEPARATE_REQ),
          face(face),
          func(func),
          ref(ref),
          mask(mask)
    {
    }

  public:
    int face;
    int func;
    int ref;
    int mask;
  };

  class StencilMaskCommandBufferRequest : public TrCommandBufferSimpleRequest<StencilMaskCommandBufferRequest>
  {
  public:
    StencilMaskCommandBufferRequest(int mask)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_STENCIL_MASK_REQ),
          mask(mask)
    {
    }

  public:
    int mask;
  };

  class StencilMaskSeparateCommandBufferRequest : public TrCommandBufferSimpleRequest<StencilMaskSeparateCommandBufferRequest>
  {
  public:
    StencilMaskSeparateCommandBufferRequest(int face, int mask)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_STENCIL_MASK_SEPARATE_REQ),
          face(face),
          mask(mask)
    {
    }

  public:
    int face;
    int mask;
  };

  class StencilOpCommandBufferRequest : public TrCommandBufferSimpleRequest<StencilOpCommandBufferRequest>
  {
  public:
    StencilOpCommandBufferRequest(int fail, int zfail, int zpass)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_STENCIL_OP_REQ),
          fail(fail),
          zfail(zfail),
          zpass(zpass)
    {
    }

  public:
    int fail;
    int zfail;
    int zpass;
  };

  class StencilOpSeparateCommandBufferRequest : public TrCommandBufferSimpleRequest<StencilOpSeparateCommandBufferRequest>
  {
  public:
    StencilOpSeparateCommandBufferRequest(int face, int fail, int zfail, int zpass)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_STENCIL_OP_SEPARATE_REQ),
          face(face),
          fail(fail),
          zfail(zfail),
          zpass(zpass)
    {
    }

  public:
    int face;
    int fail;
    int zfail;
    int zpass;
  };

  class BlendColorCommandBufferRequest : public TrCommandBufferSimpleRequest<BlendColorCommandBufferRequest>
  {
  public:
    BlendColorCommandBufferRequest(float red, float green, float blue, float alpha)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_BLEND_COLOR_REQ),
          red(red),
          green(green),
          blue(blue),
          alpha(alpha)
    {
    }

  public:
    float red;
    float green;
    float blue;
    float alpha;
  };

  class BlendEquationCommandBufferRequest : public TrCommandBufferSimpleRequest<BlendEquationCommandBufferRequest>
  {
  public:
    BlendEquationCommandBufferRequest(int mode)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_BLEND_EQUATION_REQ),
          mode(mode)
    {
    }

  public:
    int mode;
  };

  class BlendEquationSeparateCommandBufferRequest : public TrCommandBufferSimpleRequest<BlendEquationSeparateCommandBufferRequest>
  {
  public:
    BlendEquationSeparateCommandBufferRequest(int modeRGB, int modeAlpha)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_BLEND_EQUATION_SEPARATE_REQ),
          modeRGB(modeRGB),
          modeAlpha(modeAlpha)
    {
    }

  public:
    int modeRGB;
    int modeAlpha;
  };

  class BlendFuncCommandBufferRequest : public TrCommandBufferSimpleRequest<BlendFuncCommandBufferRequest>
  {
  public:
    BlendFuncCommandBufferRequest(int sfactor, int dfactor)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_BLEND_FUNC_REQ),
          sfactor(sfactor),
          dfactor(dfactor)
    {
    }

  public:
    int sfactor;
    int dfactor;
  };

  class BlendFuncSeparateCommandBufferRequest : public TrCommandBufferSimpleRequest<BlendFuncSeparateCommandBufferRequest>
  {
  public:
    BlendFuncSeparateCommandBufferRequest(int srcRGB, int dstRGB, int srcAlpha, int dstAlpha)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_BLEND_FUNC_SEPARATE_REQ),
          srcRGB(srcRGB),
          dstRGB(dstRGB),
          srcAlpha(srcAlpha),
          dstAlpha(dstAlpha)
    {
    }

  public:
    int srcRGB;
    int dstRGB;
    int srcAlpha;
    int dstAlpha;
  };

  class ColorMaskCommandBufferRequest : public TrCommandBufferSimpleRequest<ColorMaskCommandBufferRequest>
  {
  public:
    ColorMaskCommandBufferRequest(bool red, bool green, bool blue, bool alpha)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_COLOR_MASK_REQ),
          red(red),
          green(green),
          blue(blue),
          alpha(alpha)
    {
    }

  public:
    bool red;
    bool green;
    bool blue;
    bool alpha;
  };

  class CullFaceCommandBufferRequest : public TrCommandBufferSimpleRequest<CullFaceCommandBufferRequest>
  {
  public:
    CullFaceCommandBufferRequest(int mode)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_CULL_FACE_REQ),
          mode(mode)
    {
    }

  public:
    int mode;
  };

  class FrontFaceCommandBufferRequest : public TrCommandBufferSimpleRequest<FrontFaceCommandBufferRequest>
  {
  public:
    FrontFaceCommandBufferRequest(int mode)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_FRONT_FACE_REQ),
          mode(mode)
    {
    }

  public:
    int mode;
  };

  class EnableCommandBufferRequest : public TrCommandBufferSimpleRequest<EnableCommandBufferRequest>
  {
  public:
    EnableCommandBufferRequest(int cap)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_ENABLE_REQ),
          cap(cap)
    {
    }

  public:
    int cap;
  };

  class DisableCommandBufferRequest : public TrCommandBufferSimpleRequest<DisableCommandBufferRequest>
  {
  public:
    DisableCommandBufferRequest(int cap)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DISABLE_REQ),
          cap(cap)
    {
    }

  public:
    int cap;
  };
}
