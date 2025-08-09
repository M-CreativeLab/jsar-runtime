#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <cstdint>
#include "./tiny_sdf.hpp"

namespace builtin_scene::text::sdf
{
  /**
   * UV coordinates for a glyph in an atlas
   */
  struct GlyphUV
  {
    float u0, v0, u1, v1; // UV coordinates in atlas texture
    int x, y;             // Pixel coordinates in atlas
    int width, height;    // Glyph dimensions

    GlyphUV() = default;
    GlyphUV(float u0, float v0, float u1, float v1, int x, int y, int w, int h)
        : u0(u0)
        , v0(v0)
        , u1(u1)
        , v1(v1)
        , x(x)
        , y(y)
        , width(w)
        , height(h)
    {
    }
  };

  /**
   * A texture atlas containing SDF glyphs
   */
  class SDFAtlas
  {
  public:
    explicit SDFAtlas(int width = 512, int height = 512);
    ~SDFAtlas() = default;

    // Non-copyable but movable
    SDFAtlas(const SDFAtlas &) = delete;
    SDFAtlas &operator=(const SDFAtlas &) = delete;
    SDFAtlas(SDFAtlas &&) = default;
    SDFAtlas &operator=(SDFAtlas &&) = default;

    /**
     * Add a glyph to the atlas
     * @param glyph The SDF glyph to add
     * @return GlyphUV coordinates if successful, nullptr if no space
     */
    std::unique_ptr<GlyphUV> addGlyph(const SDFGlyph &glyph);

    /**
     * Add multiple glyphs to the atlas
     * @param glyphs Vector of SDF glyphs to add
     * @return Map of codepoint to GlyphUV coordinates
     */
    std::unordered_map<uint32_t, std::unique_ptr<GlyphUV>> addGlyphs(
      const std::vector<std::unique_ptr<SDFGlyph>> &glyphs);

    /**
     * Get atlas texture data (single channel)
     */
    const std::vector<uint8_t> &getTextureData() const
    {
      return textureData_;
    }

    /**
     * Get atlas dimensions
     */
    int getWidth() const
    {
      return width_;
    }
    int getHeight() const
    {
      return height_;
    }

    /**
     * Check if atlas has space for a glyph of given dimensions
     */
    bool hasSpace(int width, int height) const;

    /**
     * Get current fill ratio (0.0 to 1.0)
     */
    float getFillRatio() const;

    /**
     * Clear the atlas
     */
    void clear();

  private:
    int width_;
    int height_;
    std::vector<uint8_t> textureData_;

    // Simple bin packing state
    struct PackingNode
    {
      int x, y, width, height;
      bool used;
      std::unique_ptr<PackingNode> right;
      std::unique_ptr<PackingNode> down;

      PackingNode(int x, int y, int w, int h)
          : x(x)
          , y(y)
          , width(w)
          , height(h)
          , used(false)
      {
      }
    };

    std::unique_ptr<PackingNode> root_;
    int usedArea_;

    // Bin packing methods
    PackingNode *findNode(PackingNode *node, int width, int height);
    PackingNode *splitNode(PackingNode *node, int width, int height);
    void copyGlyphData(const SDFGlyph &glyph, int x, int y);
  };

  /**
   * Factory class for creating SDF atlases for text strings
   */
  class SDFAtlasBuilder
  {
  public:
    explicit SDFAtlasBuilder(const SDFParams &params = SDFParams(),
                             int atlasWidth = 512,
                             int atlasHeight = 512);

    /**
     * Create an atlas for a text string
     * @param text UTF-8 encoded text string
     * @return Atlas with glyph UV mapping
     */
    struct AtlasResult
    {
      std::unique_ptr<SDFAtlas> atlas;
      std::unordered_map<uint32_t, std::unique_ptr<GlyphUV>> glyphUVs;
    };

    AtlasResult createAtlasForText(const std::string &text);

    /**
     * Get/set SDF parameters
     */
    const SDFParams &getSDFParams() const
    {
      return sdfParams_;
    }
    void setSDFParams(const SDFParams &params)
    {
      sdfParams_ = params;
    }

  private:
    SDFParams sdfParams_;
    int atlasWidth_;
    int atlasHeight_;
    TinySDF sdfGenerator_;

    // Helper to extract unique codepoints from UTF-8 text
    std::vector<uint32_t> extractCodepoints(const std::string &text);
  };
}