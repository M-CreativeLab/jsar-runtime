#pragma once

#include <string>
#include <string_view>
#include <memory>

#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_texture.hpp>

namespace commandbuffers
{
  struct TextureViewDescriptor
  {
    std::string_view label;
    GPUTextureFormat format = GPUTextureFormat::kUndefined;
    GPUTextureViewDimension dimension = GPUTextureViewDimension::kUndefined;
    uint32_t baseMipLevel = 0;
    uint32_t mipLevelCount = gpu_constants::kMipLevelCountUndefined;
    uint32_t baseArrayLayer = 0;
    uint32_t arrayLayerCount = gpu_constants::kArrayLayerCountUndefined;
    GPUTextureAspect aspect = GPUTextureAspect::kAll;
    GPUTextureUsage usage = GPUTextureUsage::kNone;
  };

  class GPUTextureViewBase : public GPUHandle
  {
  public:
    GPUTextureViewBase(Ref<GPUTextureBase> texture, const TextureViewDescriptor &descriptor);

    Ref<GPUTextureBase> texture() const;
    const GPUTextureBase &textureRef() const;

    GPUTextureViewDimension dimension() const;
    uint32_t getBaseMipLevel() const;
    uint32_t getLevelCount() const;
    uint32_t getBaseArrayLayer() const;
    uint32_t getLayerCount() const;

    GPUTextureComponentSwizzle getSwizzle() const;
    bool isSwizzleIdentity() const;

  private:
    Ref<GPUTextureBase> texture_;
    GPUTextureViewDimension dimension_;
    GPUTextureUsage usage_ = GPUTextureUsage::kNone;
    GPUTextureUsage internal_usage_ = GPUTextureUsage::kNone;
    GPUComponentSwizzle swizzle_red_ = GPUComponentSwizzle::kR;
    GPUComponentSwizzle swizzle_green_ = GPUComponentSwizzle::kG;
    GPUComponentSwizzle swizzle_blue_ = GPUComponentSwizzle::kB;
    GPUComponentSwizzle swizzle_alpha_ = GPUComponentSwizzle::kA;
    bool is_swizzle_identity_ = false;
  };
}
