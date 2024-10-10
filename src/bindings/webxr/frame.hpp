#pragma once

#include <chrono>
#include <napi.h>

#include "common/xr/types.hpp"
#include "./common.hpp"
#include "./device_native.hpp"
#include "./session.hpp"

using namespace std;

namespace bindings
{
  class XRFrame : public Napi::ObjectWrap<XRFrame>
  {
    friend class XRSession;
    friend class XRInputSource;

  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, xr::TrXRFrameRequest *frameRequest, XRSession *session);
    XRFrame(const Napi::CallbackInfo &info);

  private:
    Napi::Value CreateAnchor(const Napi::CallbackInfo &info);
    Napi::Value GetHitTestResults(const Napi::CallbackInfo &info);
    Napi::Value GetHitTestResultsForTransientInput(const Napi::CallbackInfo &info);
    Napi::Value FillPoses(const Napi::CallbackInfo &info);
    Napi::Value FillJointRadii(const Napi::CallbackInfo &info);
    Napi::Value GetImageTrackingResults(const Napi::CallbackInfo &info);
    Napi::Value GetLightEstimate(const Napi::CallbackInfo &info);
    Napi::Value GetDepthInformation(const Napi::CallbackInfo &info);
    Napi::Value GetPose(const Napi::CallbackInfo &info);
    Napi::Value GetViewerPose(const Napi::CallbackInfo &info);
    Napi::Value GetJointPose(const Napi::CallbackInfo &info);

  public:
    uint32_t getStereoRenderingId();
    void start();
    void end();

  private:
    uint32_t id;
    uint32_t stereoId;
    bool active;
    bool animationFrame;
    uint32_t sessionId;
    uint32_t timestamp;
    XRSession *session;
    xr::TrXRFrameRequest *internal;
    XRDeviceNative *device;
    chrono::time_point<chrono::high_resolution_clock> startTime;
    chrono::time_point<chrono::high_resolution_clock> endTime;

  private:
    static thread_local Napi::FunctionReference *constructor;
    static thread_local uint32_t NEXT_FRAME_ID;
  };
}
