#pragma once

#include <string>
#include <optional>
#include <memory>
#include <common/command_buffers/gpu/gpu_command_encoder.hpp>

namespace gles
{
  class GPUCommandEncoderImpl : public commandbuffers::GPUCommandEncoder
  {
    friend class GPUDeviceImpl;
    using commandbuffers::GPUCommandEncoder::GPUCommandEncoder;

  public:
    commandbuffers::GPURenderPassEncoder beginRenderPass(commandbuffers::GPURenderPassDescriptor &) override;
    commandbuffers::GPUCommandBuffer finish(std::optional<std::string> label = std::nullopt) override;

  public:
    commandbuffers::GPURenderPassEncoder &getOrStartRecordingRenderPass();

  private:
    std::unique_ptr<commandbuffers::GPURenderPassEncoder> recording_renderpass_encoder_ = nullptr;
  };
}
