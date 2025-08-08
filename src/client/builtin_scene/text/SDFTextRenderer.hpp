#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include "../ecs.hpp"
#include "../text.hpp"
#include "./sdf/TinySDF.hpp"
#include "./sdf/Atlas.hpp"
#include "./sdf/Cache.hpp"

namespace builtin_scene::text
{
  /**
   * SDF text component that extends the basic Text2d component
   */
  class SDFText2d : public ecs::Component
  {
  public:
    SDFText2d(const std::string &content = "")
        : content(content)
        , isDirty(true)
        , atlasTextureId(0)
    {
    }

    std::string content;
    bool isDirty;                                                         // Whether content has changed and needs regeneration
    uint32_t atlasTextureId;                                              // OpenGL texture ID for the SDF atlas
    std::unordered_map<uint32_t, std::unique_ptr<sdf::GlyphUV>> glyphUVs; // UV mappings for glyphs

    // Font properties (should match the layout text properties)
    std::string fontFamily = "";
    int fontSize = 24;
    int fontWeight = 400;
    std::string fontStyle = "normal";
    float devicePixelRatio = 1.0f;
  };

  /**
   * SDF text renderer that handles generation and rendering of SDF-based text
   */
  class SDFTextRenderer
  {
  public:
    SDFTextRenderer();
    ~SDFTextRenderer();

    // Non-copyable, non-movable
    SDFTextRenderer(const SDFTextRenderer &) = delete;
    SDFTextRenderer &operator=(const SDFTextRenderer &) = delete;
    SDFTextRenderer(SDFTextRenderer &&) = delete;
    SDFTextRenderer &operator=(SDFTextRenderer &&) = delete;

    /**
     * Update SDF text component if content is dirty
     * This generates new SDF atlas if needed and updates GPU texture
     */
    void updateSDFText(SDFText2d &sdfText);

    /**
     * Generate mesh quads for text rendering
     * @param sdfText The SDF text component
     * @param textLayout Layout information from the text layout system
     * @return Mesh data for rendering (vertices, indices, UVs)
     */
    struct TextMeshData
    {
      std::vector<float> vertices;   // Position data (x, y, z)
      std::vector<float> uvs;        // UV coordinates
      std::vector<uint32_t> indices; // Triangle indices
    };

    TextMeshData generateTextMesh(const SDFText2d &sdfText,
                                  const std::string &text,
                                  float lineHeight = 1.2f);

    /**
     * Set SDF generation parameters
     */
    void setSDFParams(const sdf::SDFParams &params);
    const sdf::SDFParams &getSDFParams() const;

    /**
     * Get cache statistics for debugging
     */
    sdf::SDFCache::CacheStats getCacheStats() const;

  private:
    sdf::SDFParams sdfParams_;
    sdf::SDFAtlasBuilder atlasBuilder_;

    // OpenGL context for texture operations
    std::weak_ptr<client_graphics::WebGL2Context> glContext_;

    // Helper methods
    uint32_t uploadAtlasToGPU(const sdf::SDFAtlas &atlas);
    void deleteGPUTexture(uint32_t textureId);
    sdf::SDFCacheKey createCacheKey(const SDFText2d &sdfText) const;

    // Font metrics helpers
    struct GlyphMetrics
    {
      float advance;
      float left;
      float top;
      float width;
      float height;
    };

    std::unordered_map<uint32_t, GlyphMetrics> getGlyphMetrics(const std::string &text);
  };

  /**
   * ECS System for updating SDF text rendering
   */
  class SDFTextSystem : public ecs::System
  {
  public:
    SDFTextSystem();

    void onExecute() override;

  private:
    std::unique_ptr<SDFTextRenderer> renderer_;
  };
}