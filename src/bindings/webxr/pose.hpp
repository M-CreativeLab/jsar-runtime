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
    XRPoseBase(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<T>(info),
          transform(mat4(1.0f)),
          emulatedPosition(false)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      if (info.Length() < 3)
      {
        Napi::TypeError::New(env, "XRPose constructor requires 4 arguments: XRPose(device, baseSpace, transform, frameRequest).").ThrowAsJavaScriptException();
        return;
      }
      for (int i = 0; i < 3; i++)
      {
        if (!info[i].IsExternal())
        {
          Napi::TypeError::New(env, "Invalid argument to construct XRPose(), an external object is required.").ThrowAsJavaScriptException();
          return;
        }
      }

      auto deviceNativeExternal = info[0].As<Napi::External<XRDeviceNative>>();
      device = deviceNativeExternal.Data();
      if (device == nullptr)
      {
        Napi::TypeError::New(env, "Invalid argument to construct XRPose(): invalid device pointer.").ThrowAsJavaScriptException();
        return;
      }

      auto transformExternal = info[1].As<Napi::External<mat4>>();
      transform = *transformExternal.Data();

      auto frameExternal = info[2].As<Napi::External<xr::TrXRFrameRequest>>();
      frameRequest = frameExternal.Data();
    }

  protected:
    Napi::Value TransformGetter(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);
      return XRRigidTransform::NewInstance(env, transform);
    }
    Napi::Value EmulatedPositionGetter(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);
      return Napi::Boolean::New(env, emulatedPosition);
    }

  protected:
    mat4 transform;
    bool emulatedPosition;
    XRDeviceNative *device = nullptr;
    xr::TrXRFrameRequest *frameRequest = nullptr;
  };

  class XRPose : public XRPoseBase<XRPose>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, XRDeviceNative *device, mat4 &transform, xr::TrXRFrameRequest *frameRequest);
    XRPose(const Napi::CallbackInfo &info);

  private:
    static Napi::FunctionReference *constructor;
  };

  class XRViewerPose : public XRPoseBase<XRViewerPose>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env,
                                    XRDeviceNative *device,
                                    mat4 &transform,
                                    xr::TrXRFrameRequest *frameRequest,
                                    XRReferenceSpace *baseSpace,
                                    XRSession *session);
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
