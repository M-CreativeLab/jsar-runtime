#include "input_source.hpp"

namespace bindings
{
  Napi::FunctionReference *XRInputSource::constructor;
  Napi::FunctionReference *XRInputSourceArray::constructor;

  Napi::Object XRInputSource::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function func = DefineClass(env, "XRInputSource", {
                                                                InstanceAccessor("gamepad", &XRInputSource::GamepadGetter, nullptr),
                                                                InstanceAccessor("gripSpace", &XRInputSource::GripSpaceGetter, nullptr),
                                                                InstanceAccessor("hand", &XRInputSource::HandGetter, nullptr),
                                                                InstanceAccessor("handness", &XRInputSource::HandnessGetter, nullptr),
                                                                InstanceAccessor("targetRayMode", &XRInputSource::TargetRayModeGetter, nullptr),
                                                                InstanceAccessor("targetRaySpace", &XRInputSource::TargetRaySpaceGetter, nullptr),
                                                            });
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("XRInputSource", func);
    return exports;
  }

  Napi::Object XRInputSource::NewInstance(Napi::Env env, xr::InputSource &inputSource)
  {
    Napi::EscapableHandleScope scope(env);
    auto instance = Napi::External<xr::InputSource>::New(env, &inputSource);
    Napi::Object obj = constructor->New({instance});
    return scope.Escape(obj).ToObject();
  }

  XRInputSource::XRInputSource(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRInputSource>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1)
    {
      Napi::TypeError::New(env, "XRInputSource constructor expects 1 argument").ThrowAsJavaScriptException();
      return;
    }
    if (!info[0].IsExternal())
    {
      Napi::TypeError::New(env, "XRInputSource constructor could not be called").ThrowAsJavaScriptException();
      return;
    }

    auto external = info[0].As<Napi::External<xr::InputSource>>();
    internal = external.Data();
  }

  XRInputSource::~XRInputSource()
  {
  }

  Napi::Value XRInputSource::GamepadGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Value XRInputSource::GripSpaceGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Value XRInputSource::HandGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Value XRInputSource::HandnessGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Value XRInputSource::TargetRayModeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Value XRInputSource::TargetRaySpaceGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  XRInputSourceArray XRInputSourceArray::New(Napi::Env env)
  {
    napi_value value;
    napi_status status = napi_create_array(env, &value);
    NAPI_THROW_IF_FAILED(env, status, XRInputSourceArray());
    auto inputSourceArray = XRInputSourceArray(env, value);
    inputSourceArray.updateInputSourcesIfChanged();
    return inputSourceArray;
  }

  XRInputSourceArray::XRInputSourceArray(napi_env env, napi_value value) : Napi::Array(env, value)
  {
    device = xr::Device::GetInstance();
    if (device == nullptr)
    {
      Napi::Error::New(env, "XRInputSourceArray: Device is not initialized").ThrowAsJavaScriptException();
      return;
    }
  }

  XRInputSourceArray::~XRInputSourceArray()
  {
  }

  void XRInputSourceArray::updateInputSourcesIfChanged()
  {
    if (device == nullptr || !device->enabled())
      return;

    Napi::Env env = Env();
    Set(uint32_t(0), XRInputSource::NewInstance(env, device->getGazeInputSource()));
    Set(uint32_t(1), XRInputSource::NewInstance(env, device->getHandInputSource(xr::Handness::Left)));
    Set(uint32_t(2), XRInputSource::NewInstance(env, device->getHandInputSource(xr::Handness::Right)));
    Set("length", Napi::Value::From(env, 3));
  }
}
