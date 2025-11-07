#include <common/command_buffers/gpu/gpu_base.hpp>
#include <renderer/gles/gpu_backend_impl.hpp>
#include <renderer/gles/gpu_display_egl.hpp>
#include <renderer/gles/gpu_physical_device_impl.hpp>

using namespace std;
using namespace commandbuffers;

namespace gles
{
  GPUBackend::GPUBackend(GPUInstance *instance, GPUBackendType type)
      : BackendConnection(instance, type)
  {
  }

  vector<Ref<gpu::PhysicalDeviceBase>> GPUBackend::discoverPhysicalDevices(const RequestAdapterOptions &options)
  {
    if (options.forceFallbackAdapter)
      return {};
    if (options.featureLevel != GPUFeatureLevel::kCompatibility)
      return {};

    vector<Ref<gpu::PhysicalDeviceBase>> devices;
    {
      Ref<DisplayEGL> display = DisplayEGL::CreateFromCurrent();
      Ref<gpu::PhysicalDeviceBase> newDevice = GPUPhysicalDeviceImpl::Create(type(), display, true);
      devices.push_back(newDevice);
    }
    return devices;
  }

  gpu::BackendConnection *Connect(GPUInstance *instance, GPUBackendType type)
  {
    assert(type == GPUBackendType::kOpenGL || type == GPUBackendType::kOpenGLES);
    return new GPUBackend(instance, type);
  }
}
