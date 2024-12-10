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
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, XRSession *session, xr::TrXRView &view, XRReferenceSpace* baseReferenceSpace);
    XRView(const Napi::CallbackInfo &info);

  private:
    Napi::Value InitEye(Napi::Env env);
    Napi::Value InitProjectionMatrix(Napi::Env env);
    Napi::Value RequestViewportScale(const Napi::CallbackInfo &info);

  public:
    client_xr::XRViewport getViewport();

  private:
    uint32_t index;
    uint32_t sessionId;
    mat4 transformMatrix; // TODO: change to `viewMatrix`
    mat4 projectionMatrix;
    TrViewport viewport;
    float viewportScale = 1.0f;

  private:
    std::shared_ptr<client_xr::XRView> handle_;

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
