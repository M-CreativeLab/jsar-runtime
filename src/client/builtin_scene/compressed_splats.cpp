#include "./compressed_splats.hpp"
#include <cstring>
#include <cmath>

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

  // Helper functions for half-float conversion
  uint16_t floatToHalf(float f)
  {
    // Simple float to half-float conversion
    union
    {
      float f;
      uint32_t i;
    } fi = {f};
    uint32_t sign = (fi.i >> 16) & 0x8000;
    int32_t exponent = ((fi.i >> 23) & 0xFF) - 127 + 15;
    uint32_t mantissa = fi.i & 0x7FFFFF;

    if (exponent <= 0)
    {
      // Denormalized or zero
      if (exponent < -10)
        return (uint16_t)sign; // Zero
      mantissa = (mantissa | 0x800000) >> (1 - exponent);
      return (uint16_t)(sign | (mantissa >> 13));
    }
    else if (exponent >= 31)
    {
      // Infinity or NaN
      return (uint16_t)(sign | 0x7C00 | ((mantissa != 0) ? 0x200 : 0));
    }
    else
    {
      // Normal
      return (uint16_t)(sign | (exponent << 10) | (mantissa >> 13));
    }
  }

  float halfToFloat(uint16_t h)
  {
    // Simple half-float to float conversion
    uint32_t sign = (h & 0x8000) << 16;
    int32_t exponent = (h >> 10) & 0x1F;
    uint32_t mantissa = h & 0x3FF;

    if (exponent == 0)
    {
      if (mantissa == 0)
      {
        // Zero
        union
        {
          uint32_t i;
          float f;
        } fi = {sign};
        return fi.f;
      }
      else
      {
        // Denormalized
        exponent = 1;
        while ((mantissa & 0x400) == 0)
        {
          mantissa <<= 1;
          exponent--;
        }
        mantissa &= 0x3FF;
        exponent += 127 - 15;
      }
    }
    else if (exponent == 31)
    {
      // Infinity or NaN
      exponent = 255;
    }
    else
    {
      // Normal
      exponent += 127 - 15;
    }

    union
    {
      uint32_t i;
      float f;
    } fi = {sign | (exponent << 23) | (mantissa << 13)};
    return fi.f;
  }

  // Octahedral mapping functions for quaternion compression
  std::array<float, 2> octWrap(float x, float y)
  {
    return {
      (1.0f - std::abs(y)) * (x >= 0.0f ? 1.0f : -1.0f),
      (1.0f - std::abs(x)) * (y >= 0.0f ? 1.0f : -1.0f)};
  }

  std::array<float, 2> quatToOct(float x, float y, float z, float w)
  {
    // Normalize quaternion
    float length = std::sqrt(x * x + y * y + z * z + w * w);
    if (length > 0.0f)
    {
      x /= length;
      y /= length;
      z /= length;
      w /= length;
    }

    // Ensure w is positive
    if (w < 0.0f)
    {
      x = -x;
      y = -y;
      z = -z;
      w = -w;
    }

    // Project to octahedron
    float norm = std::abs(x) + std::abs(y) + std::abs(z);
    float octX = x / norm;
    float octY = y / norm;

    if (z < 0.0f)
    {
      auto wrapped = octWrap(octX, octY);
      octX = wrapped[0];
      octY = wrapped[1];
    }

    return {octX, octY};
  }

  std::array<float, 4> octToQuat(float octX, float octY)
  {
    float z = 1.0f - std::abs(octX) - std::abs(octY);
    float x = octX;
    float y = octY;

    if (z < 0.0f)
    {
      auto wrapped = octWrap(octX, octY);
      x = wrapped[0];
      y = wrapped[1];
    }

    // Normalize
    float length = std::sqrt(x * x + y * y + z * z);
    if (length > 0.0f)
    {
      x /= length;
      y /= length;
      z /= length;
    }

    // Reconstruct w
    float w = std::sqrt(std::max(0.0f, 1.0f - (x * x + y * y + z * z)));

    return {x, y, z, w};
  }

  float compressQuaternionOct(float x, float y, float z, float w)
  {
    auto oct = quatToOct(x, y, z, w);

    // Convert to 12-bit signed integers for xy, 8-bit for additional data
    int32_t octX12 = (int32_t)(oct[0] * 2047.0f); // 12-bit signed (-2047 to 2047)
    int32_t octY12 = (int32_t)(oct[1] * 2047.0f); // 12-bit signed (-2047 to 2047)

    // Clamp to valid range
    octX12 = std::max(-2047, std::min(2047, octX12));
    octY12 = std::max(-2047, std::min(2047, octY12));

    // Pack into 24-bit value: 12 bits each for x,y
    uint32_t packed = 0;
    packed |= (uint32_t)(octX12 + 2047) & 0xFFF;         // x: bits 0-11 (offset by 2047)
    packed |= ((uint32_t)(octY12 + 2047) & 0xFFF) << 12; // y: bits 12-23 (offset by 2047)

    // Convert to float using bit reinterpretation
    float result;
    std::memcpy(&result, &packed, sizeof(float));
    return result;
  }

  std::array<float, 4> decompressQuaternionOct(float compressed)
  {
    // Extract packed value
    uint32_t packed;
    std::memcpy(&packed, &compressed, sizeof(uint32_t));

    // Unpack x,y components (12 bits each, offset by 2047)
    int32_t octX12 = (int32_t)(packed & 0xFFF) - 2047;
    int32_t octY12 = (int32_t)((packed >> 12) & 0xFFF) - 2047;

    // Convert back to normalized float
    float octX = (float)octX12 / 2047.0f;
    float octY = (float)octY12 / 2047.0f;

    // Convert from octahedral to quaternion
    auto quat = octToQuat(octX, octY);
    return quat;
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

  std::array<float, 2> compressPositionHalf(float x, float y, float z)
  {
    // Pack position into two words using half-floats
    // word0: pos.xy as half-floats (16 bits each)
    // word1: pos.z as half-float in lower 16 bits

    uint16_t hx = floatToHalf(x);
    uint16_t hy = floatToHalf(y);
    uint16_t hz = floatToHalf(z);

    // Pack xy into word0
    uint32_t word0 = (uint32_t)hx | ((uint32_t)hy << 16);

    // Pack z into lower 16 bits of word1
    uint32_t word1 = (uint32_t)hz;

    // Convert to floats using bit reinterpretation
    float fword0, fword1;
    std::memcpy(&fword0, &word0, sizeof(float));
    std::memcpy(&fword1, &word1, sizeof(float));

    return {fword0, fword1};
  }

  std::array<float, 3> decompressPositionHalf(float word0, float word1)
  {
    // Extract packed values
    uint32_t w0, w1;
    std::memcpy(&w0, &word0, sizeof(uint32_t));
    std::memcpy(&w1, &word1, sizeof(uint32_t));

    // Unpack half-floats
    uint16_t hx = w0 & 0xFFFF;
    uint16_t hy = (w0 >> 16) & 0xFFFF;
    uint16_t hz = w1 & 0xFFFF;

    // Convert back to floats
    float x = halfToFloat(hx);
    float y = halfToFloat(hy);
    float z = halfToFloat(hz);

    return {x, y, z};
  }

  float compressScaleLog(float x, float y, float z, const float minLogScale[3], const float maxLogScale[3])
  {
    // Apply log2 compression to scale values
    float logX = (x > 0.0f) ? std::log2(x) : -10.0f; // Use -10 for very small scales
    float logY = (y > 0.0f) ? std::log2(y) : -10.0f;
    float logZ = (z > 0.0f) ? std::log2(z) : -10.0f;

    // Normalize to [0,1] range using provided log scale bounds
    float nx = (maxLogScale[0] > minLogScale[0]) ? (logX - minLogScale[0]) / (maxLogScale[0] - minLogScale[0]) : 0.5f;
    float ny = (maxLogScale[1] > minLogScale[1]) ? (logY - minLogScale[1]) / (maxLogScale[1] - minLogScale[1]) : 0.5f;
    float nz = (maxLogScale[2] > minLogScale[2]) ? (logZ - minLogScale[2]) / (maxLogScale[2] - minLogScale[2]) : 0.5f;

    // Clamp to [0,1] range
    nx = std::max(0.0f, std::min(1.0f, nx));
    ny = std::max(0.0f, std::min(1.0f, ny));
    nz = std::max(0.0f, std::min(1.0f, nz));

    // Convert to 8-bit integers (stored in lower 24 bits)
    uint32_t ix = (uint32_t)(nx * 255.0f);
    uint32_t iy = (uint32_t)(ny * 255.0f);
    uint32_t iz = (uint32_t)(nz * 255.0f);

    // Pack into lower 24 bits: 8 bits each for x,y,z
    uint32_t packed = ix | (iy << 8) | (iz << 16);

    // Convert to float using bit reinterpretation
    float result;
    std::memcpy(&result, &packed, sizeof(float));
    return result;
  }

  std::array<float, 3> decompressScaleLog(float word2, const float minLogScale[3], const float maxLogScale[3])
  {
    // Extract packed value
    uint32_t packed;
    std::memcpy(&packed, &word2, sizeof(uint32_t));

    // Unpack x,y,z components from lower 24 bits (8 bits each)
    uint32_t ix = packed & 0xFF;
    uint32_t iy = (packed >> 8) & 0xFF;
    uint32_t iz = (packed >> 16) & 0xFF;

    // Convert back to normalized float [0,1]
    float nx = (float)ix / 255.0f;
    float ny = (float)iy / 255.0f;
    float nz = (float)iz / 255.0f;

    // Denormalize using provided log scale bounds
    float logX = minLogScale[0] + nx * (maxLogScale[0] - minLogScale[0]);
    float logY = minLogScale[1] + ny * (maxLogScale[1] - minLogScale[1]);
    float logZ = minLogScale[2] + nz * (maxLogScale[2] - minLogScale[2]);

    // Convert back from log2 to linear scale
    float x = std::exp2(logX);
    float y = std::exp2(logY);
    float z = std::exp2(logZ);

    return {x, y, z};
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
    float a,                                   // color + opacity
    const SplatNormalizationParams &normParams // normalization parameters
  )
  {
    CompressedSplat compressed;

    // Compress position using half-floats
    auto posWords = compressPositionHalf(px, py, pz);
    compressed.word[0] = posWords[0]; // pos.xy as half-floats

    // Compress quaternion using octahedral mapping (24-bit)
    float compressedQuat = compressQuaternionOct(qx, qy, qz, qw);
    uint32_t quatBits;
    std::memcpy(&quatBits, &compressedQuat, sizeof(uint32_t));
    quatBits &= 0xFFFFFF; // Keep only 24 bits

    // word1: pos.z (lower 16 bits) + upper 16 bits of quaternion
    uint32_t word1;
    std::memcpy(&word1, &posWords[1], sizeof(uint32_t));
    word1 = (word1 & 0xFFFF) | ((quatBits & 0xFFFF) << 16);
    std::memcpy(&compressed.word[1], &word1, sizeof(float));

    // Compress scale using log compression
    float compressedScale = compressScaleLog(sx, sy, sz, normParams.scaleMin, normParams.scaleMax);
    uint32_t scaleBits;
    std::memcpy(&scaleBits, &compressedScale, sizeof(uint32_t));
    scaleBits &= 0xFFFFFF; // Keep only 24 bits

    // word2: upper 8 bits of quaternion + scale.xyz (24 bits)
    uint32_t word2 = (((quatBits >> 16) & 0xFF) << 24) | scaleBits;
    std::memcpy(&compressed.word[2], &word2, sizeof(float));

    // word3: RGBA color (unchanged)
    compressed.word[3] = compressColor(r, g, b, a);

    return compressed;
  }
}