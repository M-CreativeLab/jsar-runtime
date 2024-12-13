#pragma once

#include <napi.h>
#include <common/viewport.hpp>
#include <client/xr/webxr_viewport.hpp>
#include "./common.hpp"

namespace bindings
{
  class XRViewport : public Napi::ObjectWrap<XRViewport>
  {
  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, TrViewport viewport);
    static Napi::Object NewInstance(Napi::Env env, client_xr::XRViewport viewport);
    XRViewport(const Napi::CallbackInfo &info);

  public:
    client_xr::XRViewport config;

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
