#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class ClearCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<ClearCommandBufferRequest, COMMAND_BUFFER_CLEAR_REQ>
  {
  public:
    ClearCommandBufferRequest() = delete;
    ClearCommandBufferRequest(int mask)
        : TrCommandBufferSimpleRequest(),
          mask(mask)
    {
    }

  public:
    int mask;
  };

  class ClearColorCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<ClearColorCommandBufferRequest,
                                            COMMAND_BUFFER_CLEAR_COLOR_REQ>
  {
  public:
    ClearColorCommandBufferRequest() = delete;
    ClearColorCommandBufferRequest(float r, float g, float b, float a)
        : TrCommandBufferSimpleRequest(),
          r(r), g(g), b(b), a(a)
    {
    }

  public:
    float r;
    float g;
    float b;
    float a;
  };

  class ClearDepthCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<ClearDepthCommandBufferRequest,
                                            COMMAND_BUFFER_CLEAR_DEPTH_REQ>
  {
  public:
    ClearDepthCommandBufferRequest() = delete;
    ClearDepthCommandBufferRequest(float depth)
        : TrCommandBufferSimpleRequest(),
          depth(depth)
    {
    }

  public:
    float depth;
  };

  class ClearStencilCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<ClearStencilCommandBufferRequest,
                                            COMMAND_BUFFER_CLEAR_STENCIL_REQ>
  {
  public:
    ClearStencilCommandBufferRequest() = delete;
    ClearStencilCommandBufferRequest(int stencil)
        : TrCommandBufferSimpleRequest(),
          stencil(stencil)
    {
    }

  public:
    int stencil;
  };
}
