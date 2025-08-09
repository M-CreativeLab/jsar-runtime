#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"

namespace builtin_scene::text::sdf
{
  /**
   * Configuration parameters for SDF generation from bitmaps
   */
  struct SDFParams
  {
    int radius = 8;       // Distance field radius
    float cutoff = 0.25f; // Alpha cutoff for edge detection

    SDFParams() = default;
    SDFParams(int radius, float cutoff = 0.25f)
        : radius(radius)
        , cutoff(cutoff)
    {
    }
  };

  /**
   * TinySDF - CPU-based SDF generator for bitmap textures
   * 
   * This class generates signed distance field textures from SkBitmap/SkCanvas
   * using a CPU-based implementation inspired by TinySDF.js from Mapbox.
   * 
   * Reference: https://github.com/mapbox/tiny-sdf
   * Original implementation by Mapbox under BSD 2-Clause License
   */
  class TinySDF
  {
  public:
    explicit TinySDF(const SDFParams &params = SDFParams());
    ~TinySDF() = default;

    // Non-copyable but movable
    TinySDF(const TinySDF &) = delete;
    TinySDF &operator=(const TinySDF &) = delete;
    TinySDF(TinySDF &&) = default;
    TinySDF &operator=(TinySDF &&) = default;

    /**
     * Generate SDF from SkBitmap
     * @param bitmap Input bitmap to convert to SDF
     * @return SDF bitmap data (single channel), or empty vector if failed
     */
    std::vector<uint8_t> generateFromBitmap(const SkBitmap &bitmap);

    /**
     * Generate SDF from SkCanvas content
     * @param canvas Input canvas to convert to SDF
     * @return SDF bitmap data (single channel), or empty vector if failed
     */
    std::vector<uint8_t> generateFromCanvas(SkCanvas *canvas);

    /**
     * Get current SDF parameters
     */
    const SDFParams &getParams() const
    {
      return params_;
    }

    /**
     * Update SDF parameters
     */
    void setParams(const SDFParams &params)
    {
      params_ = params;
    }

  private:
    SDFParams params_;

    // Internal methods for SDF generation
    std::vector<uint8_t> generateSDF(const std::vector<uint8_t> &bitmap,
                                     int width,
                                     int height);
    double distanceToEdge(const std::vector<uint8_t> &bitmap, int width, int height, int x, int y, bool inside);
    std::vector<uint8_t> extractAlphaChannel(const SkBitmap &bitmap);
  };
}