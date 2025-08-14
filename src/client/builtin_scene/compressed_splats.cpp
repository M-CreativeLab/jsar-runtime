#include "./compressed_splats.hpp"
#include <cstring>

namespace builtin_scene::compressed_splat_utils
{
  std::array<uint32_t, 2> getSplatTexCoord(uint32_t splatIndex)
  {
    uint32_t x = splatIndex & COMPRESSED_SPLAT_TEX_WIDTH_MASK;  // x = index & (1024 - 1)
    uint32_t y = splatIndex >> COMPRESSED_SPLAT_TEX_WIDTH_BITS; // y = index >> 10
    return {x, y};
  }

  std::array<uint32_t, 3> getTextureSize(uint32_t numSplats)
  {
    const uint32_t width = COMPRESSED_SPLAT_TEX_WIDTH;
    uint32_t height = (numSplats + width - 1) / width; // Ceiling division

    // Round height up to next power of 2 for optimal GPU performance
    uint32_t powerOf2Height = 1;
    while (powerOf2Height < height)
      powerOf2Height <<= 1;

    uint32_t maxSplats = width * powerOf2Height;
    return {width, powerOf2Height, maxSplats};
  }

  float compressQuaternion(float x, float y, float z, float w)
  {
    // Normalize quaternion to ensure unit length
    float length = std::sqrt(x * x + y * y + z * z + w * w);
    if (length > 0.0f)
    {
      x /= length;
      y /= length;
      z /= length;
      w /= length;
    }

    // Ensure w is positive for consistent reconstruction
    if (w < 0.0f)
    {
      x = -x;
      y = -y;
      z = -z;
      w = -w;
    }

    // Pack x,y,z components and sign of w into a single float
    // Use a simple encoding: store the 3 components in a way that preserves precision
    // We'll use bit manipulation to pack the data

    // Convert to 10-bit signed integers for x,y,z (range -512 to 511)
    // This gives us reasonable precision while fitting in 32 bits
    int32_t ix = (int32_t)(x * 511.0f);
    int32_t iy = (int32_t)(y * 511.0f);
    int32_t iz = (int32_t)(z * 511.0f);

    // Clamp to valid range
    ix = std::max(-511, std::min(511, ix));
    iy = std::max(-511, std::min(511, iy));
    iz = std::max(-511, std::min(511, iz));

    // Pack into 32-bit value: 10 bits each for x,y,z, 2 bits unused
    uint32_t packed = 0;
    packed |= (uint32_t)(ix + 512) & 0x3FF;         // x: bits 0-9
    packed |= ((uint32_t)(iy + 512) & 0x3FF) << 10; // y: bits 10-19
    packed |= ((uint32_t)(iz + 512) & 0x3FF) << 20; // z: bits 20-29
    // Sign of w is preserved implicitly since we ensure w >= 0

    // Convert to float using bit reinterpretation
    float result;
    std::memcpy(&result, &packed, sizeof(float));
    return result;
  }

  std::array<float, 4> decompressQuaternion(float compressed)
  {
    // Extract packed value
    uint32_t packed;
    std::memcpy(&packed, &compressed, sizeof(uint32_t));

    // Unpack x,y,z components
    int32_t ix = (int32_t)(packed & 0x3FF) - 512;
    int32_t iy = (int32_t)((packed >> 10) & 0x3FF) - 512;
    int32_t iz = (int32_t)((packed >> 20) & 0x3FF) - 512;

    // Convert back to normalized float
    float x = (float)ix / 511.0f;
    float y = (float)iy / 511.0f;
    float z = (float)iz / 511.0f;

    // Reconstruct w using unit length constraint
    float w_squared = 1.0f - (x * x + y * y + z * z);
    float w = (w_squared > 0.0f) ? std::sqrt(w_squared) : 0.0f;

    return {x, y, z, w};
  }

  float compressColor(float r, float g, float b, float a)
  {
    // Clamp values to [0,1] range
    r = std::max(0.0f, std::min(1.0f, r));
    g = std::max(0.0f, std::min(1.0f, g));
    b = std::max(0.0f, std::min(1.0f, b));
    a = std::max(0.0f, std::min(1.0f, a));

    // Convert to 8-bit integers for maximum precision
    uint32_t ir = (uint32_t)(r * 255.0f);
    uint32_t ig = (uint32_t)(g * 255.0f);
    uint32_t ib = (uint32_t)(b * 255.0f);
    uint32_t ia = (uint32_t)(a * 255.0f);

    // Pack into 32-bit RGBA value
    uint32_t packed = (ir) | (ig << 8) | (ib << 16) | (ia << 24);

    // Convert to float using bit reinterpretation
    float result;
    std::memcpy(&result, &packed, sizeof(float));
    return result;
  }

  std::array<float, 4> decompressColor(float compressed)
  {
    // Extract packed value
    uint32_t packed;
    std::memcpy(&packed, &compressed, sizeof(uint32_t));

    // Unpack RGBA components
    uint32_t ir = packed & 0xFF;
    uint32_t ig = (packed >> 8) & 0xFF;
    uint32_t ib = (packed >> 16) & 0xFF;
    uint32_t ia = (packed >> 24) & 0xFF;

    // Convert back to normalized float
    float r = (float)ir / 255.0f;
    float g = (float)ig / 255.0f;
    float b = (float)ib / 255.0f;
    float a = (float)ia / 255.0f;

    return {r, g, b, a};
  }

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
  )
  {
    CompressedSplat compressed;

    // Texel 0: position.xyz, scale.x
    compressed.texel0[0] = px;
    compressed.texel0[1] = py;
    compressed.texel0[2] = pz;
    compressed.texel0[3] = sx;

    // Texel 1: scale.yz, compressed_quat, compressed_color
    compressed.texel1[0] = sy;
    compressed.texel1[1] = sz;
    compressed.texel1[2] = compressQuaternion(qx, qy, qz, qw);
    compressed.texel1[3] = compressColor(r, g, b, a);

    return compressed;
  }
}