#include <array>

#include "./input_source.hpp"
#include "./space.hpp"
#include "./session.hpp"
#include "./frame.hpp"
#include "./hand.hpp"
#include "./rigid_transform.hpp"

namespace bindings
{
  using namespace std;
  using namespace Napi;

  thread_local FunctionReference *XRInputSource::constructor;
  thread_local FunctionReference *XRInputSourceArray::constructor;

  // static
  void XRInputSource::Init(Napi::Env env)
  {
#define MODULE_NAME "XRInputSource"
    Function func = DefineClass(env, MODULE_NAME,
                                {
                                    InstanceAccessor("hand", &XRInputSource::HandGetter, nullptr),
                                    InstanceAccessor("gamepad", &XRInputSource::GamepadGetter, nullptr),
                                });
    constructor = new FunctionReference();
    *constructor = Persistent(func);
    env.Global().Set(MODULE_NAME, func);
#undef MODULE_NAME
  }

  // static
  Object XRInputSource::NewInstance(Napi::Env env, shared_ptr<client_xr::XRInputSource> handle)
  {
    return XRHandleWrap<XRInputSource, client_xr::XRInputSource>::NewInstance(env, handle);
  }

  XRInputSource::XRInputSource(const Napi::CallbackInfo &info)
      : XRHandleWrap<XRInputSource, client_xr::XRInputSource>(info)
  {
    auto env = info.Env();
    HandleScope scope(env);

    auto thisObject = info.This().ToObject();
    thisObject.DefineProperty(
        Napi::PropertyDescriptor::Value("gripSpace", GripSpaceGetter(info), napi_enumerable));
    thisObject.DefineProperty(
        Napi::PropertyDescriptor::Value("handedness", HandednessGetter(info), napi_enumerable));
    thisObject.DefineProperty(
        Napi::PropertyDescriptor::Value("targetRayMode", TargetRayModeGetter(info), napi_enumerable));
    thisObject.DefineProperty(
        Napi::PropertyDescriptor::Value("targetRaySpace", TargetRaySpaceGetter(info), napi_enumerable));
  }

  Napi::Value XRInputSource::GamepadGetter(const Napi::CallbackInfo &info)
  {
    auto env = info.Env();
    return env.Undefined();
  }

  Napi::Value XRInputSource::GripSpaceGetter(const Napi::CallbackInfo &info)
  {
    return XRTargetRayOrGripSpace::NewInstance(info.Env(), handle_->gripSpace());
  }

  Napi::Value XRInputSource::HandGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    HandleScope scope(env);
    return XRHand::NewInstance(env, handle_);
  }

  Napi::Value XRInputSource::HandednessGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    auto handness = handle_->handedness();
    if (handness == client_xr::XRHandedness::Left)
      return Napi::String::New(env, "left");
    else if (handness == client_xr::XRHandedness::Right)
      return Napi::String::New(env, "right");
    else
      return Napi::String::New(env, "none");
  }

  Napi::Value XRInputSource::TargetRayModeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    auto targetRayMode = handle_->targetRayMode();
    if (targetRayMode == client_xr::XRTargetRayMode::Gaze)
      return Napi::String::New(env, "gaze");
    else if (targetRayMode == client_xr::XRTargetRayMode::TrackedPointer)
      return Napi::String::New(env, "tracked-pointer");
    else if (targetRayMode == client_xr::XRTargetRayMode::TransientPointer)
      return Napi::String::New(env, "transient-pointer");
    else if (targetRayMode == client_xr::XRTargetRayMode::Screen)
      return Napi::String::New(env, "screen");
    else
      return Napi::String::New(env, "tracked-pointer");
  }

  Napi::Value XRInputSource::TargetRaySpaceGetter(const Napi::CallbackInfo &info)
  {
    return XRTargetRayOrGripSpace::NewInstance(info.Env(), handle_->targetRaySpace());
  }

  XRInputSourceArray XRInputSourceArray::New(Napi::Env env, std::optional<client_xr::XRInputSourceArray> handle)
  {
    napi_value value;
    napi_status status = napi_create_array(env, &value);
    NAPI_THROW_IF_FAILED(env, status, XRInputSourceArray());
    XRInputSourceArray inputSources(env, value);
    inputSources.update(handle);
    return inputSources;
  }

  XRInputSourceArray::XRInputSourceArray(napi_env env, napi_value value) : Napi::Array(env, value)
  {
  }

  void XRInputSourceArray::update(std::optional<client_xr::XRInputSourceArray> handle)
  {
    if (!handle.has_value())
      return;

    auto env = Env();
    HandleScope scope(env);
    auto inputSources = handle.value();

    // Remove the rest of the elements
    uint32_t length = Length();
    for (size_t i = inputSources.size(); i < length; i++)
      Delete(i);

    // Set the length to zero
    Set("length", Number::New(env, 0));

    // Update the elements from the input sources
    for (size_t i = 0; i < inputSources.size(); i++)
      Set(i, XRInputSource::NewInstance(env, inputSources[i]));
  }
}
