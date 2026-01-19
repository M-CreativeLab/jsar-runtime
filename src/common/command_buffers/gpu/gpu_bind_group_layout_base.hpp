#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <common/command_buffers/gpu/gpu_base.hpp>

namespace commandbuffers
{
  struct GPUBufferBindingLayout
  {
    GPUBufferBindingType type = GPUBufferBindingType::kUniform;
    bool hasDynamicOffset = false;
    uint64_t minBindingSize = 0;
  };

  struct GPUSamplerBindingLayout
  {
    GPUSamplerBindingType type = GPUSamplerBindingType::kFiltering;
  };

  struct GPUTextureBindingLayout
  {
    GPUTextureSampleType sampleType = GPUTextureSampleType::kFloat;
    GPUTextureViewDimension viewDimension = GPUTextureViewDimension::k2D;
    bool multisampled = false;
  };

  struct GPUStorageTextureBindingLayout
  {
    GPUStorageTextureAccess access = GPUStorageTextureAccess::kWriteOnly;
    GPUTextureFormat format = GPUTextureFormat::kUndefined;
    GPUTextureViewDimension viewDimension = GPUTextureViewDimension::k2D;
  };

  struct GPUBindGroupLayoutEntry
  {
    uint32_t binding;
    GPUShaderStage visibility = GPUShaderStage::kNone;
    uint32_t bindingArraySize = 0;

    GPUBufferBindingLayout buffer = {GPUBufferBindingType::kBindingNotUsed, false, 0};
    GPUSamplerBindingLayout sampler = {GPUSamplerBindingType::kBindingNotUsed};
    GPUTextureBindingLayout texture = {GPUTextureSampleType::kBindingNotUsed,
                                       GPUTextureViewDimension::k2D,
                                       false};
    GPUStorageTextureBindingLayout storageTexture = {GPUStorageTextureAccess::kBindingNotUsed,
                                                     GPUTextureFormat::kUndefined,
                                                     GPUTextureViewDimension::k2D};
  };

  struct GPUBindGroupLayoutDescriptor
  {
    std::string_view label;
    size_t entryCount = 0;
    GPUBindGroupLayoutEntry const *entries = nullptr;
  };
}
