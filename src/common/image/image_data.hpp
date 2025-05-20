#pragma once

#include <cstdint>
#include <vector>

namespace transmute
{
  struct ImageData
  {
    int32_t width;
    int32_t height;
    std::vector<uint8_t> pixels;

    ImageData(int32_t w, int32_t h)
        : width(w),
          height(h),
          pixels(w * h * 4, 0) // Assuming 4 bytes per pixel (RGBA)
    {
    }
  };
}
