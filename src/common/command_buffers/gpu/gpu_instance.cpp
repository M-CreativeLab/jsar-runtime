#include <common/command_buffers/gpu/gpu_instance.hpp>

using namespace std;

namespace commandbuffers
{
  unique_ptr<GPUInstanceBase, GPUInstanceBase::Deleter> GPUInstanceBase::Create(const GPUInstanceDescriptor *descriptor)
  {
    static constexpr GPUInstanceDescriptor kDefaultDesc = {};
    if (descriptor == nullptr)
    {
      descriptor = &kDefaultDesc;
    }

    auto instance = unique_ptr<GPUInstanceBase, Deleter>(new GPUInstanceBase());
    instance->initialize(*descriptor);
    return instance;
  }

  void GPUInstanceBase::addDevice(shared_ptr<GPUDeviceBase>)
  {
  }

  void GPUInstanceBase::removeDevice(shared_ptr<GPUDeviceBase>)
  {
  }

  bool GPUInstanceBase::hasFeature(GPUFeatureName feature) const
  {
    return instance_features_.find(feature) != instance_features_.end();
  }

  GPUInstanceBase::GPUInstanceBase()
  {
  }

  void GPUInstanceBase::initialize(const GPUInstanceDescriptor &descriptor)
  {
  }
}
