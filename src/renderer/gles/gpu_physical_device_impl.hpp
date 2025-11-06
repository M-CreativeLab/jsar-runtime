#pragma once

#include <common/utility.hpp>
#include <common/command_buffers/gpu/physical_device.hpp>
#include <common/command_buffers/gpu/gpu_adapter.hpp>

namespace gles
{
  class GPUPhysicalDeviceImpl final : public commandbuffers::gpu::PhysicalDeviceBase
  {
  public:
    bool supportsExternalImages() const override;

  private:
    std::unique_ptr<commandbuffers::GPUDeviceBase> createDeviceImpl(
      Ref<commandbuffers::GPUAdapterBase> adapter,
      commandbuffers::GPUDeviceDescriptor &descriptor) override;

    void initializeImpl() override;
    void initializeSupportedFeaturesImpl() override;
  };
}
