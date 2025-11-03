#pragma once

#include <common/command_buffers/gpu/gpu_base.hpp>

namespace commandbuffers
{
  class GPUTextureViewBase;

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
