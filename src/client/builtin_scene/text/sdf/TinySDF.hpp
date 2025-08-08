#pragma once

#include <vector>
#include <memory>
#include <cstdint>

namespace builtin_scene::text::sdf
{
  /**
   * Configuration parameters for SDF generation
   */
  struct SDFParams
  {
    int fontSize = 24;    // Font size in pixels
    int buffer = 8;       // Buffer around glyph
    int radius = 8;       // Distance field radius
    float cutoff = 0.25f; // Alpha cutoff

    SDFParams() = default;
    SDFParams(int fontSize, int buffer, int radius, float cutoff = 0.25f)
        : fontSize(fontSize)
        , buffer(buffer)
        , radius(radius)
        , cutoff(cutoff)
    {
    }
  };

  /**
   * Represents a single glyph with its SDF data
   */
  struct SDFGlyph
  {
    uint32_t codepoint;        // Unicode codepoint
    int width;                 // Glyph width
    int height;                // Glyph height
    int left;                  // Left bearing
    int top;                   // Top bearing
    int advance;               // Advance width
    std::vector<uint8_t> data; // SDF bitmap data (single channel)

    SDFGlyph() = default;
    SDFGlyph(uint32_t cp, int w, int h, int l, int t, int adv)
        : codepoint(cp)
        , width(w)
        , height(h)
        , left(l)
        , top(t)
        , advance(adv)
    {
    }
  };

  /**
   * TinySDF - CPU-based SDF generator for text glyphs
   * 
   * This class generates signed distance field textures from font glyphs
   * using a CPU-based implementation similar to TinySDF.js
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
     * Generate SDF for a single glyph
     * @param codepoint Unicode codepoint
     * @return SDFGlyph with generated SDF data, or nullptr if failed
     */
    std::unique_ptr<SDFGlyph> generateGlyph(uint32_t codepoint);

    /**
     * Generate SDF for multiple glyphs
     * @param codepoints Vector of Unicode codepoints
     * @return Vector of SDFGlyph pointers
     */
    std::vector<std::unique_ptr<SDFGlyph>> generateGlyphs(const std::vector<uint32_t> &codepoints);

    /**
     * Get current SDF parameters
     */
    const SDFParams &getParams() const
    {
      return params_;
    }

    /**
     * Update SDF parameters (will require regenerating glyphs)
     */
    void setParams(const SDFParams &params);

  private:
    SDFParams params_;

    // Internal methods for SDF generation
    std::vector<uint8_t> rasterizeGlyph(uint32_t codepoint, int &width, int &height, int &left, int &top, int &advance);
    std::vector<uint8_t> generateSDF(const std::vector<uint8_t> &bitmap,
                                     int width,
                                     int height);
    double distanceToEdge(const std::vector<uint8_t> &bitmap, int width, int height, int x, int y, bool inside);
  };
}