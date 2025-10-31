#include <common/command_buffers/gpu/gpu_bind_group_layout.hpp>

using namespace std;

namespace commandbuffers
{
  GPUBindGroupLayoutBase::GPUBindGroupLayoutBase(shared_ptr<GPUDeviceBase> device,
                                                 string_view label,
                                                 shared_ptr<GPUBindGroupLayoutInternalBase> internal)
      : GPUHandle(device, label)
      , internal_layout_(internal)
  {
  }

  GPUBindGroupLayoutBase::GPUBindGroupLayoutBase(shared_ptr<GPUDeviceBase> device,
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
                                     bool excludePipelineCompatibiltyToken = false) const
  {
    return getInternalBindGroupLayout() == other->getInternalBindGroupLayout();
  }

  bool GPUBindGroupLayoutBase::isEmpty() const
  {
    return internal_layout_ == nullptr || internal_layout_->isEmpty();
  }
}
