#pragma once

#include <vector>
#include <common/utility.hpp>
#include <common/non_movable.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_instance.hpp>
#include <common/command_buffers/gpu/gpu_adapter.hpp>
#include <common/command_buffers/gpu/gpu_physical_device.hpp>

namespace commandbuffers::gpu
{
  struct RequestAdapterOptions
  {
    GPUFeatureLevel featureLevel = GPUFeatureLevel::kCore;
    GPUPowerPreference powerPreference = GPUPowerPreference::Undefined;
    bool forceFallbackAdapter = false;
    GPUBackendType backendType = GPUBackendType::kUndefined;
  };

  class BackendConnection : public NonMovable
  {
  public:
    BackendConnection(GPUInstance *instance, GPUBackendType type);
    virtual ~BackendConnection() = default;

    GPUBackendType type() const;
    GPUInstance *getInstance() const;

    virtual std::vector<Ref<GPUPhysicalDeviceBase>> discoverPhysicalDevices(const RequestAdapterOptions &) = 0;

  private:
    GPUInstance *instance_ = nullptr;
    GPUBackendType type_;
  };
}
