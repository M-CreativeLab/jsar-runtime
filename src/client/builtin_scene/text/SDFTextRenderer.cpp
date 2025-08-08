#include "./SDFTextRenderer.hpp"
#include <client/graphics/webgl_context.hpp>
#include <client/per_process.hpp>
#include <skia/include/core/SkTypeface.h>
#include <skia/include/core/SkFont.h>
#include <iostream>
#include <algorithm>

namespace builtin_scene::text
{
  SDFTextRenderer::SDFTextRenderer()
      : atlasBuilder_(sdf::SDFParams(), 512, 512)
  {
  }

  SDFTextRenderer::~SDFTextRenderer()
  {
    // GPU texture cleanup would happen here if we tracked all textures
  }

  void SDFTextRenderer::updateSDFText(SDFText2d &sdfText)
  {
    if (!sdfText.isDirty)
      return;

    // Create cache key
    auto cacheKey = createCacheKey(sdfText);

    // Check cache first
    auto &cache = sdf::GlobalSDFCache::getInstance();
    auto cachedAtlas = cache.get(cacheKey);

    if (cachedAtlas)
    {
      // Use cached atlas
      sdfText.atlasTextureId = uploadAtlasToGPU(*cachedAtlas->atlas);

      // Copy UV mappings (we need to do deep copy since cache owns the originals)
      sdfText.glyphUVs.clear();
      for (const auto &[codepoint, uv] : cachedAtlas->glyphUVs)
      {
        if (uv)
        {
          sdfText.glyphUVs[codepoint] = std::make_unique<sdf::GlyphUV>(*uv);
        }
      }
    }
    else
    {
      // Generate new atlas
      auto atlasResult = atlasBuilder_.createAtlasForText(sdfText.content);

      if (atlasResult.atlas)
      {
        // Upload to GPU
        sdfText.atlasTextureId = uploadAtlasToGPU(*atlasResult.atlas);

        // Move UV mappings
        sdfText.glyphUVs = std::move(atlasResult.glyphUVs);

        // Cache the result (need to create copies for cache)
        auto atlasForCache = std::make_unique<sdf::SDFAtlas>(512, 512);
        // Copy atlas data
        *atlasForCache = std::move(*atlasResult.atlas);

        // Create copies of UV mappings for cache
        std::unordered_map<uint32_t, std::unique_ptr<sdf::GlyphUV>> uvCopies;
        for (const auto &[codepoint, uv] : sdfText.glyphUVs)
        {
          if (uv)
          {
            uvCopies[codepoint] = std::make_unique<sdf::GlyphUV>(*uv);
          }
        }

        cache.put(cacheKey, std::move(atlasForCache), std::move(uvCopies));
      }
    }

    sdfText.isDirty = false;
  }

  SDFTextRenderer::TextMeshData SDFTextRenderer::generateTextMesh(const SDFText2d &sdfText,
                                                                  const std::string &text,
                                                                  float lineHeight)
  {
    TextMeshData meshData;

    if (text.empty() || sdfText.glyphUVs.empty())
      return meshData;

    // Get glyph metrics for positioning
    auto glyphMetrics = getGlyphMetrics(text);

    float x = 0.0f;
    float y = 0.0f;
    const float scale = 1.0f / sdfText.fontSize; // Scale to normalize to unit size

    // Process each character in the text
    const char *str = text.c_str();
    size_t len = text.length();

    for (size_t i = 0; i < len;)
    {
      // Simple UTF-8 to UTF-32 conversion (basic implementation)
      uint32_t codepoint = 0;
      unsigned char c = str[i];

      if (c < 0x80)
      {
        codepoint = c;
        i += 1;
      }
      else if ((c & 0xE0) == 0xC0 && i + 1 < len)
      {
        codepoint = ((c & 0x1F) << 6) | (str[i + 1] & 0x3F);
        i += 2;
      }
      else if ((c & 0xF0) == 0xE0 && i + 2 < len)
      {
        codepoint = ((c & 0x0F) << 12) | ((str[i + 1] & 0x3F) << 6) | (str[i + 2] & 0x3F);
        i += 3;
      }
      else if ((c & 0xF8) == 0xF0 && i + 3 < len)
      {
        codepoint = ((c & 0x07) << 18) | ((str[i + 1] & 0x3F) << 12) |
                    ((str[i + 2] & 0x3F) << 6) | (str[i + 3] & 0x3F);
        i += 4;
      }
      else
      {
        i += 1;
        continue;
      }

      // Handle newlines
      if (codepoint == '\n')
      {
        x = 0.0f;
        y -= lineHeight;
        continue;
      }

      // Find glyph UV and metrics
      auto uvIt = sdfText.glyphUVs.find(codepoint);
      auto metricsIt = glyphMetrics.find(codepoint);

      if (uvIt != sdfText.glyphUVs.end() && uvIt->second &&
          metricsIt != glyphMetrics.end())
      {
        const auto &uv = *uvIt->second;
        const auto &metrics = metricsIt->second;

        // Calculate quad positions
        float left = x + metrics.left * scale;
        float top = y + metrics.top * scale;
        float right = left + metrics.width * scale;
        float bottom = top - metrics.height * scale;

        // Add quad vertices (2 triangles)
        uint32_t baseIndex = meshData.vertices.size() / 3;

        // Vertex positions (CCW winding)
        meshData.vertices.insert(meshData.vertices.end(), {
                                                            left, bottom, 0.0f, // Bottom-left
                                                            right,
                                                            bottom,
                                                            0.0f, // Bottom-right
                                                            right,
                                                            top,
                                                            0.0f, // Top-right
                                                            left,
                                                            top,
                                                            0.0f // Top-left
                                                          });

        // UV coordinates
        meshData.uvs.insert(meshData.uvs.end(), {
                                                  uv.u0, uv.v1, // Bottom-left
                                                  uv.u1,
                                                  uv.v1, // Bottom-right
                                                  uv.u1,
                                                  uv.v0, // Top-right
                                                  uv.u0,
                                                  uv.v0 // Top-left
                                                });

        // Indices for 2 triangles
        meshData.indices.insert(meshData.indices.end(), {
                                                          baseIndex, baseIndex + 1, baseIndex + 2, // First triangle
                                                          baseIndex,
                                                          baseIndex + 2,
                                                          baseIndex + 3 // Second triangle
                                                        });

        // Advance cursor
        x += metrics.advance * scale;
      }
    }

    return meshData;
  }

  void SDFTextRenderer::setSDFParams(const sdf::SDFParams &params)
  {
    sdfParams_ = params;
    atlasBuilder_.setSDFParams(params);
  }

  const sdf::SDFParams &SDFTextRenderer::getSDFParams() const
  {
    return sdfParams_;
  }

  sdf::SDFCache::CacheStats SDFTextRenderer::getCacheStats() const
  {
    return sdf::GlobalSDFCache::getInstance().getStats();
  }

  uint32_t SDFTextRenderer::uploadAtlasToGPU(const sdf::SDFAtlas &atlas)
  {
    auto glContext = glContext_.lock();
    if (!glContext)
    {
      // Try to get context from the global state
      glContext = TrClientContextPerProcess::Get()->getHostWebGLContext();
      if (!glContext)
        return 0;
      glContext_ = glContext;
    }

    // Create OpenGL texture
    auto textureId = glContext->createTexture();
    if (!textureId.has_value())
      return 0;

    glContext->bindTexture(GL_TEXTURE_2D, textureId.value());

    // Upload texture data
    glContext->texImage2D(GL_TEXTURE_2D, 0, GL_R8, atlas.getWidth(), atlas.getHeight(), 0, GL_RED, GL_UNSIGNED_BYTE, atlas.getTextureData().data());

    // Set texture parameters for SDF rendering
    glContext->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glContext->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glContext->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glContext->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glContext->bindTexture(GL_TEXTURE_2D, 0);

    return textureId.value();
  }

  void SDFTextRenderer::deleteGPUTexture(uint32_t textureId)
  {
    auto glContext = glContext_.lock();
    if (glContext && textureId != 0)
    {
      glContext->deleteTexture(textureId);
    }
  }

  sdf::SDFCacheKey SDFTextRenderer::createCacheKey(const SDFText2d &sdfText) const
  {
    return sdf::SDFCacheKey(
      sdfText.fontFamily,
      sdfText.fontSize,
      sdfText.fontWeight,
      sdfText.fontStyle,
      sdfText.devicePixelRatio,
      sdfText.content);
  }

  std::unordered_map<uint32_t, SDFTextRenderer::GlyphMetrics>
  SDFTextRenderer::getGlyphMetrics(const std::string &text)
  {
    std::unordered_map<uint32_t, GlyphMetrics> metrics;

    // Get font manager and create font
    auto fontManager = TrClientContextPerProcess::Get()->getFontCacheManager();
    auto typeface = fontManager->matchFamilyStyle("", SkFontStyle());
    if (!typeface)
    {
      typeface = SkTypeface::MakeDefault();
    }

    SkFont font(typeface, static_cast<float>(sdfParams_.fontSize));

    // Extract unique codepoints and get their metrics
    std::vector<uint32_t> codepoints;
    const char *str = text.c_str();
    size_t len = text.length();

    for (size_t i = 0; i < len;)
    {
      uint32_t codepoint = 0;
      unsigned char c = str[i];

      if (c < 0x80)
      {
        codepoint = c;
        i += 1;
      }
      else if ((c & 0xE0) == 0xC0 && i + 1 < len)
      {
        codepoint = ((c & 0x1F) << 6) | (str[i + 1] & 0x3F);
        i += 2;
      }
      else if ((c & 0xF0) == 0xE0 && i + 2 < len)
      {
        codepoint = ((c & 0x0F) << 12) | ((str[i + 1] & 0x3F) << 6) | (str[i + 2] & 0x3F);
        i += 3;
      }
      else if ((c & 0xF8) == 0xF0 && i + 3 < len)
      {
        codepoint = ((c & 0x07) << 18) | ((str[i + 1] & 0x3F) << 12) |
                    ((str[i + 2] & 0x3F) << 6) | (str[i + 3] & 0x3F);
        i += 4;
      }
      else
      {
        i += 1;
        continue;
      }

      if (metrics.find(codepoint) == metrics.end())
      {
        SkGlyphID glyphId = font.unicharToGlyph(codepoint);
        if (glyphId != 0)
        {
          SkRect bounds;
          font.getBounds(&glyphId, 1, &bounds, nullptr);

          SkScalar advance;
          font.getWidths(&glyphId, 1, &advance);

          GlyphMetrics gm;
          gm.advance = advance;
          gm.left = bounds.left();
          gm.top = bounds.top();
          gm.width = bounds.width();
          gm.height = bounds.height();

          metrics[codepoint] = gm;
        }
      }
    }

    return metrics;
  }

  // SDFTextSystem implementation
  SDFTextSystem::SDFTextSystem()
      : renderer_(std::make_unique<SDFTextRenderer>())
  {
  }

  void SDFTextSystem::onExecute()
  {
    // Update all SDF text components
    getScene().view<SDFText2d>().each([this](auto entity, SDFText2d &sdfText)
                                      {
      if (sdfText.isDirty)
      {
        renderer_->updateSDFText(sdfText);
      } });
  }
}