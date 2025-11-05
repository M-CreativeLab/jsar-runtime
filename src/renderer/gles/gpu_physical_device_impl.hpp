#pragma once

#include <common/command_buffers/gpu/gpu_physical_device.hpp>
#include <common/command_buffers/gpu/gpu_adapter.hpp>

namespace gles
{
  class GPUPhysicalDeviceImpl final : public commandbuffers::GPUPhysicalDeviceBase
  {
  public:
    bool supportsExternalImages() const override;

  private:
    std::unique_ptr<GPUDeviceBase> createDeviceImpl(std::shared_ptr<GPUAdapterBase> adapter,
                                                    GPUDeviceDescriptor &descriptor) override;
    void initializeImpl() override;
    void initializeSupportedFeaturesImpl() override;
  };
}
