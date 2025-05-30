#pragma once

#include <string>
#include <unordered_map>
#include <math/vectors.hpp>

namespace client_cssom
{
  // The Pixel Per Inch (PPI) constant.
  constexpr const int PPI = 92;
  
  // The device pixel ratio.
  constexpr const float DevicePixelRatio = 1.5f;

  // The screen width and height.
  constexpr const int ScreenWidth = 1920;
  constexpr const int ScreenHeight = 1080;
  constexpr const int VolumeDepth = 400;

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

  /**
   * Convert a `Size3` value from pixel to a new `Size3` value in meter.
   * 
   * @param sizeInPx The size in pixel to convert.
   * @returns The new size in meter.
   */
  inline math::Size3 pixelToMeter(math::Size3 sizeInPx)
  {
    return math::Size3(pixelToMeter(sizeInPx.width()),
                       pixelToMeter(sizeInPx.height()),
                       pixelToMeter(sizeInPx.depth()));
  }

  inline float meterToPixel(float meter)
  {
    return meter * 100.0f * PPI / 2.54f;
  }
}
