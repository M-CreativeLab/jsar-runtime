#include <common/command_buffers/gpu/gpu_bind_group_layout_internal.hpp>

using namespace std;

namespace commandbuffers
{
  GPUBindGroupLayoutInternalBase::GPUBindGroupLayoutInternalBase(shared_ptr<GPUDeviceBase> device,
                                                                 const GPUBindGroupLayoutDescriptor &descriptor)
      : GPUHandle(device, descriptor.label)
  {
  }

  GPUBindGroupLayoutInternalBase::~GPUBindGroupLayoutInternalBase() = default;

  const GPUBindingInfo &GPUBindGroupLayoutInternalBase::getBindingInfo(GPUBindingIndex bindingIndex) const
  {
    assert(!isError());
    // Assert that this is an internal binding.
    assert(bindingIndex < getBindingCount());
    return binding_info_.at(bindingIndex);
  }

  const GPUBindingInfo &GPUBindGroupLayoutInternalBase::getClientBindingInfo(GPUClientBindingIndex bindingIndex) const
  {
    assert(!isError());
    GPUBindingIndex index = GPUBindingIndex(uint32_t(bindingIndex));
    assert(index < binding_info_.size());

    // Assert this is a user-facing binding and not an private internal binding.
    assert(binding_map_.contains(binding_info_.at(index).binding));
    return binding_info_.at(index);
  }

  const GPUBindGroupLayoutInternalBase::BindingMap &GPUBindGroupLayoutInternalBase::getBindingMap() const
  {
    assert(!isError());
    return binding_map_;
  }

  GPUBindingIndex GPUBindGroupLayoutInternalBase::asBindingIndex(GPUClientBindingIndex bindingIndex) const
  {
    assert(!isError());
    // Assert this is a user-facing binding and not a private internal binding, and that it
    // represents an internal bindings.
    GPUBindingIndex index = GPUBindingIndex(uint32_t(bindingIndex));
    assert(index < getBindingCount());
    assert(binding_map_.contains(binding_info_.at(index).binding));
    return index;
  }

  GPUClientBindingIndex GPUBindGroupLayoutInternalBase::getClientBindingIndex(GPUBindingNumber bindingNumber) const
  {
    assert(!isError());
    const auto &it = binding_map_.find(bindingNumber);
    assert(it != binding_map_.end());
    return it->second;
  }

  GPUBindingIndex GPUBindGroupLayoutInternalBase::getBindingCount() const
  {
    assert(!isError());
    return getBindingTypeStart(GPUBindingTypeOrder_ExternalTexture);
  }


  bool GPUBindGroupLayoutInternalBase::isEmpty() const
  {
    assert(!isError());
    return binding_info_.empty() && !has_dynamic_array_;
  }

  string GPUBindGroupLayoutInternalBase::entriesToString() const
  {
    string entries = "[";
    string sep = "";
    const GPUBindGroupLayoutInternalBase::BindingMap &bindingMap = getBindingMap();
    for (const auto [bindingNumber, bindingIndex] : bindingMap)
    {
      const GPUBindingInfo &bindingInfo = getClientBindingInfo(bindingIndex);
      entries += sep + bindingInfo.toString();
      sep = ", ";
    }
    entries += "]";
    return entries;
  }

  GPUBindingIndex GPUBindGroupLayoutInternalBase::getBindingTypeStart(GPUBindingTypeOrder type) const
  {
    return binding_type_start_[type];
  }

  GPUBindingIndex GPUBindGroupLayoutInternalBase::getBindingTypeEnd(GPUBindingTypeOrder type) const
  {
    return binding_type_start_[GPUBindingTypeOrder(static_cast<uint32_t>(type) + 1)];
  }
}
