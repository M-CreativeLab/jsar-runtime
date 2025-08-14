#pragma once

#include <cstdint>
#include <array>
#include <cmath>
#include <algorithm>

namespace builtin_scene
{
  // Compressed splat storage constants (power of 2 textures)
  constexpr uint32_t COMPRESSED_SPLAT_TEX_WIDTH_BITS = 10u; // 1024 width
  constexpr uint32_t COMPRESSED_SPLAT_TEX_WIDTH = 1u << COMPRESSED_SPLAT_TEX_WIDTH_BITS;
  constexpr uint32_t COMPRESSED_SPLAT_TEX_WIDTH_MASK = COMPRESSED_SPLAT_TEX_WIDTH - 1u;

  // Compressed splat structure: 2 texels (8 floats) per splat
  // Texel 0: R=pos.x, G=pos.y, B=pos.z, A=scale.x
  // Texel 1: R=scale.y, G=scale.z, B=compressed_quat, A=compressed_color
  struct CompressedSplat
  {
    float texel0[4]; // position.xyz, scale.x
    float texel1[4]; // scale.yz, compressed_quat, compressed_color
  };

  // Utility functions for compressed splat storage
  namespace compressed_splat_utils
  {
    // Calculate texture coordinates for a splat index using bit operations
    std::array<uint32_t, 2> getSplatTexCoord(uint32_t splatIndex);

    // Calculate required texture size for given number of splats
    std::array<uint32_t, 3> getTextureSize(uint32_t numSplats);

    // Compress quaternion (x,y,z,w) to single float using 3D packing
    // Stores x,y,z and sign of w, reconstructs w using unit length constraint
    float compressQuaternion(float x, float y, float z, float w);

    // Decompress quaternion from single float to (x,y,z,w)
    std::array<float, 4> decompressQuaternion(float compressed);

    // Compress RGBA color to single float using normalized encoding
    float compressColor(float r, float g, float b, float a);

    // Decompress color from single float to RGBA
    std::array<float, 4> decompressColor(float compressed);

    // Convert from Splat struct to CompressedSplat
    CompressedSplat convertSplat(
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