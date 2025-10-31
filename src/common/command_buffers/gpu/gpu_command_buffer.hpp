#pragma once

#include <vector>
#include <memory>
#include <string_view>
#include <string>

#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_buffer.hpp>
#include <common/command_buffers/gpu/gpu_pipeline.hpp>
#include <common/command_buffers/gpu/gpu_commands.hpp>
#include <common/command_buffers/gpu/gpu_command_encoder.hpp>
#include <common/command_buffers/gpu/command_allocator.hpp>
#include <common/command_buffers/gpu/indirect_draw_metadata.hpp>
#include <common/command_buffers/gpu/pass_resource_usage.hpp>

namespace commandbuffers
{
  struct GPUCommandBufferDescriptor
  {
    std::string_view label;
  };

  class GPUCommandBufferBase : public GPUHandle
  {
    friend class GPUComputePassEncoderBase;
    friend class GPURenderPassEncoderBase;

  public:
    GPUCommandBufferBase(GPUCommandEncoderBase *encoder,
                         const GPUCommandBufferDescriptor *descriptor);
    virtual ~GPUCommandBufferBase() = default;

    GPUHandleType type() const override final
    {
      return GPUHandleType::kCommandBuffer;
    }

    const std::string &getEncoderLabel() const;
    void setEncoderLabel(std::string encoderLabel);

    const GPUCommandBufferResourceUsage &getResourceUsages() const;
    const std::vector<GPUIndirectDrawMetadata> &getIndirectDrawMetadata();

    template <typename F>
    auto useCommands(F func) -> auto
    {
      auto result = func(commands_);
      commands_.reset();
      return result;
    }

  protected:
    gpu::CommandIterator commands_;

  private:
    GPUCommandBufferBase(std::shared_ptr<GPUDeviceBase> device,
                         GPUHandle::ErrorTag tag,
                         std::string_view label);

    GPUCommandBufferResourceUsage resource_usages_;
    std::vector<GPUIndirectDrawMetadata> indirect_draw_metadata_;
    std::string encoder_label_;
  };
}
