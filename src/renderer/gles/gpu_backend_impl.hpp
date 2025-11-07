#pragma once

#include <vector>
#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_instance.hpp>
#include <common/command_buffers/gpu/backend_connection.hpp>

namespace gles
{
  class GPUBackend final : public commandbuffers::gpu::BackendConnection
  {
  public:
    GPUBackend(commandbuffers::GPUInstance *instance, commandbuffers::GPUBackendType type);

    std::vector<Ref<commandbuffers::gpu::PhysicalDeviceBase>> discoverPhysicalDevices(
      const commandbuffers::RequestAdapterOptions &options) override;
  };
}
