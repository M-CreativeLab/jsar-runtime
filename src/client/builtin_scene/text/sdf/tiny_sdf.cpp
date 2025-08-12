#include "./tiny_sdf.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include "include/core/SkPixmap.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"

namespace builtin_scene::text::sdf
{
  TinySDF::TinySDF(const SDFParams &params)
      : params_(params)
  {
  }

  bool TinySDF::generateFromBitmapInPlace(SkBitmap &bitmap)
  {
    if (bitmap.empty())
      return false;

    auto alphaData = extractAlphaChannel(bitmap);
    if (alphaData.empty())
      return false;

    auto sdfData = generateSDF(alphaData, bitmap.width(), bitmap.height());
    if (sdfData.empty())
      return false;

    return updateAlphaChannelWithSDF(bitmap, sdfData);
  }

  bool TinySDF::generateFromCanvasInPlace(SkCanvas *canvas)
  {
    if (!canvas)
      return false;

    // Get the surface from the canvas to extract bitmap
    auto surface = canvas->getSurface();
    if (!surface)
      return false;

    SkBitmap bitmap;
    if (!surface->makeImageSnapshot()->asLegacyBitmap(&bitmap))
      return false;

    // Generate SDF in place on the bitmap
    bool success = generateFromBitmapInPlace(bitmap);
    if (success)
    {
      // Update the canvas with the modified bitmap
      canvas->clear(SK_ColorTRANSPARENT);
      canvas->drawImage(bitmap.asImage(), 0, 0);
    }

    return success;
  }

  bool TinySDF::updateAlphaChannelWithSDF(SkBitmap &bitmap, const std::vector<uint8_t> &sdfData)
  {
    SkPixmap pixmap;
    if (!bitmap.peekPixels(&pixmap))
      return false;

    int width = bitmap.width();
    int height = bitmap.height();

    if (sdfData.size() != static_cast<size_t>(width * height))
      return false;

    // Make the bitmap writable
    if (!bitmap.isImmutable())
    {
      bitmap.setImmutable(); // This actually makes it mutable if it was already immutable
    }

    // Get writable pixels
    void *pixels = bitmap.getPixels();
    if (!pixels)
      return false;

    SkColorType colorType = pixmap.colorType();
    int rowBytes = pixmap.rowBytes();

    for (int y = 0; y < height; ++y)
    {
      for (int x = 0; x < width; ++x)
      {
        int idx = y * width + x;
        uint8_t sdfValue = sdfData[idx];

        switch (colorType)
        {
        case kAlpha_8_SkColorType:
        {
          uint8_t *row = static_cast<uint8_t *>(pixels) + y * rowBytes;
          row[x] = sdfValue;
          break;
        }
        case kRGBA_8888_SkColorType:
        {
          uint32_t *row = reinterpret_cast<uint32_t *>(static_cast<uint8_t *>(pixels) + y * rowBytes);
          uint32_t pixel = row[x];
          // Clear existing alpha and set SDF value as alpha, preserve RGB as 0
          row[x] = (pixel & 0x00FFFFFF) | (static_cast<uint32_t>(sdfValue) << 24);
          // Set RGB channels to 0 for SDF texture
          row[x] = static_cast<uint32_t>(sdfValue) << 24;
          break;
        }
        case kBGRA_8888_SkColorType:
        {
          uint32_t *row = reinterpret_cast<uint32_t *>(static_cast<uint8_t *>(pixels) + y * rowBytes);
          // For BGRA, alpha is still in the top 8 bits
          row[x] = static_cast<uint32_t>(sdfValue) << 24;
          break;
        }
        default:
        {
          // For other formats, set pixel color with SDF value as alpha
          SkColor sdfColor = SkColorSetA(SK_ColorBLACK, sdfValue);
          *bitmap.getAddr32(x, y) = sdfColor;
          break;
        }
        }
      }
    }

    return true;
  }

  std::vector<uint8_t> TinySDF::extractAlphaChannel(const SkBitmap &bitmap)
  {
    SkPixmap pixmap;
    if (!bitmap.peekPixels(&pixmap))
      return {};

    int width = bitmap.width();
    int height = bitmap.height();
    std::vector<uint8_t> alphaData(width * height);

    // Extract alpha channel based on color type
    SkColorType colorType = pixmap.colorType();
    const void *pixels = pixmap.addr();
    int rowBytes = pixmap.rowBytes();

    for (int y = 0; y < height; ++y)
    {
      for (int x = 0; x < width; ++x)
      {
        uint8_t alpha = 0;

        switch (colorType)
        {
        case kAlpha_8_SkColorType:
        {
          const uint8_t *row = static_cast<const uint8_t *>(pixels) + y * rowBytes;
          alpha = row[x];
          break;
        }
        case kRGBA_8888_SkColorType:
        case kBGRA_8888_SkColorType:
        {
          const uint32_t *row = reinterpret_cast<const uint32_t *>(static_cast<const uint8_t *>(pixels) + y * rowBytes);
          uint32_t pixel = row[x];
          alpha = (pixel >> 24) & 0xFF; // Extract alpha from ARGB
          break;
        }
        case kRGB_888x_SkColorType:
        {
          alpha = 255; // No alpha channel, assume fully opaque
          break;
        }
        default:
        {
          // For other formats, use Skia's color extraction
          SkColor color = pixmap.getColor(x, y);
          alpha = SkColorGetA(color);
          break;
        }
        }

        alphaData[y * width + x] = alpha;
      }
    }

    return alphaData;
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