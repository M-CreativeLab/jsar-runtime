#pragma once

#include <napi.h>
#include <chrono>
#include "common.hpp"
#include "device_native.hpp"
#include "space.hpp"
#include "xr/render_state.hpp"

#include "common/scoped_thread.hpp"
#include "common/frame_request/types.hpp"
#include "common/xr/types.hpp"
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
    static Napi::Value FrameHandler(const Napi::CallbackInfo &info);
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
    void tick();
    /**
     * @brief Calculate the frames per second.
     * @returns true if the FPS was updated.
     */
    bool calcFps();
    void updateFrameTime(bool updateStereoFrame = false);
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
    xr::TrXRSessionMode mode = xr::TrXRSessionMode::ImmersiveAR;
    xr::TrXRSessionInit config;
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

  private:
    uint32_t fps = 0;
    int frameCount = 0;
    atomic<bool> inXRFrame = false;
    /**
     * Every frame timepoint, updated at the start of each frame.
     */
    std::chrono::steady_clock::time_point frameTimepoint;
    /**
     * The last stereo frame timepoint, updated at the start of each frame from the `frameTimepoint`.
     */
    std::chrono::steady_clock::time_point lastStereoFrameTimepoint;
    /**
     * The last recorded frame timepoint, updated by manual at calculating FPS.
     */
    std::chrono::steady_clock::time_point lastRecordedFrameTimepoint = chrono::steady_clock::now();
    /**
     * The session context zone client.
     */
    unique_ptr<xr::TrXRSessionContextZone> sessionContextZoneClient;
    /**
     * The frames worker thread.
     */
    unique_ptr<WorkerThread> framesWorker;
    Napi::FunctionReference *frameHandlerRef = nullptr;
    Napi::ThreadSafeFunction frameHandlerTSFN;

  public:
    static Napi::FunctionReference *constructor;

    friend class XRFrame;
  };
}
