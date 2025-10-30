#pragma once

#include <string>
#include <idgen.hpp>

namespace commandbuffers
{
  enum class GPUHandleType : uint32_t
  {
    kAdapter,
    kBindGroup,
    kBindGroupLayout,
    kBuffer,
    kCommandBuffer,
    kCommandEncoder,
    kComputePassEncoder,
    kComputePipeline,
    kDevice,
    kExternalTexture,
    kInstance,
    kPipelineLayout,
    kQuerySet,
    kQueue,
    kRenderBundle,
    kRenderBundleEncoder,
    kRenderPassEncoder,
    kRenderPipeline,
    kSampler,
    kShaderModule,
    kSharedBufferMemory,
    kSharedFence,
    kSharedTextureMemory,
    kSurface,
    kTexelBufferView,
    kTexture,
    kTextureView,

    // Additional internal object types. Keep kExtraObjectTypes in sync when updating.
    kBindGroupLayoutInternal,
  };

  enum class GPUShaderStage
  {
    kVertex,
    kFragment,
    kCompute,
  };

  enum class GPUBufferType
  {
    kReadOnlyStorage,
    kStorage,
    kUniform,
  };

  enum class GPUIndexFormat
  {
    kUint16,
    kUint32,
  };

  enum class GPUStorageAccess
  {
    kReadOnly,
    kReadWrite,
    kWriteOnly,
  };

  enum class GPUSamplerType
  {
    kComparison,
    kFiltering,
    kNonFiltering,
  };

  typedef uint32_t GPUIdentifier;
  class GPUHandle
  {
  private:
    static inline TrIdGeneratorBase<GPUIdentifier> Ids = TrIdGeneratorBase<GPUIdentifier>(0);

  public:
    GPUHandle(std::string label = "")
        : label(std::move(label))
        , id(Ids.get())
    {
    }

    virtual GPUHandleType type() const = 0;

  public:
    const std::string label = "";
    const GPUIdentifier id;
  };
}
