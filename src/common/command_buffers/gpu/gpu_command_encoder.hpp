#pragma once

#include <memory>
#include <string_view>

#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_command_buffer.hpp>
#include <common/command_buffers/gpu/gpu_pass_encoder_base.hpp>
#include <common/command_buffers/gpu/gpu_renderpass_encoder.hpp>
#include <common/command_buffers/gpu/encoding_context.hpp>

namespace commandbuffers
{
  struct GPUCommandEncoderDescriptor
  {
    std::string_view label;
  };

  enum class GPUUsageValidationMode
  {
    kDefault,
    kInternal,
  };

  class GPUCommandEncoder final : public GPUHandle
  {
  public:
    GPUHandleType type() const override
    {
      return GPUHandleType::kCommandEncoder;
    }

  public:
    // Pass Encoding
    // TODO(yorkie): beginComputePass
    GPURenderPassEncoder beginRenderPass(GPURenderPassDescriptor &);
    std::unique_ptr<GPUCommandBufferBase> finish(std::optional<std::string> label = std::nullopt) const;

    // Copy Operations
    // WebGPU Spec: https://www.w3.org/TR/webgpu/#copy-operations
    //
    // TODO: Implement copy operations for TrCommandBufferManager generic command encoding.
    // These methods are needed to handle buffer and texture copy commands.
    //
    // void copyBufferToBuffer(
    //   const GPUBufferBase& source,
    //   uint64_t sourceOffset,
    //   const GPUBufferBase& destination,
    //   uint64_t destinationOffset,
    //   uint64_t size);
    //
    // void copyBufferToTexture(
    //   const GPUImageCopyBuffer& source,
    //   const GPUImageCopyTexture& destination,
    //   const GPUExtent3D& copySize);
    //
    // void copyTextureToBuffer(
    //   const GPUImageCopyTexture& source,
    //   const GPUImageCopyBuffer& destination,
    //   const GPUExtent3D& copySize);
    //
    // void copyTextureToTexture(
    //   const GPUImageCopyTexture& source,
    //   const GPUImageCopyTexture& destination,
    //   const GPUExtent3D& copySize);
    //
    // void clearBuffer(
    //   const GPUBufferBase& buffer,
    //   uint64_t offset = 0,
    //   std::optional<uint64_t> size = std::nullopt);

  private:
    GPUCommandEncoder(Ref<GPUDeviceBase> device, const GPUCommandEncoderDescriptor &descriptor);
    GPUCommandEncoder(Ref<GPUDeviceBase> device,
                      GPUHandle::ErrorTag tag,
                      std::string_view label);

    bool validateFinish() const;

    gpu::EncodingContext encoding_context_;
    std::unordered_set<GPUBufferBase *> top_level_buffers_;
    std::unordered_set<GPUTextureBase *> top_level_textures_;
    // std::unordered_set<GPUQuerySetBase *> used_query_sets_;

    uint64_t debug_group_stack_size_ = 0;
    GPUUsageValidationMode usage_validation_mode_;
  };
}
