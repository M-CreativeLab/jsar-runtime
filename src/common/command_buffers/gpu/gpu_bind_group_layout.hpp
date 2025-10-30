#pragma once

#include <optional>
#include <vector>
#include <variant>

#include "./gpu_base.hpp"
#include "./gpu_texture.hpp"

namespace commandbuffers
{
  class GPUBindGroupLayoutBase : public GPUHandle
  {
  public:
    class BufferLayout
    {
    public:
      GPUBufferType type;
      bool hasDynamicOffset = false;
      uint32_t minBindingSize = 0; // in bytes
    };

    class TextureLayout
    {
    public:
      bool multisampled = false;
    };

    class StorageTextureLayout
    {
    public:
      GPUStorageAccess access;
      GPUTextureFormat format;
      std::optional<GPUTextureDimension> viewDimension;
    };

    class ExternalTextureLayout
    {
    };

    class SamplerLayout
    {
    public:
      GPUSamplerType type;
    };

    using ResourceLayout = std::variant<BufferLayout,
                                        TextureLayout,
                                        StorageTextureLayout,
                                        ExternalTextureLayout,
                                        SamplerLayout>;
    class Entry
    {
    public:
      uint32_t binding;
      GPUShaderStage visibility;
      ResourceLayout layout;
    };

  private:
    std::vector<Entry> entries_;
  };
}
