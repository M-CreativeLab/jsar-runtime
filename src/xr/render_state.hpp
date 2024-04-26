#pragma once

#include <stdint.h>
#include "layer.hpp"

namespace xr
{
  class RenderState
  {
  public:
    RenderState() : depthFar(1000.0),
                    depthNear(0.1),
                    inlineVerticalFieldOfView(0)
    {
    }
    RenderState(RenderState *state) : baseLayer(state->baseLayer),
                                      depthFar(state->depthFar),
                                      depthNear(state->depthNear),
                                      inlineVerticalFieldOfView(state->inlineVerticalFieldOfView)
    {
    }
    ~RenderState()
    {
      if (baseLayer != nullptr)
        delete baseLayer;
    }

  public:
    void updateBaseLayer(WebGLLayer *layer)
    {
      if (baseLayer != nullptr)
        delete baseLayer;
      baseLayer = new WebGLLayer(*layer);
    }

  public:
    WebGLLayer *baseLayer = nullptr;
    float depthFar;
    float depthNear;
    uint32_t inlineVerticalFieldOfView;
  };
}
