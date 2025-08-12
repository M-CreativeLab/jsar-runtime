#include "direct_splats.hpp"
#include <common/debug.hpp>

namespace builtin_scene::direct_splat_utils
{
  using namespace std;

  std::array<uint32_t, 2> getSplatTexCoord(uint32_t splatIndex)
  {
    uint32_t x = splatIndex & SPLAT_TEX_WIDTH_MASK;  // x = index & (1024 - 1)
    uint32_t y = splatIndex >> SPLAT_TEX_WIDTH_BITS; // y = index >> 10
    return {x, y};
  }

  std::array<uint32_t, 3> getTextureSize(uint32_t numSplats)
  {
    // Calculate how many rows we need for the 2D texture
    uint32_t numRows = (numSplats + SPLAT_TEX_WIDTH - 1) / SPLAT_TEX_WIDTH;

    // Round up to nearest power of 2 for height (optional optimization)
    uint32_t height = 1;
    while (height < numRows)
    {
      height <<= 1;
    }

    uint32_t maxSplats = SPLAT_TEX_WIDTH * height;

    return {SPLAT_TEX_WIDTH, height, maxSplats};
  }

  DirectSplat convertSplat(
    float px, float py, float pz, // position
    float sx,
    float sy,
    float sz, // scale
    float qx,
    float qy,
    float qz,
    float qw, // quaternion
    float r,
    float g,
    float b,
    float a // color + opacity
  )
  {
    DirectSplat direct;

    // Position
    direct.position[0] = px;
    direct.position[1] = py;
    direct.position[2] = pz;

    // Color
    direct.color[0] = r;
    direct.color[1] = g;
    direct.color[2] = b;
    direct.color[3] = a;

    // Scale
    direct.scale[0] = sx;
    direct.scale[1] = sy;
    direct.scale[2] = sz;

    // Quaternion
    direct.quaternion[0] = qx;
    direct.quaternion[1] = qy;
    direct.quaternion[2] = qz;
    direct.quaternion[3] = qw;

    return direct;
  }
}