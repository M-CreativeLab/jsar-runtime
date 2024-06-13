#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class ClearCommandBufferRequest : public TrCommandBufferSimpleRequest<ClearCommandBufferRequest>
  {
  public:
    ClearCommandBufferRequest(int mask)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_CLEAR_REQ),
          mask(mask)
    {
    }

  public:
    int mask;
  };

  class ClearColorCommandBufferRequest : public TrCommandBufferSimpleRequest<ClearColorCommandBufferRequest>
  {
  public:
    ClearColorCommandBufferRequest(float r, float g, float b, float a)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_CLEAR_COLOR_REQ),
          r(r),
          g(g),
          b(b),
          a(a)
    {
    }

  public:
    float r;
    float g;
    float b;
    float a;
  };

  class ClearDepthCommandBufferRequest : public TrCommandBufferSimpleRequest<ClearDepthCommandBufferRequest>
  {
  public:
    ClearDepthCommandBufferRequest(float depth)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_CLEAR_DEPTH_REQ),
          depth(depth)
    {
    }

  public:
    float depth;
  };

  class ClearStencilCommandBufferRequest : public TrCommandBufferSimpleRequest<ClearStencilCommandBufferRequest>
  {
  public:
    ClearStencilCommandBufferRequest(int stencil)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_CLEAR_STENCIL_REQ),
          stencil(stencil)
    {
    }

  public:
    int stencil;
  };
}
