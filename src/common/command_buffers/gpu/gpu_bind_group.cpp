#include <common/command_buffers/gpu/gpu_bind_group.hpp>

using namespace std;

namespace commandbuffers
{
  GPUBindGroupBase::GPUBindGroupBase(Ref<GPUDeviceBase> device,
                                     const GPUBindGroupDescriptor &descriptor,
                                     void *bindingDataStart)
      : GPUHandle(device, descriptor.label)
  {
  }

  GPUBindGroupBase::GPUBindGroupBase(Ref<GPUDeviceBase> device,
                                     GPUHandle::ErrorTag tag,
                                     string_view label)
      : GPUHandle(device, tag, label)
  {
  }
}
