#pragma once

#include <stdint.h>

namespace xr
{
  class WebGLLayer
  {
  public:
    WebGLLayer() : antialias(true),
                   depth(true),
                   stencil(false),
                   alpha(true),
                   multiview(false),
                   ignoreDepthValues(false),
                   framebufferScaleFactor(1.0f),
                   framebuffer(0),
                   framebufferWidth(0),
                   framebufferHeight(0)
    {
    }

  public:
    bool antialias;
    bool depth;
    bool stencil;
    bool alpha;
    bool multiview;
    bool ignoreDepthValues;
    float framebufferScaleFactor;
    uint32_t framebuffer;
    uint32_t framebufferWidth;
    uint32_t framebufferHeight;
  };
}
