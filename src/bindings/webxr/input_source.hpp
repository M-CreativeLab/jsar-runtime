#pragma once

#include <set>
#include <vector>
#include <optional>

#include <napi.h>
#include <client/per_process.hpp>
#include <client/xr/webxr_input_sources.hpp>
#include <common/xr/input_sources.hpp>
#include "./common.hpp"

namespace bindings
{
  using InputSourceInternalResetCallback = function<xr::TrXRInputSource *(xr::TrXRFrameRequest *)>;
  using InputSourcesChangedCallback = function<void(vector<XRInputSource *> added, vector<XRInputSource *> removed)>;

  class XRInputSource : public XRHandleWrap<XRInputSource, client_xr::XRInputSource>
  {
    friend class XRHandleWrap<XRInputSource, client_xr::XRInputSource>;

  public:
    /**
     * Add `XRInputSource` class to the provided `env`.
     *
     * @param env The `Napi::Env` to add the class to.
     */
    static void Init(Napi::Env env);
    /**
     * Get the `XRInputSource` instance from the provided `handle`, or create a new one if not exists.
     * 
     * @param env The `Napi::Env` to create the instance.
     * @param handle The `client_xr::XRInputSource` to get or create the instance.
     * @returns The `XRInputSource` instance.
     */
    static Napi::Object GetOrNewInstance(Napi::Env env, shared_ptr<client_xr::XRInputSource> handle);

  public:
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

  private:
    static thread_local Napi::FunctionReference *constructor;
  };

  class XRInputSourceArray : public Napi::Array
  {
  public:
    static XRInputSourceArray New(Napi::Env env, std::optional<client_xr::XRInputSourceArray> handle);

  public:
    XRInputSourceArray(napi_env env, napi_value value);
    ~XRInputSourceArray() = default;

  public:
    void update(std::optional<client_xr::XRInputSourceArray>);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
