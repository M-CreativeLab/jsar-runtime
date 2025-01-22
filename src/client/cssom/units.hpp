#pragma once

#include <string>
#include <unordered_map>

namespace client_cssom
{
  // The Pixel Per Inch (PPI) constant.
  constexpr const int PPI = 92;
  
  // The device pixel ratio.
  constexpr const int DevicePixelRatio = 1.0f;

  // The screen width and height.
  constexpr const int ScreenWidth = 1920;
  constexpr const int ScreenHeight = 1080;

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
   * Convert the pixel value to the meter value, the formula is: `pixelToMeter(pixel) = 
   * (pixel / PPI * 2.54f) / 100.0f`.
   *
   * Where PPI is the Pixel Per Inch constant, which is 96.
   *
   * @param pixel The pixel value to convert.
   * @returns The meter value.
   */
  inline float pixelToMeter(float pixel)
  {
    return pixelToCm(pixel) / 100.0f;
  }
}
