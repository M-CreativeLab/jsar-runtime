#pragma once

#include <string>
#include <common/command_buffers/gpu/gpu_device.hpp>

namespace gles
{
  class GPUQueueImpl : public commandbuffers::gpu::GPUQueue
  {
  public:
    GPUQueueImpl();
    void submit(const std::vector<std::shared_ptr<commandbuffers::gpu::GPUCommandBufferBase>> &) override;
  };

  class GPUDeviceImpl : public commandbuffers::gpu::GPUDeviceBase
  {
  public:
    GPUDeviceImpl();

  public:
    std::unique_ptr<commandbuffers::gpu::GPUCommandEncoderBase> createCommandEncoder(std::optional<std::string> label) override;
  };
}
