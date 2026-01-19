#include <common/command_buffers/gpu/gpu_bind_group_layout.hpp>

using namespace std;

namespace commandbuffers
{
  GPUBindGroupLayoutBase::GPUBindGroupLayoutBase(Ref<GPUDeviceBase> device,
                                                 string_view label,
                                                 Ref<GPUBindGroupLayoutInternalBase> internal)
      : GPUHandle(device, label)
      , internal_layout_(internal)
  {
  }

  GPUBindGroupLayoutBase::GPUBindGroupLayoutBase(Ref<GPUDeviceBase> device,
                                                 GPUHandle::ErrorTag tag,
                                                 string_view label)
      : GPUHandle(device, tag, label)
  {
  }

  GPUBindGroupLayoutInternalBase *GPUBindGroupLayoutBase::getInternalBindGroupLayout() const
  {
    return internal_layout_.get();
  }

  bool GPUBindGroupLayoutBase::equal(const GPUBindGroupLayoutBase *other,
                                     bool excludePipelineCompatibiltyToken) const
  {
    return getInternalBindGroupLayout() == other->getInternalBindGroupLayout();
  }

  bool GPUBindGroupLayoutBase::isEmpty() const
  {
    return internal_layout_ == nullptr || internal_layout_->isEmpty();
  }
}
