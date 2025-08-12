#include "./tiny_sdf.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace builtin_scene::text::sdf
{
  static constexpr double INF = 1e20;

  TinySDF::TinySDF(const SDFParams &params)
      : params_(params)
  {
  }

  bool TinySDF::generateFromPixelsInPlace(unsigned char *pixels, int width, int height)
  {
    if (!pixels || width <= 0 || height <= 0)
      return false;

    int len = width * height;

    // Extract alpha channel from pixel data
    auto alphaData = extractAlphaChannel(pixels, width, height);
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

    // Generate SDF and update pixel alpha channel
    return generateSDFFromGrids(pixels, width, height, gridOuter, gridInner);
  }

  std::vector<uint8_t> TinySDF::extractAlphaChannel(const unsigned char *pixels, int width, int height)
  {
    if (!pixels || width <= 0 || height <= 0)
      return {};

    std::vector<uint8_t> alphaData(width * height);

    // Assume RGBA format (4 bytes per pixel)
    for (int y = 0; y < height; ++y)
    {
      for (int x = 0; x < width; ++x)
      {
        const int pixelIndex = (y * width + x) * 4;
        const uint8_t alpha = pixels[pixelIndex + 3]; // Alpha is 4th component
        alphaData[y * width + x] = alpha;
      }
    }

    return alphaData;
  }

  bool TinySDF::generateSDFFromGrids(unsigned char *pixels, int width, int height, const std::vector<double> &gridOuter, const std::vector<double> &gridInner)
  {
    if (!pixels || width <= 0 || height <= 0)
      return false;

    int len = width * height;

    if (gridOuter.size() != static_cast<size_t>(len) ||
        gridInner.size() != static_cast<size_t>(len))
      return false;

    // Update only the alpha channel, preserving RGB channels
    for (int i = 0; i < len; i++)
    {
      // Combine outer and inner distance fields to create signed distance
      const double d = std::sqrt(gridOuter[i]) - std::sqrt(gridInner[i]);

      // Convert to SDF value in [0, 255] range
      const double sdfValue = 255.0 - 255.0 * (d / params_.radius + params_.cutoff);
      const uint8_t sdfByte = static_cast<uint8_t>(std::round(std::clamp(sdfValue, 0.0, 255.0)));

      // Update only alpha channel (4th component in RGBA)
      const int pixelIndex = i * 4;
      pixels[pixelIndex + 3] = sdfByte;
      // RGB channels (pixels[pixelIndex], pixels[pixelIndex + 1], pixels[pixelIndex + 2]) remain unchanged
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