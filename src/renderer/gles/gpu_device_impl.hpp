#pragma once

#include <string>
#include <common/command_buffers/gpu/gpu_device.hpp>

namespace gles
{
  class GPUDeviceImpl final : public commandbuffers::GPUDeviceBase
  {
  private:
    GPUDeviceImpl();

  public:
    commandbuffers::GPUHandleType type() const override
    {
      return commandbuffers::GPUHandleType::kDevice;
    }

  public:
    std::unique_ptr<commandbuffers::GPUCommandEncoderBase> createCommandEncoder(std::optional<std::string> label) override;
  };
}
