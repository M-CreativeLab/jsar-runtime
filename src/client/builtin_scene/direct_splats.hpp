#pragma once

#include <cstdint>
#include <array>
#include <cmath>
#include <algorithm>

namespace builtin_scene
{
  // Direct splat storage constants (power of 2 textures)
  constexpr uint32_t SPLAT_TEX_WIDTH_BITS = 10u; // 1024 width
  constexpr uint32_t SPLAT_TEX_WIDTH = 1u << SPLAT_TEX_WIDTH_BITS;
  constexpr uint32_t SPLAT_TEX_WIDTH_MASK = SPLAT_TEX_WIDTH - 1u;

  // Simple splat structure for direct storage (no packing needed)
  struct DirectSplat
  {
    float position[3];   // XYZ position
    float color[4];      // RGBA color and opacity
    float scale[3];      // XYZ scale values
    float quaternion[4]; // XYZW rotation quaternion
  };

  // Utility functions for direct splat storage
  namespace direct_splat_utils
  {
    // Calculate texture coordinates for a splat index using bit operations
    std::array<uint32_t, 2> getSplatTexCoord(uint32_t splatIndex);

    // Calculate required texture size for given number of splats
    std::array<uint32_t, 3> getTextureSize(uint32_t numSplats);

    // Convert from Splat struct to DirectSplat
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
    );
  }
}