#pragma once

#include <napi.h>
#include <chrono>
#include "common.hpp"
#include "space.hpp"
#include "xr/render_state.hpp"

#include "common/scoped_thread.hpp"
#include "common/frame_request/types.hpp"
#include "common/xr/types.hpp"
#include "client/per_process.hpp"
#include "client/xr/webxr_session.hpp"

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
    static thread_local uint32_t NEXT_HANDLE;
  };

  class XRSession : public Napi::ObjectWrap<XRSession>
  {
    friend class XRFrame;

  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRSession> handle);
    static bool IsInstanceOf(Napi::Value value)
    {
      return value.As<Napi::Object>().InstanceOf(constructor->Value());
    }
    static Napi::Value FrameHandler(const Napi::CallbackInfo &info);
    XRSession(const Napi::CallbackInfo &info);
    ~XRSession() = default;

  private:
    Napi::Value RenderStateGetter(const Napi::CallbackInfo &info);
    Napi::Value EnvironmentBlendModeGetter(const Napi::CallbackInfo &info);
    Napi::Value EnabledFeaturesGetter(const Napi::CallbackInfo &info);
    Napi::Value RequestAnimationFrame(const Napi::CallbackInfo &info);
    Napi::Value CancelAnimationFrame(const Napi::CallbackInfo &info);
    Napi::Value RequestReferenceSpace(const Napi::CallbackInfo &info);
    Napi::Value UpdateRenderState(const Napi::CallbackInfo &info);
    Napi::Value UpdateTargetFrameRate(const Napi::CallbackInfo &info);
    Napi::Value UpdateCollisionBox(const Napi::CallbackInfo &info);
    Napi::Value End(const Napi::CallbackInfo &info);

  private:
    Napi::Array createEnabledFeatures(Napi::Env env);
    Napi::Value createInputSourcesChangeEvent(Napi::Env env,
                                              std::vector<XRInputSource *> &added,
                                              std::vector<XRInputSource *> &removed);

  public:
    void onPrimaryActionStart(XRInputSource *inputSource, XRFrame *frame);
    void onPrimaryActionEnd(XRInputSource *inputSource, XRFrame *frame);
    void onSqueezeActionStart(XRInputSource *inputSource, XRFrame *frame);
    void onSqueezeActionEnd(XRInputSource *inputSource, XRFrame *frame);

  public:
    inline int32_t id() const { return handle_->id; }
    inline client_xr::XRSessionMode mode() const { return handle_->mode; }
    inline client_xr::XRSessionRequestInit requestInit() const { return handle_->requestInit; }
    inline bool immersive() const { return handle_->immersive(); }
    inline client_xr::XREnvironmentBlendMode environmentBlendMode() const { return handle_->environmentBlendMode(); }
    inline std::shared_ptr<client_xr::XRSession> handle() const { return handle_; }
    Napi::FunctionReference onEventCallback;

  private:
    std::shared_ptr<client_xr::XRSession> handle_;

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
