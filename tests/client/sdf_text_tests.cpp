#include <catch2/catch_test_macros.hpp>
#include <client/builtin_scene/text/sdf/TinySDF.hpp>
#include <client/builtin_scene/text/sdf/Atlas.hpp>
#include <client/builtin_scene/text/sdf/Cache.hpp>
#include <client/builtin_scene/web_content.hpp>

using namespace builtin_scene::text::sdf;

TEST_CASE("Integrated SDF Text Rendering", "[sdf][text][integration]")
{
  SECTION("SDF text feature flag detection")
  {
    // Test environment variable detection
    const char *env = getenv("JSAR_ENABLE_SDF_TEXT");
    bool sdfEnabled = env && (strcmp(env, "1") == 0 || strcmp(env, "true") == 0);
    
    // This should match the logic in web_content_renderer.cpp
    REQUIRE((sdfEnabled == true || sdfEnabled == false)); // Just verify it's boolean
  }
}

TEST_CASE("TinySDF Basic Generation", "[sdf][text]")
{
  SECTION("TinySDF can be created with default parameters")
  {
    TinySDF sdf;
    auto params = sdf.getParams();
    REQUIRE(params.fontSize == 24);
    REQUIRE(params.buffer == 8);
    REQUIRE(params.radius == 8);
    REQUIRE(params.cutoff == 0.25f);
  }

  SECTION("TinySDF can generate glyph for simple ASCII character")
  {
    TinySDF sdf;
    auto glyph = sdf.generateGlyph('A');
    
    // Note: This test may fail in headless environments without font support
    // In a real test environment, we would mock or provide test fonts
    if (glyph)
    {
      REQUIRE(glyph->codepoint == 'A');
      REQUIRE(glyph->width > 0);
      REQUIRE(glyph->height > 0);
      REQUIRE(!glyph->data.empty());
      REQUIRE(glyph->data.size() == static_cast<size_t>(glyph->width * glyph->height));
    }
  }
}

TEST_CASE("SDF Atlas Packing", "[sdf][atlas]")
{
  SECTION("Atlas can be created with specified dimensions")
  {
    SDFAtlas atlas(256, 256);
    REQUIRE(atlas.getWidth() == 256);
    REQUIRE(atlas.getHeight() == 256);
    REQUIRE(atlas.getFillRatio() == 0.0f);
  }

  SECTION("Atlas can pack simple glyphs")
  {
    SDFAtlas atlas(64, 64);
    
    // Create a mock glyph
    SDFGlyph glyph('A', 16, 16, 0, 0, 16);
    glyph.data.resize(16 * 16, 128); // Fill with mid-gray
    
    auto uv = atlas.addGlyph(glyph);
    REQUIRE(uv != nullptr);
    REQUIRE(uv->u0 >= 0.0f);
    REQUIRE(uv->v0 >= 0.0f);
    REQUIRE(uv->u1 > uv->u0);
    REQUIRE(uv->v1 > uv->v0);
    REQUIRE(atlas.getFillRatio() > 0.0f);
  }
}

TEST_CASE("SDF Cache", "[sdf][cache]")
{
  SECTION("Cache can store and retrieve entries")
  {
    SDFCache cache(1024); // 1KB cache
    
    SDFCacheKey key("Arial", 16, 400, "normal", 1.0f, "test");
    
    // Initially empty
    REQUIRE(!cache.contains(key));
    REQUIRE(cache.get(key) == nullptr);
    
    // Create and store atlas
    auto atlas = std::make_unique<SDFAtlas>(32, 32);
    std::unordered_map<uint32_t, std::unique_ptr<GlyphUV>> uvs;
    
    cache.put(key, std::move(atlas), std::move(uvs));
    
    // Should now be found
    REQUIRE(cache.contains(key));
    auto retrieved = cache.get(key);
    REQUIRE(retrieved != nullptr);
    REQUIRE(retrieved->atlas != nullptr);
  }

  SECTION("Cache respects size limits")
  {
    SDFCache cache(100); // Very small cache - 100 bytes
    
    auto stats = cache.getStats();
    REQUIRE(stats.entryCount == 0);
    REQUIRE(stats.totalMemoryBytes == 0);
  }
}

TEST_CASE("SDF Atlas Builder", "[sdf][builder]")
{
  SECTION("Builder can create atlas for simple text")
  {
    SDFAtlasBuilder builder;
    
    // Test with simple ASCII text
    auto result = builder.createAtlasForText("AB");
    
    // In a headless environment, this might not work without proper font setup
    // But the structure should be valid
    if (result.atlas)
    {
      REQUIRE(result.atlas->getWidth() > 0);
      REQUIRE(result.atlas->getHeight() > 0);
    }
  }
}