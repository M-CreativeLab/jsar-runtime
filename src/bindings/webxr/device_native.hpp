#pragma once

#include <mutex>
#include <napi.h>
#include <node/uv.h>

#include "common/viewport.hpp"
#include "common/xr/types.hpp"
#include "client/per_process.hpp"
#include "./common.hpp"

namespace bindings
{
  typedef std::function<void(Napi::Env env, xr::TrXRFrameRequest *, void *context)> XRFrameCallback;
  class ContextifiedXRFrameCallback
  {
  public:
    ContextifiedXRFrameCallback(XRFrameCallback callback, void *context) : callback(callback), context(context) {}

  public:
    XRFrameCallback callback;
    void *context;
  };

  class XRDeviceNative : public Napi::ObjectWrap<XRDeviceNative>
  {
    friend class XRSession;
    friend class XRFrame;

  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static XRDeviceNative *GetInstance();
    XRDeviceNative(const Napi::CallbackInfo &info);

  private:
    Napi::Value IsSessionSupported(const Napi::CallbackInfo &info);
    Napi::Value RequestSession(const Napi::CallbackInfo &info);

  public:
    bool supportsSessionMode(XRSessionMode sessionMode);
    bool supportsReferenceSpaceType(XRReferenceSpaceType referenceSpaceType);
    void requestFrame(XRFrameCallback callback, void *context);
    bool startFrame(xr::TrXRFrameRequest *frameRequest);
    bool endFrame(xr::TrXRFrameRequest *frameRequest);
    TrViewport getViewport(uint32_t viewIndex);
    xr::TrDeviceInit &getDeviceInit();

  private:
    TrClientContextPerProcess *clientContext = nullptr;
    std::vector<ContextifiedXRFrameCallback> contextifiedFrameCallbacks;

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
