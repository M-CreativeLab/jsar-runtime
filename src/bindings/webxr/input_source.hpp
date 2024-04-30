#pragma once

#include <napi.h>
#include "common.hpp"
#include "xr/device.hpp"

namespace bindings
{
  class XRInputSource : public Napi::ObjectWrap<XRInputSource>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, xr::InputSource &inputSource);
    XRInputSource(const Napi::CallbackInfo &info);
    ~XRInputSource();

  private:
    Napi::Value GamepadGetter(const Napi::CallbackInfo &info);
    Napi::Value GripSpaceGetter(const Napi::CallbackInfo &info);
    Napi::Value HandGetter(const Napi::CallbackInfo &info);
    Napi::Value HandnessGetter(const Napi::CallbackInfo &info);
    Napi::Value TargetRayModeGetter(const Napi::CallbackInfo &info);
    Napi::Value TargetRaySpaceGetter(const Napi::CallbackInfo &info);

  public:
    xr::InputSource *internal = nullptr;

  private:
    static Napi::FunctionReference *constructor;
  };

  class XRInputSourceArray : public Napi::Array
  {
  public:
    static XRInputSourceArray New(Napi::Env env);
    XRInputSourceArray(napi_env env, napi_value value);
    ~XRInputSourceArray();

  public:
    void updateInputSourcesIfChanged();

  private:
    xr::Device *device = nullptr;

  private:
    static Napi::FunctionReference *constructor;
  };
}
