#pragma once

#include <napi.h>
#include "layer.hpp"
#include "xr/render_state.hpp"
#include <client/xr/webxr_renderstate.hpp>

namespace bindings
{
  class XRRenderState : public Napi::ObjectWrap<XRRenderState>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, client_xr::XRRenderState state);
    XRRenderState(const Napi::CallbackInfo &info);

  private:
    Napi::Value BaseLayerGetter(const Napi::CallbackInfo &info);
    Napi::Value DepthFarGetter(const Napi::CallbackInfo &info);
    Napi::Value DepthNearGetter(const Napi::CallbackInfo &info);
    Napi::Value InlineVerticalFieldOfViewGetter(const Napi::CallbackInfo &info);

  public:
    inline client_xr::XRRenderState handle() { return handle_; }

  private:
    client_xr::XRRenderState handle_;

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
