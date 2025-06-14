#pragma once

#include <stdint.h>

namespace xr
{
  class WebGLLayer
  {
  public:
    WebGLLayer()
        : antialias(true)
        , depth(true)
        , stencil(false)
        , alpha(true)
        , multiviewRequired(false)
        , ignoreDepthValues(false)
        , framebufferScaleFactor(1.0f)
        , framebuffer(0)
        , framebufferWidth(0)
        , framebufferHeight(0)
    {
    }

  public:
    void update(WebGLLayer *layer)
    {
      antialias = layer->antialias;
      depth = layer->depth;
      stencil = layer->stencil;
      alpha = layer->alpha;
      multiviewRequired = layer->multiviewRequired;
      ignoreDepthValues = layer->ignoreDepthValues;
      framebufferScaleFactor = layer->framebufferScaleFactor;
      framebuffer = layer->framebuffer;
      framebufferWidth = layer->framebufferWidth;
      framebufferHeight = layer->framebufferHeight;
    }

  public:
    bool antialias;
    bool depth;
    bool stencil;
    bool alpha;
    bool multiviewRequired;
    bool ignoreDepthValues;
    float framebufferScaleFactor;
    uint32_t framebuffer;
    uint32_t framebufferWidth;
    uint32_t framebufferHeight;
  };
}
