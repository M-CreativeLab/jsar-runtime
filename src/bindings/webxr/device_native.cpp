#include "device_native.hpp"

namespace bindings
{
  Napi::FunctionReference *XRDeviceNative::constructor;

  Napi::Object XRDeviceNative::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "XRDeviceNative", {
      InstanceMethod("isSessionSupported", &XRDeviceNative::IsSessionSupported),
      InstanceMethod("requestSession", &XRDeviceNative::RequestSession)
    });

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

    // XRSessionMode mode = static_cast<XRSessionMode>(info[0].As<Napi::Number>().Int32Value());
    // xr::XRSystemNative *system = xr::XRSystemNative::GetInstance();
    // xr::XRSessionNative *session = system->requestSession(mode);
    // return XRSession::NewInstance(env, Napi::External<xr::XRSessionNative>::New(env, session));
    return env.Undefined();
  }
}
