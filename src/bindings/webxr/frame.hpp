#pragma once

#include <chrono>
#include <napi.h>

#include <common/xr/types.hpp>
#include <client/xr/webxr_frame.hpp>
#include "./common.hpp"
#include "./session.hpp"

using namespace std;

namespace bindings
{
  class XRFrame : public Napi::ObjectWrap<XRFrame>
  {
    friend class XRSession;
    friend class XRInputSource;

  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, XRSession *session, std::shared_ptr<client_xr::XRFrame> frame);

  public:
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
    void start();
    void end();

  public:
    inline std::shared_ptr<client_xr::XRFrame> handle() { return handle_; }
    inline uint32_t id() { return handle_->id(); }
    inline XRSession *session() { return session_; }
    inline uint32_t sessionId() { return session_->id(); }
    inline uint32_t stereoId() { return handle_->stereoId(); }

  private:
    std::shared_ptr<client_xr::XRFrame> handle_;
    XRSession *session_;

  private:
    static thread_local Napi::FunctionReference *constructor;
    static thread_local uint32_t NEXT_FRAME_ID;
  };
}
