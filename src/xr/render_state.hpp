#pragma once

#include <stdint.h>
#include "layer.hpp"

namespace xr
{
  class RenderState
  {
  public:
    RenderState()
        : depthFar(1000.0)
        , depthNear(0.1)
        , inlineVerticalFieldOfView(0)
    {
    }
    RenderState(RenderState *state)
        : depthFar(state->depthFar)
        , depthNear(state->depthNear)
        , inlineVerticalFieldOfView(state->inlineVerticalFieldOfView)
    {
      if (state->baseLayer != nullptr)
      {
        baseLayer = new WebGLLayer();
        baseLayer->update(state->baseLayer);
      }
    }
    ~RenderState()
    {
      if (baseLayer != nullptr)
      {
        delete baseLayer;
        baseLayer = nullptr;
      }
    }

  public:
    void update(RenderState *state)
    {
      depthFar = state->depthFar;
      depthNear = state->depthNear;
      inlineVerticalFieldOfView = state->inlineVerticalFieldOfView;
      if (state->baseLayer != nullptr)
        updateBaseLayer(state->baseLayer);
    }
    void updateBaseLayer(WebGLLayer *layer)
    {
      if (baseLayer == nullptr)
        baseLayer = new WebGLLayer();
      baseLayer->update(layer);
    }

  public:
    WebGLLayer *baseLayer = nullptr;
    float depthFar;
    float depthNear;
    float inlineVerticalFieldOfView;
  };
}
