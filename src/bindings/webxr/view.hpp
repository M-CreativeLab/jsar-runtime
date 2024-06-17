#pragma once

#include <napi.h>
#include "common.hpp"
#include "common/viewport.hpp"
#include "common/xr/types.hpp"
#include "device_native.hpp"
#include "rigid_transform.hpp"
#include "xr/viewport.hpp"

namespace bindings
{
  class XRView : public Napi::ObjectWrap<XRView>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, XRSession *session, xr::TrXRView &view);
    XRView(const Napi::CallbackInfo &info);

  private:
    Napi::Value InitEye(Napi::Env env);
    Napi::Value InitProjectionMatrix(Napi::Env env);
    Napi::Value RequestViewportScale(const Napi::CallbackInfo &info);

  public:
    TrViewport getViewport();

  private:
    XRDeviceNative *device;
    uint32_t index;
    uint32_t sessionId;
    mat4 transformMatrix; // TODO: change to `viewMatrix`
    mat4 projectionMatrix;
    TrViewport viewport;
    float viewportScale = 1.0f;

  private:
    static Napi::FunctionReference *constructor;
  };
}
