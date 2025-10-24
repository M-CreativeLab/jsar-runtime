#include <algorithm>
#include <cmath>
#include <limits>
#include "./generator.hpp"

namespace endor
{
  namespace builtin_scene::text::sdf
  {
    using namespace std;

    static constexpr double INF = 1e20;

    SDFGenerator::SDFGenerator(const SDFParams &params)
        : params_(params)
    {
    }

    bool SDFGenerator::generateOnPixels(unsigned char *pixels, int width, int height)
    {
      if (!pixels || width <= 0 || height <= 0)
        return false;

      int len = width * height;

      // Create temporary grids for distance transform
      vector<double> gridOuter(len, INF);
      vector<double> gridInner(len, 0.0);

      // Initialize grids based on alpha values
      for (int y = 0; y < height; y++)
      {
        for (int x = 0; x < width; x++)
        {
          const int pixelIndex = (y * width + x) * 4;
          const double alpha = pixels[pixelIndex + 3] / 255.0; // Normalize the alpha to [0,1]
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
      vector<double> f(max(width, height));
      vector<double> z(max(width, height) + 1);
      vector<int> v(max(width, height));

      // Apply Euclidean Distance Transform
      edt(gridOuter, 0, 0, width, height, width, f, v, z);
      edt(gridInner, 0, 0, width, height, width, f, v, z);

      // Write SDF and update pixel alpha channel
      return writeFromGrids(pixels, width, height, gridOuter, gridInner);
    }

    bool SDFGenerator::writeFromGrids(unsigned char *pixels,
                                      int width,
                                      int height,
                                      const vector<double> &gridOuter,
                                      const vector<double> &gridInner)
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
        const double d = sqrt(gridOuter[i]) - sqrt(gridInner[i]);

        // Convert to SDF value in [0, 255] range
        const double sdfValue = 255.0 - 255.0 * (d / params_.radius + params_.cutoff);
        const uint8_t sdfByte = static_cast<uint8_t>(round(clamp(sdfValue, 0.0, 255.0)));

        // Update only alpha channel (4th component in RGBA)
        const int pixelIndex = i * 4;
        pixels[pixelIndex + 3] = sdfByte;
        // RGB channels (pixels[pixelIndex], pixels[pixelIndex + 1], pixels[pixelIndex + 2]) remain unchanged
      }

      return true;
    }

    void SDFGenerator::edt(vector<double> &data,
                           int x0,
                           int y0,
                           int width,
                           int height,
                           int gridSize,
                           vector<double> &f,
                           vector<int> &v,
                           vector<double> &z)
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

    void SDFGenerator::edt1d(vector<double> &grid,
                             int offset,
                             int stride,
                             int length,
                             vector<double> &f,
                             vector<int> &v,
                             vector<double> &z)
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
} // namespace endor
