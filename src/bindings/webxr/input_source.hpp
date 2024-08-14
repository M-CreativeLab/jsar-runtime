#pragma once

#include <set>
#include <vector>
#include <napi.h>
#include "client/per_process.hpp"
#include "common/xr/input_sources.hpp"
#include "common.hpp"

namespace bindings
{
  using InputSourceInternalResetCallback = function<xr::TrXRInputSource *(xr::TrXRFrameRequest *)>;
  using InputSourcesChangedCallback = function<void(vector<XRInputSource *> added, vector<XRInputSource *> removed)>;

  class XRInputSource : public Napi::ObjectWrap<XRInputSource>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, XRSession *session, xr::TrXRInputSource* internal);
    XRInputSource(const Napi::CallbackInfo &info);
    ~XRInputSource();

  private:
    Napi::Value GamepadGetter(const Napi::CallbackInfo &info);
    Napi::Value GripSpaceGetter(const Napi::CallbackInfo &info);
    Napi::Value HandGetter(const Napi::CallbackInfo &info);
    Napi::Value HandednessGetter(const Napi::CallbackInfo &info);
    Napi::Value TargetRayModeGetter(const Napi::CallbackInfo &info);
    Napi::Value TargetRaySpaceGetter(const Napi::CallbackInfo &info);

  public:
    bool dispatchSelectOrSqueezeEvents(XRFrame *frame);

  public:
    xr::TrXRInputSource *internal = nullptr;
    Napi::ObjectReference xrSessionRef;

  private:
    bool primaryActionPressed = false;
    bool squeezeActionPressed = false;

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
    void updateInputSources(XRFrame *frame, XRSession *session, InputSourcesChangedCallback onChangedCallback);

  private:
    XRInputSource* getInputSourceById(int id);

  private:
    TrClientContextPerProcess* clientContext = nullptr;

  private:
    static Napi::FunctionReference *constructor;
  };
}
