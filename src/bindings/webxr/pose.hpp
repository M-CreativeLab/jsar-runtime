#pragma once

#include <napi.h>
#include "common/xr/types.hpp"
#include "./common.hpp"
#include "./rigid_transform.hpp"
#include "./view.hpp"

using namespace glm;

namespace bindings
{
  template <typename T>
  class XRPoseBase : public Napi::ObjectWrap<T>
  {
  public:
    XRPoseBase(const Napi::CallbackInfo &info);

  protected:
    Napi::Value TransformGetter(const Napi::CallbackInfo &info);
    Napi::Value EmulatedPositionGetter(const Napi::CallbackInfo &info);

  protected:
    mat4 transform;
    bool emulatedPosition;
    XRDeviceNative* device = nullptr;
    xr::TrXRFrameRequest* frameRequest = nullptr;
  };

  class XRPose : public XRPoseBase<XRPose>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, XRDeviceNative* device, mat4& transform, xr::TrXRFrameRequest *frameRequest);
    XRPose(const Napi::CallbackInfo &info);

  private:
    static Napi::FunctionReference *constructor;
  };

  class XRViewerPose : public XRPoseBase<XRViewerPose>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, XRDeviceNative* device, mat4& transform, xr::TrXRFrameRequest* frameRequest, XRSession* session);
    XRViewerPose(const Napi::CallbackInfo &info);
    ~XRViewerPose();

  private:
    Napi::Value ViewsGetter(const Napi::CallbackInfo &info);

  private:
    std::vector<Napi::ObjectReference> views;

  private:
    static Napi::FunctionReference *constructor;
  };
}
