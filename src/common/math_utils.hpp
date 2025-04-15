#pragma once

#include <cmath>
#include <algorithm>

namespace transmute::common
{
  class math_utils
  {
  public:
    /**
     * Clamp a value between a minimum and maximum, commonly it's from 0.0f to 1.0f.
     * 
     * @param value The value to clamp.
     * @param min The minimum value, default is 0.0f.
     * @param max The maximum value, default is 1.0f.
     * @returns The clamped value, which is always between min and max.
     */
    static float Clamp(float value, float min = 0.0f, float max = 1.0f)
    {
      return std::max(min, std::min(value, max));
    }

    /**
     * Clamp a value to be non-negative, such as clamping all negative values to 0.0f.
     * 
     * @param value The value to clamp.
     * @returns The clamped value, which is always >= 0.0f.
     */
    static float ClampNegativeToZero(float value)
    {
      return std::max(0.0f, value);
    }
  };
}