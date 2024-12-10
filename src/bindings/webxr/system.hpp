#pragma once

#include <memory>
#include <napi.h>
#include <client/xr/webxr_system.hpp>

namespace bindings::webxr
{
  class XRSystem : public Napi::ObjectWrap<XRSystem>
  {
  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env);

  public:
    XRSystem(const Napi::CallbackInfo &info);

  private:
    Napi::Value IsSessionSupported(const Napi::CallbackInfo &info);
    Napi::Value RequestSession(const Napi::CallbackInfo &info);

  private:
    std::shared_ptr<client_xr::XRSystem> handle_;

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
