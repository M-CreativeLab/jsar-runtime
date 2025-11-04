#pragma once

#include <string>
#include <common/command_buffers/gpu/gpu_device.hpp>

namespace gles
{
  class GPUDeviceImpl final : public commandbuffers::GPUDeviceBase
  {
  private:
    GPUDeviceImpl(commandbuffers::GPUAdapterBase *adapter, commandbuffers::GPUDeviceDescriptor &descriptor);

  public:
    std::unique_ptr<commandbuffers::GPUCommandBufferBase> createCommandBuffer(
      commandbuffers::GPUCommandEncoder &encoder,
      const commandbuffers::GPUCommandBufferDescriptor *descriptor = nullptr) override;
  };
}
