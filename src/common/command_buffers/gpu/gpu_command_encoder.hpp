#pragma once

#include <memory>

#include "./gpu_base.hpp"
#include "./gpu_command_buffer.hpp"
#include "./gpu_pass_encoder_base.hpp"
#include "./gpu_renderpass_encoder.hpp"

namespace commandbuffers
{
  class GPUCommandEncoder : public GPUHandle
  {
    using GPUHandle::GPUHandle;

  public:
    virtual ~GPUCommandEncoder() = default;

  public:
    // TODO(yorkie): begineComputePass
    virtual GPURenderPassEncoder beginRenderPass(GPURenderPassDescriptor &) = 0;
    virtual std::unique_ptr<GPUCommandBuffer> finish(std::optional<std::string> label = std::nullopt) const = 0;

  protected:
    std::shared_ptr<GPUPassEncoderBase> current_pass_encoder_;
  };
}
