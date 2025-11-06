#include <renderer/metal/gpu_backend_mtl.h>

using namespace std;
using namespace commandbuffers;

namespace metal
{
  GPUBackend::GPUBackend(GPUInstance *instance, GPUBackendType type)
      : BackendConnection(instance, type)
  {
  }

  vector<Ref<gpu::PhysicalDeviceBase>> GPUBackend::discoverPhysicalDevices(const gpu::RequestAdapterOptions &options)
  {
    return {};
  }

  gpu::BackendConnection *Connect(GPUInstance *instance, GPUBackendType type)
  {
    assert(type == GPUBackendType::kMetal);
    return new GPUBackend(instance, type);
  }
}
