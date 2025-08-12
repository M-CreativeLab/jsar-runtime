#include "packed_splats.hpp"
#include <common/debug.hpp>
#include <cstring>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace builtin_scene::packed_splat_utils
{
  using namespace std;

  // Helper union for float/uint32 conversion
  union FloatInt
  {
    float f;
    uint32_t i;
  };

  float uint32ToFloat(uint32_t value)
  {
    FloatInt fi;
    fi.i = value;
    return fi.f;
  }

  uint32_t floatToUint32(float value)
  {
    FloatInt fi;
    fi.f = value;
    return fi.i;
  }

  uint16_t packHalf(float value)
  {
    FloatInt fi;
    fi.f = value;
    uint32_t f32 = fi.i;

    uint32_t sign = (f32 >> 16) & 0x8000;
    uint32_t exp = ((f32 >> 23) & 0xff) - 127 + 15;
    uint32_t mant = (f32 >> 13) & 0x3ff;

    if (exp <= 0)
    {
      // Underflow to zero
      return static_cast<uint16_t>(sign);
    }
    else if (exp >= 31)
    {
      // Overflow to infinity
      return static_cast<uint16_t>(sign | 0x7c00);
    }

    return static_cast<uint16_t>(sign | (exp << 10) | mant);
  }

  float unpackHalf(uint16_t value)
  {
    uint32_t sign = (value & 0x8000) << 16;
    uint32_t exp = (value >> 10) & 0x1f;
    uint32_t mant = value & 0x3ff;

    if (exp == 0)
    {
      if (mant == 0)
      {
        // Zero
        FloatInt fi;
        fi.i = sign;
        return fi.f;
      }
      else
      {
        // Denormalized
        exp = 127 - 15 - 9;
        while ((mant & 0x400) == 0)
        {
          mant <<= 1;
          exp--;
        }
        mant &= 0x3ff;
      }
    }
    else if (exp == 31)
    {
      // Infinity or NaN
      exp = 255;
    }
    else
    {
      exp += 127 - 15;
    }

    FloatInt fi;
    fi.i = sign | (exp << 23) | (mant << 13);
    return fi.f;
  }

  uint32_t packHalf2(float x, float y)
  {
    uint16_t hx = packHalf(x);
    uint16_t hy = packHalf(y);
    return static_cast<uint32_t>(hx) | (static_cast<uint32_t>(hy) << 16);
  }

  uint32_t encodeQuatOctXy88R8(float x, float y, float z, float w)
  {
    // Ensure minimal representation: flip if w is negative
    if (w < 0.0f)
    {
      x = -x;
      y = -y;
      z = -z;
      w = -w;
    }

    // Compute rotation angle: θ = 2 * acos(w) ∈ [0,π]
    float theta = 2.0f * std::acos(std::clamp(w, -1.0f, 1.0f));
    float halfTheta = theta * 0.5f;
    float s = std::sin(halfTheta);

    // Recover the rotation axis; use a default if nearly zero rotation
    float axis_x, axis_y, axis_z;
    if (std::abs(s) < 1e-6f)
    {
      axis_x = 1.0f;
      axis_y = 0.0f;
      axis_z = 0.0f;
    }
    else
    {
      axis_x = x / s;
      axis_y = y / s;
      axis_z = z / s;
    }

    // Folded Octahedral Mapping
    float sum = abs(axis_x) + abs(axis_y) + abs(axis_z);
    float p_x = axis_x / sum;
    float p_y = axis_y / sum;

    // If axis_z < 0, fold the mapping
    if (axis_z < 0.0f)
    {
      float oldPx = p_x;
      p_x = (1.0f - abs(p_y)) * (p_x >= 0.0f ? 1.0f : -1.0f);
      p_y = (1.0f - abs(oldPx)) * (p_y >= 0.0f ? 1.0f : -1.0f);
    }

    // Remap from [-1,1] to [0,1]
    float u_f = p_x * 0.5f + 0.5f;
    float v_f = p_y * 0.5f + 0.5f;

    // Quantize to 8 bits (0 to 255)
    uint32_t quantU = static_cast<uint32_t>(clamp(round(u_f * 255.0f), 0.0f, 255.0f));
    uint32_t quantV = static_cast<uint32_t>(clamp(round(v_f * 255.0f), 0.0f, 255.0f));

    // Angle Quantization - quantize θ ∈ [0,π] to 8 bits (0 to 255)
    uint32_t angleInt = static_cast<uint32_t>(clamp(static_cast<float>(round((theta / M_PI) * 255.0f)), 0.0f, 255.0f));

    // Pack bits: bits [0–7]: quantU, [8–15]: quantV, [16–23]: angleInt
    return (angleInt << 16u) | (quantV << 8u) | quantU;
  }

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
  )
  {
    PackedSplat packed;

    // Word 0: RGBA as 4 x uint8
    uint32_t uR = static_cast<uint32_t>(clamp(round(r * 255.0f), 0.0f, 255.0f));
    uint32_t uG = static_cast<uint32_t>(clamp(round(g * 255.0f), 0.0f, 255.0f));
    uint32_t uB = static_cast<uint32_t>(clamp(round(b * 255.0f), 0.0f, 255.0f));
    uint32_t uA = static_cast<uint32_t>(clamp(round(a * 255.0f), 0.0f, 255.0f));
    uint32_t word0_bits = uR | (uG << 8u) | (uB << 16u) | (uA << 24u);
    packed.word0 = uint32ToFloat(word0_bits);

    // Word 1: Center XY as 2 x float16
    uint32_t word1_bits = packHalf2(px, py);
    packed.word1 = uint32ToFloat(word1_bits);

    // Encode quaternion using octahedral method
    uint32_t uQuat = encodeQuatOctXy88R8(qx, qy, qz, qw);
    uint32_t uQuat0 = uQuat & 0xffu;          // bits 0-7
    uint32_t uQuat1 = (uQuat >> 8u) & 0xffu;  // bits 8-15
    uint32_t uQuat2 = (uQuat >> 16u) & 0xffu; // bits 16-23

    // Word 2: Center Z as float16 + partial quaternion
    uint32_t word2_bits = packHalf2(pz, 0.0f) | (uQuat0 << 16u) | (uQuat1 << 24u);
    packed.word2 = uint32ToFloat(word2_bits);

    // Encode scales in three uint8s, where 0=>0.0 and 1..=255 stores log scale
    float lnScaleScale = 254.0f / (LN_SCALE_MAX - LN_SCALE_MIN);
    uint32_t uScaleX = (sx == 0.0f) ? 0u : static_cast<uint32_t>(clamp(round((log(sx) - LN_SCALE_MIN) * lnScaleScale), 0.0f, 254.0f)) + 1u;
    uint32_t uScaleY = (sy == 0.0f) ? 0u : static_cast<uint32_t>(clamp(round((log(sy) - LN_SCALE_MIN) * lnScaleScale), 0.0f, 254.0f)) + 1u;
    uint32_t uScaleZ = (sz == 0.0f) ? 0u : static_cast<uint32_t>(clamp(round((log(sz) - LN_SCALE_MIN) * lnScaleScale), 0.0f, 254.0f)) + 1u;

    // Word 3: Scales as 3 x uint8 + remaining quaternion
    uint32_t word3_bits = uScaleX | (uScaleY << 8u) | (uScaleZ << 16u) | (uQuat2 << 24u);
    packed.word3 = uint32ToFloat(word3_bits);

    return packed;
  }

  std::array<uint32_t, 3> getSplatTexCoord(uint32_t splatIndex)
  {
    uint32_t x = splatIndex & SPLAT_TEX_WIDTH_MASK;
    uint32_t y = (splatIndex >> SPLAT_TEX_WIDTH_BITS) & SPLAT_TEX_HEIGHT_MASK;
    uint32_t z = splatIndex >> SPLAT_TEX_LAYER_BITS;
    return {x, y, z};
  }

  std::array<uint32_t, 4> getTextureSize(uint32_t numSplats)
  {
    // Calculate how many layers we need
    uint32_t splatsPerLayer = SPLAT_TEX_WIDTH * SPLAT_TEX_HEIGHT;
    uint32_t numLayers = (numSplats + splatsPerLayer - 1) / splatsPerLayer;

    // Clamp to maximum depth
    numLayers = std::min(numLayers, SPLAT_TEX_DEPTH);

    uint32_t maxSplats = numLayers * splatsPerLayer;

    return {SPLAT_TEX_WIDTH, SPLAT_TEX_HEIGHT, numLayers, maxSplats};
  }
}