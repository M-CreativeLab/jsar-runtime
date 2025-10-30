#pragma once

#include <string>
#include <optional>
#include <memory>
#include <common/command_buffers/gpu/gpu_command_encoder.hpp>
#include "./gpu_command_buffer_impl.hpp"

namespace gles
{
  class GPUCommandEncoderImpl : public commandbuffers::gpu::GPUCommandEncoderBase
  {
    friend class GPUDeviceImpl;
    using commandbuffers::gpu::GPUCommandEncoderBase::GPUCommandEncoderBase;

  public:
    commandbuffers::gpu::GPURenderPassEncoderBase beginRenderPass(commandbuffers::gpu::GPURenderPassDescriptor &) override;
    std::unique_ptr<commandbuffers::gpu::GPUCommandBufferBase> finish(std::optional<std::string> label = std::nullopt) const override;

  public:
    commandbuffers::gpu::GPURenderPassEncoderBase &getOrStartRecordingRenderPass();
    bool isRenderPassWith(GLuint target_framebuffer) const;
  };
}
