#include <renderer/gles/gpu_physical_device_impl.hpp>

using namespace std;
using namespace commandbuffers;

namespace gles
{
  // static
  Ref<GPUPhysicalDeviceImpl> GPUPhysicalDeviceImpl::Create(GPUBackendType backendType,
                                                           Ref<DisplayEGL> display,
                                                           bool forceES31AndMinExtensions)
  {
    return nullptr;
  }

  GPUPhysicalDeviceImpl::GPUPhysicalDeviceImpl(GPUBackendType backendType, Ref<DisplayEGL> display)
      : PhysicalDeviceBase(backendType)
  {
  }

  DisplayEGL *GPUPhysicalDeviceImpl::display() const
  {
    return display_.get();
  }

  bool GPUPhysicalDeviceImpl::supportsExternalImages() const
  {
    return false;
  }

  bool GPUPhysicalDeviceImpl::supportsFeatureLevel(GPUFeatureLevel featureLevel,
                                                   GPUInstance *instance) const
  {
    return false;
  }

  void GPUPhysicalDeviceImpl::initializeImpl()
  {
  }

  void GPUPhysicalDeviceImpl::initializeSupportedFeaturesImpl()
  {
  }

  Ref<GPUDeviceBase> GPUPhysicalDeviceImpl::createDeviceImpl(Ref<GPUAdapterBase> adapter,
                                                             const GPUDeviceDescriptor &descriptor)
  {
    return nullptr;
  }
}
