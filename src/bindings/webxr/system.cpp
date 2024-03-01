#include "system.hpp"

namespace bindings
{
  Napi::FunctionReference *XRSystem::constructor;

  Napi::Object XRSystem::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "XRSystem", {
      InstanceMethod("isSessionSupported", &XRSystem::IsSessionSupported),
      InstanceMethod("requestSession", &XRSystem::RequestSession)
    });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);

    exports.Set("XRSystem", tpl);
    return exports;
  }

  XRSystem::XRSystem(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRSystem>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
  }

  Napi::Value XRSystem::IsSessionSupported(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return env.Undefined();
  }

  Napi::Value XRSystem::RequestSession(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // XRSessionMode mode = static_cast<XRSessionMode>(info[0].As<Napi::Number>().Int32Value());
    // xr::XRSystemNative *system = xr::XRSystemNative::GetInstance();
    // xr::XRSessionNative *session = system->requestSession(mode);
    // return XRSession::NewInstance(env, Napi::External<xr::XRSessionNative>::New(env, session));

  }
}
