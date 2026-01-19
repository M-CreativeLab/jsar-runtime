#pragma once

#include <string_view>
#include <common/command_buffers/gpu/gpu_constants.hpp>
#include <common/command_buffers/gpu/gpu_buffer.hpp>
#include <common/command_buffers/gpu/gpu_texture_view.hpp>
#include <common/command_buffers/gpu/gpu_sampler.hpp>
#include <common/command_buffers/gpu/gpu_bind_group_layout.hpp>

namespace commandbuffers
{
  struct GPUBindGroupEntry
  {
    uint32_t binding;
    GPUBufferBase *buffer = nullptr;
    uint64_t offset = 0;
    uint64_t size = gpu_constants::kWholeSize;
    GPUSamplerBase *sampler = nullptr;
    GPUTextureViewBase *textureView = nullptr;
  };

  struct GPUBindGroupDescriptor
  {
    std::string_view label;
    GPUBindGroupLayoutBase *layout;
    size_t entryCount = 0;
    GPUBindGroupEntry const *entries = nullptr;
  };
}
