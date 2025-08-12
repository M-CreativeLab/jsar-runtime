#pragma once

#include <vector>
#include <memory>
#include <cstdint>

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
   * TinySDF - CPU-based SDF generator for raw pixel data
   * 
   * This class generates signed distance field textures from raw pixel data
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
     * Generate SDF from raw pixel data and update alpha channel in place
     * Only modifies the alpha channel, preserving RGB channels
     * @param pixels Raw pixel data (RGBA format, modified in place)
     * @param width Width of the image in pixels
     * @param height Height of the image in pixels
     * @return true if SDF generation succeeded, false otherwise
     */
    bool generateFromPixelsInPlace(unsigned char *pixels, int width, int height);

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

    // Internal methods for SDF generation using EDT algorithm
    std::vector<uint8_t> extractAlphaChannel(const unsigned char *pixels, int width, int height);
    bool generateSDFFromGrids(unsigned char *pixels, int width, int height, const std::vector<double> &gridOuter, const std::vector<double> &gridInner);

    // Euclidean Distance Transform (EDT) algorithm implementation
    // Based on "Distance Transforms of Sampled Functions" by Felzenszwalb & Huttenlocher
    void edt(std::vector<double> &data, int x0, int y0, int width, int height, int gridSize, std::vector<double> &f, std::vector<int> &v, std::vector<double> &z);
    void edt1d(std::vector<double> &grid, int offset, int stride, int length, std::vector<double> &f, std::vector<int> &v, std::vector<double> &z);
  };
}