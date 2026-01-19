#pragma once

#include <common/command_buffers/gpu/gpu_base.hpp>

namespace commandbuffers
{
  class GPUTextureViewBase;

  struct GPUTextureDescriptor
  {
    std::string_view label;
    GPUTextureUsage usage = GPUTextureUsage::kNone;
    GPUTextureDimension dimension = GPUTextureDimension::k2D;
    GPUExtent3D size;
    GPUTextureFormat format = GPUTextureFormat::kUndefined;
    uint32_t mipLevelCount = 1;
    uint32_t sampleCount = 1;
    size_t viewFormatCount = 0;
    GPUTextureFormat const *viewFormats = nullptr;
  };

  class GPUTextureBase : public GPUHandle
  {
  public:
  private:
    GPUExtent3D base_size_;
    GPUTextureDimension dimension_;
    GPUTextureFormat format_;
    uint32_t mip_level_count_;
    uint32_t sample_count_;
    GPUTextureUsage usage_ = GPUTextureUsage::kNone;
    GPUTextureUsage internal_usage_ = GPUTextureUsage::kNone;
    GPUTextureViewBase *default_view_ = nullptr;
  };
}
