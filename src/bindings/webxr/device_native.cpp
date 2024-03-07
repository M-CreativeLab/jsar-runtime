#include "device_native.hpp"
#include "xr/device.hpp"

namespace bindings
{
  Napi::FunctionReference *XRDeviceNative::constructor;

  Napi::Object XRDeviceNative::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "XRDeviceNative", {InstanceMethod("isSessionSupported", &XRDeviceNative::IsSessionSupported), InstanceMethod("requestSession", &XRDeviceNative::RequestSession)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);

    exports.Set("XRDeviceNative", tpl);
    return exports;
  }

  XRDeviceNative::XRDeviceNative(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRDeviceNative>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
  }

  Napi::Value XRDeviceNative::IsSessionSupported(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return env.Undefined();
  }

  Napi::Value XRDeviceNative::RequestSession(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsNumber())
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestSession: expected a number")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto id = info[0].As<Napi::Number>().Int32Value();
    auto device = xr::Device::GetInstance();
    if (device == nullptr)
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestSession: device is not initialized")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    return Napi::Boolean::New(env, device->requestSession(id));
  }
}
