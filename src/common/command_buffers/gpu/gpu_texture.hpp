#pragma once

#include "./gpu_base.hpp"

namespace commandbuffers
{
  enum class GPUTextureFormat
  {
    // 8-bit formats
    kR8UNORM,
    kR8SNORM,
    kR8UINT,
    kR8SINT,

    // 16-bit formats
    kR16UNORM,
    kR16SNORM,
    kR16UINT,
    kR16SINT,
    kR16FLOAT,
    kRG8UNORM,
    kRG8SNORM,
    kRG8UINT,
    kRG8SINT,

    // 32-bit formats
    kR32UINT,
    kR32SINT,
    kR32FLOAT,
    kRG16UNORM,
    kRG16SNORM,
    kRG16UINT,
    kRG16SINT,
    kRG16FLOAT,
    kRGBA8UNORM,
    kRGBA8UNORM_SRGB,
    kRGBA8SNORM,
    kRGBA8UINT,
    kRGBA8SINT,
    kBGRA8UNORM,
    kBGRA8UNORM_SRGB,

    // Packed 32-bit formats
    kRGB9E5UFLOAT,
    kRGB10A2UINT,
    kRGB10A2UNORM,
    kRG11B10UFLOAT,

    // 64-bit formats
    kRG32UINT,
    kRG32SINT,
    kRG32FLOAT,
    kRGBA16UNORM,
    kRGBA16SNORM,
    kRGBA16UINT,
    kRGBA16SINT,
    kRGBA16FLOAT,

    // 128-bit formats
    kRGBA32UINT,
    kRGBA32SINT,
    kRGBA32FLOAT,

    // Depth/stencil formats
    kSTENCIL8,
    kDEPTH16UNORM,
    kDEPTH24PLUS,
    kDEPTH24PLUS_STENCIL8,
    kDEPTH32FLOAT,
    kDEPTH32FLOAT_STENCIL8,

    // BC compressed formats
    kBC1_RGBA_UNORM,
    kBC1_RGBA_UNORM_SRGB,
    kBC2_RGBA_UNORM,
    kBC2_RGBA_UNORM_SRGB,
    kBC3_RGBA_UNORM,
    kBC3_RGBA_UNORM_SRGB,
    kBC4_R_UNORM,
    kBC4_R_SNORM,
    kBC5_RG_UNORM,
    kBC5_RG_SNORM,
    kBC6H_RGB_UFLOAT,
    kBC6H_RGB_FLOAT,
    kBC7_RGBA_UNORM,
    kBC7_RGBA_UNORM_SRGB,

    // ETC2 compressed formats
    kETC2_RGB8UNORM,
    kETC2_RGB8UNORM_SRGB,
    kETC2_RGB8A1UNORM,
    kETC2_RGB8A1UNORM_SRGB,
    kETC2_RGBA8UNORM,
    kETC2_RGBA8UNORM_SRGB,
    kEAC_R11UNORM,
    kEAC_R11SNORM,
    kEAC_RG11UNORM,
    kEAC_RG11SNORM,

    // ASTC compressed formats
    kASTC_4x4_UNORM,
    kASTC_4x4_UNORM_SRGB,
    kASTC_5x4_UNORM,
    kASTC_5x4_UNORM_SRGB,
    kASTC_5x5_UNORM,
    kASTC_5x5_UNORM_SRGB,
    kASTC_6x5_UNORM,
    kASTC_6x5_UNORM_SRGB,
    kASTC_6x6_UNORM,
    kASTC_6x6_UNORM_SRGB,
    kASTC_8x5_UNORM,
    kASTC_8x5_UNORM_SRGB,
    kASTC_8x6_UNORM,
    kASTC_8x6_UNORM_SRGB,
    kASTC_8x8_UNORM,
    kASTC_8x8_UNORM_SRGB,
    kASTC_10x5_UNORM,
    kASTC_10x5_UNORM_SRGB,
    kASTC_10x6_UNORM,
    kASTC_10x6_UNORM_SRGB,
    kASTC_10x8_UNORM,
    kASTC_10x8_UNORM_SRGB,
    kASTC_10x10_UNORM,
    kASTC_10x10_UNORM_SRGB,
    kASTC_12x10_UNORM,
    kASTC_12x10_UNORM_SRGB,
    kASTC_12x12_UNORM,
    kASTC_12x12_UNORM_SRGB
  };

  enum class GPUTextureDimension
  {
    kTexture1D,
    kTexture2D,
    kTexture3D,
  };

  class GPUTexture : public GPUHandle
  {
  public:
  private:
    uint32_t width_;
    uint32_t height_;
    uint32_t depth_or_array_layers_;
    GPUTextureDimension dimension_;
    GPUTextureFormat format_;
    uint32_t mip_level_count_;
    uint32_t sample_count_;
  };
}
