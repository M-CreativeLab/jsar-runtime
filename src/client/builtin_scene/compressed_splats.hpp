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
  // word0: pos.xy as half-floats (16 bits each)
  // word1: pos.z as half-float (16 bits) + upper 16 bits of quaternion
  // word2: lower 8 bits of quaternion + scale.xyz (8 bits each)
  // word3: RGBA color (8 bits each)
  struct CompressedSplat
  {
    float word[4]; // word0, word1, word2, word3
  };

  // Scale normalization parameters (for log-scale range)
  struct SplatNormalizationParams
  {
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

    // Compress quaternion using octahedral mapping (24-bit)
    // Returns packed value as float containing the 24-bit compressed quaternion
    float compressQuaternionOct(float x, float y, float z, float w);

    // Decompress quaternion from octahedral mapping (24-bit) to (x,y,z,w)
    std::array<float, 4> decompressQuaternionOct(float compressed);

    // Compress RGBA color to single float using normalized encoding
    float compressColor(float r, float g, float b, float a);

    // Decompress color from single float to RGBA
    std::array<float, 4> decompressColor(float compressed);

    // Compress position (x,y,z) using half-floats (no normalization needed)
    std::array<float, 2> compressPositionHalf(float x, float y, float z);

    // Decompress position from half-floats to (x,y,z)
    std::array<float, 3> decompressPositionHalf(float word0, float word1);

    // Compress scale (x,y,z) using log2 compression + 8-bit packing
    float compressScaleLog(float x, float y, float z, const float minLogScale[3], const float maxLogScale[3]);

    // Decompress scale from 8-bit log values to (x,y,z)
    std::array<float, 3> decompressScaleLog(float word2, const float minLogScale[3], const float maxLogScale[3]);

    // Convert from Splat struct to CompressedSplat with new format
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