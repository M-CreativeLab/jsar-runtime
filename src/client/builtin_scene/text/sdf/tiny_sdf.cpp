#include "./tiny_sdf.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkPaint.h>
#include <skia/include/core/SkTypeface.h>
#include <skia/include/core/SkFont.h>
#include <skia/include/core/SkBitmap.h>
#include <skia/include/core/SkPixmap.h>
#include <client/per_process.hpp>

namespace builtin_scene::text::sdf
{
  TinySDF::TinySDF(const SDFParams &params)
      : params_(params)
  {
  }

  void TinySDF::setParams(const SDFParams &params)
  {
    params_ = params;
  }

  std::unique_ptr<SDFGlyph> TinySDF::generateGlyph(uint32_t codepoint)
  {
    int width, height, left, top, advance;
    auto bitmap = rasterizeGlyph(codepoint, width, height, left, top, advance);

    if (bitmap.empty())
      return nullptr;

    auto sdfData = generateSDF(bitmap, width, height);

    auto glyph = std::make_unique<SDFGlyph>(codepoint, width, height, left, top, advance);
    glyph->data = std::move(sdfData);

    return glyph;
  }

  std::vector<std::unique_ptr<SDFGlyph>> TinySDF::generateGlyphs(const std::vector<uint32_t> &codepoints)
  {
    std::vector<std::unique_ptr<SDFGlyph>> result;
    result.reserve(codepoints.size());

    for (uint32_t codepoint : codepoints)
    {
      auto glyph = generateGlyph(codepoint);
      if (glyph)
        result.push_back(std::move(glyph));
    }

    return result;
  }

  std::vector<uint8_t> TinySDF::rasterizeGlyph(uint32_t codepoint, int &width, int &height, int &left, int &top, int &advance)
  {
    // Get default typeface from the font cache manager
    auto fontManager = TrClientContextPerProcess::Get()->getFontCacheManager();
    auto typeface = fontManager->matchFamilyStyle("", SkFontStyle());

    if (!typeface)
    {
      // Fallback to default typeface
      typeface = SkTypeface::MakeDefault();
    }

    SkFont font(typeface, static_cast<float>(params_.fontSize));
    font.setHinting(SkFontHinting::kNone);
    font.setSubpixel(false);

    // Get glyph metrics
    SkGlyphID glyphId = font.unicharToGlyph(codepoint);
    if (glyphId == 0)
      return {}; // Glyph not found

    // Get glyph bounds and metrics
    SkRect bounds;
    font.getBounds(&glyphId, 1, &bounds, nullptr);

    SkScalar advanceWidth;
    font.getWidths(&glyphId, 1, &advanceWidth);
    advance = static_cast<int>(std::round(advanceWidth));

    // Calculate rendering dimensions with buffer
    int glyphWidth = static_cast<int>(std::ceil(bounds.width())) + 2 * params_.buffer;
    int glyphHeight = static_cast<int>(std::ceil(bounds.height())) + 2 * params_.buffer;

    if (glyphWidth <= 0 || glyphHeight <= 0)
      return {};

    width = glyphWidth;
    height = glyphHeight;
    left = static_cast<int>(bounds.left()) - params_.buffer;
    top = static_cast<int>(bounds.top()) - params_.buffer;

    // Create bitmap and canvas for rasterization
    SkBitmap bitmap;
    bitmap.allocPixels(SkImageInfo::MakeA8(width, height));
    bitmap.eraseColor(SK_ColorTRANSPARENT);

    SkCanvas canvas(bitmap);
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(SK_ColorWHITE);

    // Position the glyph
    float x = params_.buffer - bounds.left();
    float y = params_.buffer - bounds.top();

    canvas.drawSimpleText(&codepoint, sizeof(codepoint), SkTextEncoding::kUTF32, x, y, font, paint);

    // Extract bitmap data
    SkPixmap pixmap;
    if (!bitmap.peekPixels(&pixmap))
      return {};

    const uint8_t *pixels = static_cast<const uint8_t *>(pixmap.addr());
    int stride = pixmap.rowBytes();

    std::vector<uint8_t> result(width * height);
    for (int y = 0; y < height; ++y)
    {
      for (int x = 0; x < width; ++x)
      {
        result[y * width + x] = pixels[y * stride + x];
      }
    }

    return result;
  }

  std::vector<uint8_t> TinySDF::generateSDF(const std::vector<uint8_t> &bitmap, int width, int height)
  {
    std::vector<uint8_t> sdf(width * height);
    const double maxDistance = params_.radius;

    for (int y = 0; y < height; ++y)
    {
      for (int x = 0; x < width; ++x)
      {
        int idx = y * width + x;
        uint8_t alpha = bitmap[idx];
        bool inside = alpha > static_cast<uint8_t>(params_.cutoff * 255);

        double distance = distanceToEdge(bitmap, width, height, x, y, inside);
        distance = std::min(distance, maxDistance);

        // Normalize distance to [0, 1] range
        double normalizedDistance = (distance + maxDistance) / (2.0 * maxDistance);
        sdf[idx] = static_cast<uint8_t>(std::round(normalizedDistance * 255.0));
      }
    }

    return sdf;
  }

  double TinySDF::distanceToEdge(const std::vector<uint8_t> &bitmap, int width, int height, int x, int y, bool inside)
  {
    double minDistance = std::numeric_limits<double>::max();
    const int searchRadius = params_.radius;
    const double cutoffAlpha = params_.cutoff * 255.0;

    // Search in a square around the current pixel
    for (int dy = -searchRadius; dy <= searchRadius; ++dy)
    {
      for (int dx = -searchRadius; dx <= searchRadius; ++dx)
      {
        int nx = x + dx;
        int ny = y + dy;

        // Skip out-of-bounds pixels
        if (nx < 0 || nx >= width || ny < 0 || ny >= height)
          continue;

        int idx = ny * width + nx;
        uint8_t alpha = bitmap[idx];
        bool otherInside = alpha > cutoffAlpha;

        // If we find a pixel with different inside/outside state
        if (inside != otherInside)
        {
          double distance = std::sqrt(dx * dx + dy * dy);
          minDistance = std::min(minDistance, distance);
        }
      }
    }

    // If no edge found within search radius, return max distance
    if (minDistance == std::numeric_limits<double>::max())
      minDistance = searchRadius;

    // Return signed distance (negative for inside, positive for outside)
    return inside ? -minDistance : minDistance;
  }
}