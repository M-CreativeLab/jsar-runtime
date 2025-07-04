#pragma once

#include "./gpu_base.hpp"
#include "./gpu_command_buffer.hpp"
#include "./gpu_renderpass_encoder.hpp"

namespace commandbuffers
{
  class GPUCommandEncoder : public GPUHandle
  {
    using GPUHandle::GPUHandle;

  public:
    virtual ~GPUCommandEncoder() = default;

  public:
    virtual GPURenderPassEncoder beginRenderPass(GPURenderPassDescriptor &) = 0;
    virtual GPUCommandBuffer finish(std::optional<std::string> label = std::nullopt) = 0;
  };
}
