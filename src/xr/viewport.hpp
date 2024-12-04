#pragma once

#include <stdint.h>
#include <common/viewport.hpp>

namespace xr
{
  class Viewport
  {
  public:
    Viewport() : width(0), height(0), x(0), y(0)
    {
    }
    Viewport(uint32_t width, uint32_t height, uint32_t x, uint32_t y) : width(width),
                                                                        height(height),
                                                                        x(x),
                                                                        y(y)
    {
    }
    Viewport(TrViewport &viewport) : width(viewport.width),
                                     height(viewport.height),
                                     x(viewport.x),
                                     y(viewport.y)
    {
    }

  public:
    uint32_t width;
    uint32_t height;
    uint32_t x;
    uint32_t y;
  };
}
