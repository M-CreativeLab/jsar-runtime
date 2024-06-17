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
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static XRDeviceNative *GetInstance();
    static Napi::Value NativeFrameHandler(const Napi::CallbackInfo &info);
    XRDeviceNative(const Napi::CallbackInfo &info);

  private:
    Napi::Value IsSessionSupported(const Napi::CallbackInfo &info);
    Napi::Value RequestSession(const Napi::CallbackInfo &info);
    Napi::Value RequestFrameOfReferenceTransform(const Napi::CallbackInfo &info);
    Napi::Value GetViewport(const Napi::CallbackInfo &info);
    Napi::Value GetViewerTransform(const Napi::CallbackInfo &info);
    Napi::Value GetViewerStereoViewMatrix(const Napi::CallbackInfo &info);
    Napi::Value GetViewerStereoProjectionMatrix(const Napi::CallbackInfo &info);
    Napi::Value GetActiveEyeId(const Napi::CallbackInfo &info);
    Napi::Value GetGazeInputSource(const Napi::CallbackInfo &info);
    Napi::Value GetHandInputSource(const Napi::CallbackInfo &info);
    Napi::Value GetGamepadInputSources(const Napi::CallbackInfo &info);
    Napi::Value GetScreenInputSources(const Napi::CallbackInfo &info);
    Napi::Value StartFrame(const Napi::CallbackInfo &info);
    Napi::Value EndFrame(const Napi::CallbackInfo &info);

  public:
    bool supportsSessionMode(XRSessionMode sessionMode);
    bool supportsReferenceSpaceType(XRReferenceSpaceType referenceSpaceType);
    void requestFrame(XRFrameCallback callback, void* context);
    bool startFrame(uint32_t sessionId, uint32_t stereoRenderingId, uint32_t passIndex);
    bool endFrame(uint32_t sessionId, uint32_t stereoRenderingId, uint32_t passIndex);
    TrViewport getViewport(uint32_t viewIndex);

  private:
    void handleFrameRequest(xr::TrXRFrameRequest *frameRequest);

  private:
    TrClientContextPerProcess* clientContext = nullptr;
    Napi::FunctionReference *frameHandler = nullptr;
    Napi::ThreadSafeFunction tsfnWithFrameHandler;
    std::vector<ContextifiedXRFrameCallback> contextifiedFrameCallbacks;

  private:
    static Napi::FunctionReference *constructor;
  };
}
