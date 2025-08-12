#include "./tiny_sdf.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <skia/include/core/SkPixmap.h>
#include <skia/include/core/SkImageInfo.h>
#include <skia/include/core/SkSurface.h>

namespace builtin_scene::text::sdf
{
  static constexpr double INF = 1e20;

  TinySDF::TinySDF(const SDFParams &params)
      : params_(params)
  {
  }

  bool TinySDF::generateFromBitmapInPlace(SkBitmap &bitmap)
  {
    if (bitmap.empty())
      return false;

    int width = bitmap.width();
    int height = bitmap.height();
    int len = width * height;

    // Extract alpha channel from bitmap
    auto alphaData = extractAlphaChannel(bitmap);
    if (alphaData.empty())
      return false;

    // Create temporary grids for distance transform
    std::vector<double> gridOuter(len, INF);
    std::vector<double> gridInner(len, 0.0);

    // Initialize grids based on alpha values
    for (int y = 0; y < height; y++)
    {
      for (int x = 0; x < width; x++)
      {
        const double alpha = alphaData[y * width + x] / 255.0; // Normalize to [0,1]
        if (alpha == 0.0)
          continue; // Skip empty pixels

        const int j = y * width + x;

        if (alpha == 1.0)
        {
          // Fully drawn pixels
          gridOuter[j] = 0.0;
          gridInner[j] = INF;
        }
        else
        {
          // Anti-aliased pixels
          const double d = 0.5 - alpha;
          gridOuter[j] = d > 0 ? d * d : 0.0;
          gridInner[j] = d < 0 ? d * d : 0.0;
        }
      }
    }

    // Create temporary arrays for EDT algorithm
    std::vector<double> f(std::max(width, height));
    std::vector<double> z(std::max(width, height) + 1);
    std::vector<int> v(std::max(width, height));

    // Apply Euclidean Distance Transform
    edt(gridOuter, 0, 0, width, height, width, f, v, z);
    edt(gridInner, 0, 0, width, height, width, f, v, z);

    // Generate SDF and update bitmap alpha channel
    return generateSDFFromGrids(bitmap, gridOuter, gridInner);
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

  bool TinySDF::generateSDFFromGrids(SkBitmap &bitmap,
                                     const std::vector<double> &gridOuter,
                                     const std::vector<double> &gridInner)
  {
    SkPixmap pixmap;
    if (!bitmap.peekPixels(&pixmap))
      return false;

    int width = bitmap.width();
    int height = bitmap.height();
    int len = width * height;

    if (gridOuter.size() != static_cast<size_t>(len) ||
        gridInner.size() != static_cast<size_t>(len))
      return false;

    // Get writable pixels
    void *pixels = bitmap.getPixels();
    if (!pixels)
      return false;

    SkColorType colorType = pixmap.colorType();
    int rowBytes = pixmap.rowBytes();

    for (int i = 0; i < len; i++)
    {
      // Combine outer and inner distance fields to create signed distance
      const double d = std::sqrt(gridOuter[i]) - std::sqrt(gridInner[i]);

      // Convert to SDF value in [0, 255] range
      const double sdfValue = 255.0 - 255.0 * (d / params_.radius + params_.cutoff);
      const uint8_t sdfByte = static_cast<uint8_t>(std::round(std::clamp(sdfValue, 0.0, 255.0)));

      int y = i / width;
      int x = i % width;

      switch (colorType)
      {
      case kAlpha_8_SkColorType:
      {
        uint8_t *row = static_cast<uint8_t *>(pixels) + y * rowBytes;
        row[x] = sdfByte;
        break;
      }
      case kRGBA_8888_SkColorType:
      {
        uint32_t *row = reinterpret_cast<uint32_t *>(static_cast<uint8_t *>(pixels) + y * rowBytes);
        // Set RGB channels to 0 for SDF texture, alpha to SDF value
        row[x] = static_cast<uint32_t>(sdfByte) << 24;
        break;
      }
      case kBGRA_8888_SkColorType:
      {
        uint32_t *row = reinterpret_cast<uint32_t *>(static_cast<uint8_t *>(pixels) + y * rowBytes);
        // For BGRA, alpha is still in the top 8 bits
        row[x] = static_cast<uint32_t>(sdfByte) << 24;
        break;
      }
      default:
      {
        // For other formats, set pixel color with SDF value as alpha
        SkColor sdfColor = SkColorSetA(SK_ColorBLACK, sdfByte);
        *bitmap.getAddr32(x, y) = sdfColor;
        break;
      }
      }
    }

    return true;
  }

  void TinySDF::edt(std::vector<double> &data, int x0, int y0, int width, int height, int gridSize, std::vector<double> &f, std::vector<int> &v, std::vector<double> &z)
  {
    // Transform along columns
    for (int x = x0; x < x0 + width; x++)
    {
      edt1d(data, y0 * gridSize + x, gridSize, height, f, v, z);
    }

    // Transform along rows
    for (int y = y0; y < y0 + height; y++)
    {
      edt1d(data, y * gridSize + x0, 1, width, f, v, z);
    }
  }

  void TinySDF::edt1d(std::vector<double> &grid, int offset, int stride, int length, std::vector<double> &f, std::vector<int> &v, std::vector<double> &z)
  {
    v[0] = 0;
    z[0] = -INF;
    z[1] = INF;
    f[0] = grid[offset];

    for (int q = 1, k = 0; q < length; q++)
    {
      f[q] = grid[offset + q * stride];
      const double q2 = q * q;
      double s;

      do
      {
        const int r = v[k];
        s = (f[q] - f[r] + q2 - r * r) / (q - r) / 2.0;
      } while (s <= z[k] && --k > -1);

      k++;
      v[k] = q;
      z[k] = s;
      z[k + 1] = INF;
    }

    for (int q = 0, k = 0; q < length; q++)
    {
      while (z[k + 1] < q)
        k++;
      const int r = v[k];
      const double qr = q - r;
      grid[offset + q * stride] = f[r] + qr * qr;
    }
  }
}