#include <renderer/gles/gpu_physical_device_impl.hpp>

using namespace std;
using namespace commandbuffers;

namespace gles
{
  bool GPUPhysicalDeviceImpl::supportsExternalImages() const
  {
    return false;
  }

  unique_ptr<GPUDeviceBase> GPUPhysicalDeviceImpl::createDeviceImpl(shared_ptr<GPUAdapterBase> adapter,
                                                                    GPUDeviceDescriptor &descriptor)
  {
    return nullptr;
  }

  void GPUPhysicalDeviceImpl::initializeImpl()
  {
  }

  void GPUPhysicalDeviceImpl::initializeSupportedFeaturesImpl()
  {
  }

}
