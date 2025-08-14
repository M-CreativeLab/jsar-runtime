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

  // Compressed splat structure: 1 texel (4 floats) per splat
  // R=compressed_pos.xyz (3x8-bit), G=compressed_scale.xyz (3x8-bit log2),
  // B=compressed_quat, A=compressed_color
  struct CompressedSplat
  {
    float texel[4]; // compressed_pos, compressed_scale, compressed_quat, compressed_color
  };

  // Position and scale normalization parameters
  struct SplatNormalizationParams
  {
    float posMin[3];   // Minimum position values for normalization
    float posMax[3];   // Maximum position values for normalization
    float scaleMin[3]; // Minimum log2 scale values for normalization
    float scaleMax[3]; // Maximum log2 scale values for normalization
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

    // Compress position (x,y,z) to single float using 3x8-bit packing
    // Positions are normalized to [0,1] range using provided min/max bounds
    float compressPosition(float x, float y, float z, const float minPos[3], const float maxPos[3]);

    // Decompress position from single float to (x,y,z)
    std::array<float, 3> decompressPosition(float compressed, const float minPos[3], const float maxPos[3]);

    // Compress scale (x,y,z) to single float using log2 compression + 3x8-bit packing
    // Scales are log2-compressed and normalized to [0,1] range using provided min/max log bounds
    float compressScale(float x, float y, float z, const float minLogScale[3], const float maxLogScale[3]);

    // Decompress scale from single float to (x,y,z)
    std::array<float, 3> decompressScale(float compressed, const float minLogScale[3], const float maxLogScale[3]);

    // Convert from Splat struct to CompressedSplat with normalization parameters
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
      float a,                                   // color + opacity
      const SplatNormalizationParams &normParams // normalization parameters
    );
  }
}