#include "input_source.hpp"
#include "space.hpp"

namespace bindings
{
  Napi::FunctionReference *XRInputSource::constructor;
  Napi::FunctionReference *XRInputSourceArray::constructor;

  Napi::Object XRInputSource::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function func = DefineClass(env, "XRInputSource", {});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("XRInputSource", func);
    return exports;
  }

  Napi::Object XRInputSource::NewInstance(Napi::Env env, xr::DeviceFrame *frame,
                                          InputSourceInternalResetCallback resetInternal)
  {
    Napi::EscapableHandleScope scope(env);
    auto frameExternal = Napi::External<xr::DeviceFrame>::New(env, frame);
    auto resetInternalExternal = Napi::External<InputSourceInternalResetCallback>::New(env, &resetInternal);
    Napi::Object obj = constructor->New({frameExternal, resetInternalExternal});
    return scope.Escape(obj).ToObject();
  }

  XRInputSource::XRInputSource(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRInputSource>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 2)
    {
      Napi::TypeError::New(env, "XRInputSource constructor expects 1 argument").ThrowAsJavaScriptException();
      return;
    }
    if (!info[0].IsExternal() || !info[1].IsExternal())
    {
      Napi::TypeError::New(env, "XRInputSource constructor could not be called").ThrowAsJavaScriptException();
      return;
    }

    auto frameExternal = info[0].As<Napi::External<xr::DeviceFrame>>();
    frame = frameExternal.Data();

    auto resetInternalExternal = info[1].As<Napi::External<InputSourceInternalResetCallback>>();
    onResetInternal = *resetInternalExternal.Data();
    updateInternal(frame); // Update internal once when the input source is created

    auto thisObject = info.This().ToObject();
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("gamepad", GamepadGetter(info), napi_enumerable));
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("gripSpace", GripSpaceGetter(info), napi_enumerable));
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("hand", HandGetter(info), napi_enumerable));
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("handedness", HandednessGetter(info), napi_enumerable));
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("targetRayMode", TargetRayModeGetter(info), napi_enumerable));
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("targetRaySpace", TargetRaySpaceGetter(info), napi_enumerable));
  }

  XRInputSource::~XRInputSource()
  {
    delete internal;
    internal = nullptr;
  }

  Napi::Value XRInputSource::GamepadGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  Napi::Value XRInputSource::GripSpaceGetter(const Napi::CallbackInfo &info)
  {
    return XRTargetRayOrGripSpace::NewInstance(info.Env(), internal, true);
  }

  Napi::Value XRInputSource::HandGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Value XRInputSource::HandednessGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    auto handness = internal->handness;
    if (handness == xr::Handness::Left)
      return Napi::String::New(env, "left");
    else if (handness == xr::Handness::Right)
      return Napi::String::New(env, "right");
    else
      return Napi::String::New(env, "none");
  }

  Napi::Value XRInputSource::TargetRayModeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    auto targetRayMode = internal->targetRayMode;
    if (targetRayMode == xr::TargetRayMode::Gaze)
      return Napi::String::New(env, "gaze");
    else if (targetRayMode == xr::TargetRayMode::TrackedPointer)
      return Napi::String::New(env, "tracked-pointer");
    else if (targetRayMode == xr::TargetRayMode::Screen)
      return Napi::String::New(env, "screen");
    else
      return Napi::String::New(env, "tracked-pointer");
  }

  Napi::Value XRInputSource::TargetRaySpaceGetter(const Napi::CallbackInfo &info)
  {
    return XRTargetRayOrGripSpace::NewInstance(info.Env(), internal, false);
  }

  bool XRInputSource::updateInternal(xr::DeviceFrame *frame)
  {
    if (!onResetInternal)
      return false;

    auto newInternal = onResetInternal(frame);
    if (newInternal == nullptr)
      return false;

    if (internal == nullptr)
      internal = new xr::InputSource(newInternal);
    else
      internal->update(newInternal);
    return true;
  }

  XRInputSourceArray XRInputSourceArray::New(Napi::Env env)
  {
    napi_value value;
    napi_status status = napi_create_array(env, &value);
    NAPI_THROW_IF_FAILED(env, status, XRInputSourceArray());
    return XRInputSourceArray(env, value);
  }

  XRInputSourceArray::XRInputSourceArray(napi_env env, napi_value value) : Napi::Array(env, value)
  {
  }

  XRInputSourceArray::~XRInputSourceArray()
  {
  }

  void XRInputSourceArray::updateInputSources(xr::DeviceFrame *frame, InputSourcesChangedCallback onChangedCallback)
  {
    Napi::Env env = Env();
    if (Length() == 0)
    {
      vector<XRInputSource *> added;
      auto gazeInputSource = XRInputSource::Unwrap(
          XRInputSource::NewInstance(env, frame, [](xr::DeviceFrame *frame) -> xr::InputSource *
                                     { return &frame->getGazeInputSource(); }));
      auto leftHandInputSource = XRInputSource::Unwrap(
          XRInputSource::NewInstance(env, frame, [](xr::DeviceFrame *frame) -> xr::InputSource *
                                     { return &frame->getHandInputSource(xr::Handness::Left); }));
      auto rightHandInputSource = XRInputSource::Unwrap(
          XRInputSource::NewInstance(env, frame, [](xr::DeviceFrame *frame) -> xr::InputSource *
                                     { return &frame->getHandInputSource(xr::Handness::Right); }));

      added.push_back(gazeInputSource);
      added.push_back(leftHandInputSource);
      added.push_back(rightHandInputSource);
      onChangedCallback(added, {});

      for (uint32_t i = 0; i < added.size(); i++)
        Set(i, added[i]->Value());
      Set("length", Napi::Value::From(env, added.size()));
    }
    else
    {
      // When the length is not zero, we need to update the internal input sources
      for (uint32_t i = 0; i < Length(); i++)
      {
        auto inputSource = XRInputSource::Unwrap(Get(i).ToObject());
        inputSource->updateInternal(frame);
      }
    }
  }
}
