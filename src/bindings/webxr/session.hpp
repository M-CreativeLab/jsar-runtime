#pragma once

#include <napi.h>
#include "common.hpp"
#include "device_native.hpp"
#include "space.hpp"
#include "xr/render_state.hpp"

#include "common/frame_request/types.hpp"
#include "client/per_process.hpp"

namespace bindings
{
  class XRFrameCallbackDescriptor
  {
  public:
    XRFrameCallbackDescriptor(Napi::Function cb)
        : handle(++NEXT_HANDLE), cancelled(false)
    {
      callback = new Napi::FunctionReference();
      *callback = Napi::Persistent(cb);
    }
    ~XRFrameCallbackDescriptor()
    {
      if (callback)
      {
        callback->Unref();
        delete callback;
        callback = nullptr;
      }
    }

  public:
    Napi::FunctionReference *callback;
    uint32_t handle;
    bool cancelled;

  private:
    static uint32_t NEXT_HANDLE;
  };

  class XRSession : public Napi::ObjectWrap<XRSession>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    XRSession(const Napi::CallbackInfo &info);
    ~XRSession();

  private:
    Napi::Value RenderStateGetter(const Napi::CallbackInfo &info);
    Napi::Value EnvironmentBlendModeGetter(const Napi::CallbackInfo &info);
    Napi::Value EnabledFeaturesGetter(const Napi::CallbackInfo &info);
    Napi::Value RequestAnimationFrame(const Napi::CallbackInfo &info);
    Napi::Value CancelAnimationFrame(const Napi::CallbackInfo &info);
    Napi::Value RequestReferenceSpace(const Napi::CallbackInfo &info);
    Napi::Value UpdateRenderState(const Napi::CallbackInfo &info);
    Napi::Value UpdateTargetFrameRate(const Napi::CallbackInfo &info);
    Napi::Value End(const Napi::CallbackInfo &info);

  private:
    void start();
    void stop();
    void updateInputSourcesIfChanged(XRFrame *frame);
    void onFrame(Napi::Env env, xr::TrXRFrameRequest *frameRequest);
    bool queueNextFrame();
    void addViewSpace(Napi::Env env, XRViewSpaceType type);
    Napi::Array createEnabledFeatures(Napi::Env env);
    Napi::Value createInputSourcesChangeEvent(Napi::Env env,
                                              std::vector<XRInputSource *> &added,
                                              std::vector<XRInputSource *> &removed);

  public:
    void onPrimaryActionStart(XRInputSource *inputSource, XRFrame *frame);
    void onPrimaryActionEnd(XRInputSource *inputSource, XRFrame *frame);
    void onSqueezeActionStart(XRInputSource *inputSource, XRFrame *frame);
    void onSqueezeActionEnd(XRInputSource *inputSource, XRFrame *frame);
    XRReferenceSpace *getLocalSpace();
    XRReferenceSpace *getViewerSpace();
    void iterateViewSpaces(std::function<void(XRViewSpace *, uint32_t, XRSession *)> callback);

  public:
    XRDeviceNative *device;
    int32_t id;
    XRSessionMode mode;
    bool immersive;
    bool started;
    bool ended;
    bool suspended;
    xr::RenderState *activeRenderState = nullptr;
    xr::RenderState *pendingRenderState = nullptr;
    XREnvironmentBlendMode environmentBlendMode;
    std::vector<XRFrameCallbackDescriptor *> pendingFrameCallbacks;
    std::vector<XRFrameCallbackDescriptor *> currentFrameCallbacks;
    Napi::Reference<Napi::Array> enabledFeatures;
    Napi::ObjectReference localSpace;
    Napi::ObjectReference viewerSpace;
    Napi::ObjectReference unboundedSpace;
    std::vector<Napi::ObjectReference *> viewSpaces;
    Napi::Reference<XRInputSourceArray> inputSources;
    Napi::FunctionReference onEventCallback;

  public:
    static Napi::FunctionReference *constructor;
  };
}
