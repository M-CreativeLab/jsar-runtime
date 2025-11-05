#include <renderer/gles/gpu_backend_impl.hpp>

using namespace std;
using namespace commandbuffers;

namespace gles
{
  GPUBackend::GPUBackend(GPUInstance *instance, GPUBackendType type)
      : BackendConnection(instance, type)
  {
  }

  vector<Ref<GPUPhysicalDeviceBase>> GPUBackend::discoverPhysicalDevices(const gpu::RequestAdapterOptions &options)
  {
    return {};
  }

  gpu::BackendConnection *Connect(GPUInstance *instance, GPUBackendType type)
  {
    assert(type == GPUBackendType::kOpenGL || type == GPUBackendType::kOpenGLES);
    return new GPUBackend(instance, type);
  }
}
