#pragma once

#include <string>
#include <common/command_buffers/gpu/gpu_device.hpp>

namespace gles
{
  class GPUQueueImpl : public commandbuffers::GPUQueue
  {
  public:
    GPUQueueImpl();
    void submit(const std::vector<commandbuffers::GPUCommandBuffer> &) override;
  };

  class GPUDeviceImpl : public commandbuffers::GPUDevice
  {
  public:
    GPUDeviceImpl();

  public:
    std::unique_ptr<commandbuffers::GPUCommandEncoder> createCommandEncoder(std::optional<std::string> label) override;
  };
}
