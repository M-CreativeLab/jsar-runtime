#pragma once

#include <memory>
#include <string_view>

#include "./gpu_base.hpp"
#include "./gpu_command_buffer.hpp"
#include "./gpu_pass_encoder_base.hpp"
#include "./gpu_renderpass_encoder.hpp"

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
    virtual ~GPUCommandEncoder() = default;

    GPUHandleType type() const override
    {
      return GPUHandleType::kCommandEncoder;
    }

  public:
    // TODO(yorkie): begineComputePass
    virtual GPURenderPassEncoderBase beginRenderPass(GPURenderPassDescriptor &) = 0;
    virtual std::unique_ptr<GPUCommandBufferBase> finish(std::optional<std::string> label = std::nullopt) const = 0;

  private:
    GPUCommandEncoder(std::shared_ptr<GPUDeviceBase> device, const GPUCommandEncoderDescriptor &descriptor);
    GPUCommandEncoder(std::shared_ptr<GPUDeviceBase> device,
                      GPUHandle::ErrorTag tag,
                      std::string_view label);

    bool validateFinish() const;

    GPUEncodingContext encoding_context_;
    std::unordered_set<GPUBufferBase *> top_level_buffers_;
    std::unordered_set<GPUTextureBase *> top_level_textures_;
    // std::unordered_set<GPUQuerySetBase *> used_query_sets_;

    uint64_t debug_group_stack_size_ = 0;
    GPUUsageValidationMode usage_validation_mode_;
  };
}
