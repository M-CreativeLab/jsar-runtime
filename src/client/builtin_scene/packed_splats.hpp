#pragma once

#include <cstdint>
#include <array>
#include <cmath>
#include <algorithm>

namespace builtin_scene
{
  // SparkJS-compatible packed splat encoding constants
  constexpr uint32_t SPLAT_TEX_WIDTH_BITS = 11u;
  constexpr uint32_t SPLAT_TEX_HEIGHT_BITS = 11u;
  constexpr uint32_t SPLAT_TEX_DEPTH_BITS = 11u;
  constexpr uint32_t SPLAT_TEX_LAYER_BITS = SPLAT_TEX_WIDTH_BITS + SPLAT_TEX_HEIGHT_BITS;

  constexpr uint32_t SPLAT_TEX_WIDTH = 1u << SPLAT_TEX_WIDTH_BITS;
  constexpr uint32_t SPLAT_TEX_HEIGHT = 1u << SPLAT_TEX_HEIGHT_BITS;
  constexpr uint32_t SPLAT_TEX_DEPTH = 1u << SPLAT_TEX_DEPTH_BITS;

  constexpr uint32_t SPLAT_TEX_WIDTH_MASK = SPLAT_TEX_WIDTH - 1u;
  constexpr uint32_t SPLAT_TEX_HEIGHT_MASK = SPLAT_TEX_HEIGHT - 1u;
  constexpr uint32_t SPLAT_TEX_DEPTH_MASK = SPLAT_TEX_DEPTH - 1u;

  // SparkJS encoding constants
  constexpr float LN_SCALE_MIN = -12.0f;
  constexpr float LN_SCALE_MAX = 9.0f;

  // Packed splat structure - exactly 4 float values (16 bytes)
  // Each float contains bit-packed data compatible with RGBA32F texture format
  struct PackedSplat
  {
    float word0; // RGBA as 4 x uint8 (stored as float)
    float word1; // Center XY as 2 x float16 (stored as float)
    float word2; // Center Z as float16 + partial quaternion (stored as float)
    float word3; // Scales as 3 x uint8 + remaining quaternion (stored as float)
  };

  // Utility functions for packing/unpacking
  namespace packed_splat_utils
  {
    // Convert uint32 to float preserving bit pattern (for packed data)
    float uint32ToFloat(uint32_t value);

    // Convert float to uint32 preserving bit pattern (for unpacking)
    uint32_t floatToUint32(float value);

    // Pack a float to half precision (float16)
    uint16_t packHalf(float value);

    // Unpack half precision to float
    float unpackHalf(uint16_t value);

    // Pack two float16 values into one uint32
    uint32_t packHalf2(float x, float y);

    // Encode quaternion using SparkJS octahedral folding method
    uint32_t encodeQuatOctXy88R8(float x, float y, float z, float w);

    // Pack splat data into SparkJS-compatible format
    PackedSplat packSplat(
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

    // Calculate texture coordinates for a splat index (SparkJS method)
    std::array<uint32_t, 3> getSplatTexCoord(uint32_t splatIndex);

    // Calculate required texture size for given number of splats
    std::array<uint32_t, 4> getTextureSize(uint32_t numSplats);
  }
}