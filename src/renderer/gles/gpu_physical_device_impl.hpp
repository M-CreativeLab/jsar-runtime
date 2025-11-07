#pragma once

#include <common/utility.hpp>
#include <common/command_buffers/gpu/physical_device.hpp>
#include <common/command_buffers/gpu/gpu_adapter.hpp>
#include <common/command_buffers/gpu/gpu_instance.hpp>
#include <renderer/gles/gpu_display_egl.hpp>

namespace gles
{
  class GPUPhysicalDeviceImpl final : public commandbuffers::gpu::PhysicalDeviceBase
  {
  public:
    static Ref<GPUPhysicalDeviceImpl> Create(commandbuffers::GPUBackendType backendType,
                                             Ref<DisplayEGL> display,
                                             bool forceES31AndMinExtensions);

    DisplayEGL *display() const;

    bool supportsExternalImages() const override;
    bool supportsFeatureLevel(commandbuffers::GPUFeatureLevel featureLevel,
                              commandbuffers::GPUInstance *instance) const override;

  private:
    GPUPhysicalDeviceImpl(commandbuffers::GPUBackendType backendType,
                          Ref<DisplayEGL> display);

    void initializeImpl() override;
    void initializeSupportedFeaturesImpl() override;

    Ref<commandbuffers::GPUDeviceBase> createDeviceImpl(
      Ref<commandbuffers::GPUAdapterBase> adapter,
      const commandbuffers::GPUDeviceDescriptor &descriptor) override;

  private:
    Ref<DisplayEGL> display_;
  };
}
