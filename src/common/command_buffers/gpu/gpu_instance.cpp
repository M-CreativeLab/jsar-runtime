#include <common/command_buffers/gpu/gpu_instance.hpp>
#include <common/command_buffers/gpu/backend_connection.hpp>
#include <common/command_buffers/gpu/physical_device.hpp>

using namespace std;

namespace commandbuffers
{
  Ref<GPUInstance> GPUInstance::Create(const GPUInstanceDescriptor *descriptor)
  {
    static constexpr GPUInstanceDescriptor kDefaultDesc = {};
    if (descriptor == nullptr)
    {
      descriptor = &kDefaultDesc;
    }

    auto instance = Ref<GPUInstance>(new GPUInstance(), [](GPUInstance *ptr)
                                     { delete ptr; });
    instance->initialize(*descriptor);
    return instance;
  }

  void GPUInstance::registerBackend(gpu::BackendConnection *backend)
  {
    assert(backend != nullptr && "Backend connection cannot be null.");
    backend_ = std::unique_ptr<gpu::BackendConnection>(backend);
  }

  void GPUInstance::addDevice(Ref<GPUDeviceBase>)
  {
  }

  void GPUInstance::removeDevice(Ref<GPUDeviceBase>)
  {
  }

  bool GPUInstance::hasFeature(GPUFeatureName feature) const
  {
    return instance_features_.find(feature) != instance_features_.end();
  }

  GPUInstance::GPUInstance()
  {
  }

  void GPUInstance::initialize(const GPUInstanceDescriptor &descriptor)
  {
  }
}
