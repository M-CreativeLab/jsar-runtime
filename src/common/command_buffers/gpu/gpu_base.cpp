#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_instance.hpp>
#include <common/command_buffers/gpu/gpu_device.hpp>

using namespace std;

namespace commandbuffers
{
  //
  // ErrorMonad Implementation
  //

  ErrorMonad::ErrorMonad()
      : tag_payload_(kInitializedNoErrorPayload)
  {
  }

  ErrorMonad::ErrorMonad(ErrorTag)
      : tag_payload_(kInitializedErrorPayload)
  {
  }
  ErrorMonad::ErrorMonad(DelayedInitializationTag)
      : tag_payload_(kNotInitializedPayload)
  {
  }

  bool ErrorMonad::initialized() const
  {
    return (tag_payload_ & kInitializedMask) == kInitialized;
  }

  bool ErrorMonad::isError() const
  {
    assert((tag_payload_ & kInitializedMask) == kInitialized);
    return tag_payload_ != kInitializedNoErrorPayload;
  }

  void ErrorMonad::setInitializedError()
  {
    uint64_t previousPayload = fetchAnd(kInitializedErrorPayload);
    assert(previousPayload == kNotInitializedPayload);
  }

  void ErrorMonad::setInitializedNoError()
  {
    uint64_t previousPayload = fetchAnd(kInitializedNoErrorPayload);
    assert(previousPayload == kNotInitializedPayload);
  }

  uint64_t ErrorMonad::fetchAnd(uint64_t arg)
  {
    return tag_payload_.exchange(arg, memory_order_acq_rel);
  }

  //
  // GPUObject Implementation
  //

  GPUObject::GPUObject(shared_ptr<GPUDeviceBase> device)
      : ErrorMonad()
      , device_(device)
  {
  }

  GPUObject::GPUObject(shared_ptr<GPUDeviceBase> device, ErrorTag)
      : ErrorMonad(kError)
      , device_(device)
  {
  }

  GPUObject::GPUObject(shared_ptr<GPUDeviceBase> device, DelayedInitializationTag)
      : ErrorMonad(kDelayedInitialization)
      , device_(device)
  {
  }

  GPUInstanceBase *GPUObject::instance() const
  {
    return device_->instance();
  }

  shared_ptr<GPUDeviceBase> GPUObject::device() const
  {
    return device_;
  }

  //
  // GPUHandle Implementation
  //

  GPUHandle::GPUHandle(shared_ptr<GPUDeviceBase> device, string_view label)
      : GPUObject(device)
      , id_(Ids.get())
      , label_(string(label))
  {
  }

  GPUHandle::GPUHandle(shared_ptr<GPUDeviceBase> device,
                       ErrorTag tag,
                       string_view label)
      : GPUObject(device, tag)
      , id_(Ids.get())
      , label_(string(label))
  {
  }

  GPUHandle::GPUHandle(shared_ptr<GPUDeviceBase> device,
                       DelayedInitializationTag tag,
                       string_view label)
      : GPUObject(device, tag)
      , id_(Ids.get())
      , label_(string(label))
  {
  }

  GPUHandle::GPUHandle(shared_ptr<GPUDeviceBase> device, LabelNotImplementedTag tag)
      : GPUObject(device)
      , id_(Ids.get())
  {
  }

  void GPUHandle::setLabel(std::string label)
  {
    label_ = move(label);
  }

  const std::string &GPUHandle::getLabel() const
  {
    return label_;
  }
}
