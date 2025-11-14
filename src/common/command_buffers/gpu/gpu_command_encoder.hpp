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
    static Ref<GPUCommandEncoder> Create(Ref<GPUDeviceBase> device, const GPUCommandEncoderDescriptor &descriptor);
    static Ref<GPUCommandEncoder> MakeError(Ref<GPUDeviceBase> device, std::string_view label);

    GPUHandleType type() const override
    {
      return GPUHandleType::kCommandEncoder;
    }

  public:
    // TODO(yorkie): begineComputePass
    GPURenderPassEncoder beginRenderPass(GPURenderPassDescriptor &);
    void clearBuffer();
    void copyBufferToBuffer();
    void copyBufferToTexture();
    void copyTextureToBuffer();
    void copyTextureToTexture();
    std::unique_ptr<GPUCommandBufferBase> finish(std::optional<std::string> label = std::nullopt) const;

    void insertDebugMarker(std::string_view marker);
    void pushDebugGroup(std::string_view group);
    void popDebugGroup();

    void resolveQuerySet();
    void writeTimestamp();

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
