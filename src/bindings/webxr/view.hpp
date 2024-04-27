#pragma once

#include <napi.h>
#include "common.hpp"
#include "device_native.hpp"
#include "rigid_transform.hpp"
#include "xr/viewport.hpp"

namespace bindings
{
  class XRView : public Napi::ObjectWrap<XRView>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, XRSession *session, mat4 &transform, uint32_t index, XREye eye);
    XRView(const Napi::CallbackInfo &info);

  private:
    Napi::Value EyeGetter(const Napi::CallbackInfo &info);
    Napi::Value ProjectionMatrixGetter(const Napi::CallbackInfo &info);
    Napi::Value TransformGetter(const Napi::CallbackInfo &info);
    Napi::Value RecommendedViewportScaleGetter(const Napi::CallbackInfo &info);
    Napi::Value RequestViewportScale(const Napi::CallbackInfo &info);

  public:
    xr::Viewport getViewport();

  private:
    XRDeviceNative* device;
    uint32_t index;
    uint32_t eyeId;
    uint32_t sessionId;
    mat4 transformMatrix;
    mat4 projectionMatrix;
    xr::Viewport viewport;

  private:
    static Napi::FunctionReference *constructor;
  };
}
