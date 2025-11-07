#pragma once

#include <string>
#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_device.hpp>

namespace gles
{
  class GPUDeviceImpl final : public commandbuffers::GPUDeviceBase
  {
  public:
    static Ref<GPUDeviceImpl> Create(Ref<commandbuffers::GPUAdapterBase> adapter,
                                     const commandbuffers::GPUDeviceDescriptor &descriptor);

  private:
    GPUDeviceImpl(Ref<commandbuffers::GPUAdapterBase> adapter,
                  const commandbuffers::GPUDeviceDescriptor &descriptor);

    bool initialize(const commandbuffers::GPUDeviceDescriptor &descriptor);

  public:
    std::unique_ptr<commandbuffers::GPUCommandBufferBase> createCommandBuffer(
      commandbuffers::GPUCommandEncoder &encoder,
      const commandbuffers::GPUCommandBufferDescriptor *descriptor = nullptr) override;
  };
}
