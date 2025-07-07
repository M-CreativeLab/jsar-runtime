#pragma once

#include <string>
#include <optional>
#include <memory>
#include <common/command_buffers/gpu/gpu_command_encoder.hpp>
#include "./gpu_command_buffer_impl.hpp"

namespace gles
{
  class GPUCommandEncoderImpl : public commandbuffers::GPUCommandEncoder
  {
    friend class GPUDeviceImpl;
    using commandbuffers::GPUCommandEncoder::GPUCommandEncoder;

  public:
    commandbuffers::GPURenderPassEncoder beginRenderPass(commandbuffers::GPURenderPassDescriptor &) override;
    std::unique_ptr<commandbuffers::GPUCommandBuffer> finish(std::optional<std::string> label = std::nullopt) const override;

  public:
    commandbuffers::GPURenderPassEncoder &getOrStartRecordingRenderPass();
    bool isRenderPassWith(GLuint target_framebuffer) const;
  };
}
