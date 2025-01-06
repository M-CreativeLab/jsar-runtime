#pragma once

#include <string>
#include <unordered_map>

namespace client_cssom
{
  // The Pixel Per Inch (PPI) constant.
  constexpr const int PPI = 96;

  /**
   * Convert the pixel value to the centimeter value.
   * 
   * @param pixel The pixel value to convert.
   * @returns The centimeter value.
   */
  inline float pixelToCm(float pixel)
  {
    return pixel / PPI * 2.54f;
  }

  /**
   * Convert the pixel value to the meter value.
   * 
   * @param pixel The pixel value to convert.
   * @returns The meter value.
   */
  inline float pixelToMeter(float pixel)
  {
    return pixelToCm(pixel) / 100.0f;
  }
}
