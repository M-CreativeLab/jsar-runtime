#pragma once

#include <variant>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_bind_group_layout_base.hpp>
#include <common/command_buffers/gpu/gpu_per_stage.hpp>

namespace commandbuffers
{
  enum class GPUBindingInfoType
  {
    Buffer,
    Sampler,
    Texture,
    StorageTexture,
    TexelBuffer,
    ExternalTexture,
    StaticSampler,
    // Internal to vulkan only.
    InputAttachment,
  };

  struct GPUBufferBindingInfo
  {
    GPUBufferBindingType type;
    uint64_t minBindingSize{0u};
    bool hasDynamicOffset{false};

    static GPUBufferBindingInfo From(const GPUBufferBindingLayout &);
  };

  struct GPUSamplerBindingInfo
  {
    GPUSamplerBindingType type;

    static GPUSamplerBindingInfo From(const GPUSamplerBindingLayout &);
  };

  struct GPUTextureBindingInfo
  {
    GPUTextureSampleType sampleType;
    GPUTextureViewDimension viewDimension;
    bool multisampled{false};

    static GPUTextureBindingInfo From(const GPUTextureBindingLayout &);
  };

  struct GPUTexelBufferBindingInfo
  {
    // TODO: Add fields if needed
  };

  struct GPUStorageTextureBindingInfo
  {
    GPUTextureFormat format;
    GPUTextureViewDimension viewDimension;
    GPUStorageTextureAccess access;

    static GPUStorageTextureBindingInfo From(const GPUStorageTextureBindingLayout &);
  };

  struct GPUExternalTextureBindingInfo
  {
    GPUBindingIndex metadata;
    GPUBindingIndex plane0;
    GPUBindingIndex plane1;
  };

  struct GPUStaticSamplerBindingInfo
  {
    // No fields needed for static sampler
  };

  struct GPUInputAttachmentBindingInfo
  {
    GPUTextureSampleType type;
  };

  struct GPUBindingInfo
  {
    GPUBindingNumber binding;
    GPUShaderStage visibility;
    GPUBindingIndex arraySize{1u};
    GPUBindingIndex indexInArray{0u};

    using BindingLayout = std::variant<GPUBufferBindingInfo,
                                       GPUSamplerBindingInfo,
                                       GPUTextureBindingInfo,
                                       GPUTexelBufferBindingInfo,
                                       GPUStorageTextureBindingInfo,
                                       GPUExternalTextureBindingInfo,
                                       GPUStaticSamplerBindingInfo,
                                       GPUInputAttachmentBindingInfo>;
    BindingLayout bindingLayout;

    bool operator==(const GPUBindingInfo &rhs) const = default;
    std::string toString() const;
  };

  GPUBindingInfoType GetBindingInfoType(const GPUBindingInfo &);

  struct GPUBindingSlot
  {
    GPUBindGroupIndex group;
    GPUBindingNumber binding;

    constexpr bool operator==(const GPUBindingSlot &rhs) const
    {
      return group == rhs.group && binding == rhs.binding;
    }
    constexpr bool operator!=(const GPUBindingSlot &rhs) const
    {
      return !(*this == rhs);
    }
    constexpr bool operator<(const GPUBindingSlot &rhs) const
    {
      if (group < rhs.group)
      {
        return true;
      }
      if (group > rhs.group)
      {
        return false;
      }
      return binding < rhs.binding;
    }
  };

  struct GPUBindingCountsPerStage
  {
    uint32_t sampledTextureCount;
    uint32_t samplerCount;
    uint32_t storageBufferCount;
    uint32_t storageTextureCount;
    uint32_t texelBufferCount;
    uint32_t uniformBufferCount;
    uint32_t externalTextureCount;
    uint32_t staticSamplerCount;
  };

  struct GPUBindingCounts
  {
    uint32_t totalCount;
    uint32_t bufferCount;
    uint32_t unverifiedBufferCount;
    uint32_t dynamicUniformBufferCount;
    uint32_t dynamicStorageBufferCount;
    uint32_t staticSamplerCount;
    GPUPerStage<GPUBindingCountsPerStage> perStage;
  };
}
