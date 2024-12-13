#pragma once

#include <napi.h>
#include <common/viewport.hpp>
#include <common/xr/types.hpp>
#include <xr/viewport.hpp>
#include <client/xr/webxr_view.hpp>

#include "./common.hpp"
#include "./rigid_transform.hpp"

namespace bindings
{
  class XRView : public Napi::ObjectWrap<XRView>
  {
  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRView> handle);

  public:
    XRView(const Napi::CallbackInfo &info);

  private:
    Napi::Value InitEye(Napi::Env env);
    Napi::Value InitProjectionMatrix(Napi::Env env);
    Napi::Value RequestViewportScale(const Napi::CallbackInfo &info);

  public:
    client_xr::XRViewport getViewport();

  private:
    std::shared_ptr<client_xr::XRView> handle_;

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
