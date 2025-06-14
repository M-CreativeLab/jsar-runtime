#pragma once

#include <chrono>
#include <napi.h>

#include <common/scoped_thread.hpp>
#include <common/frame_request/types.hpp>
#include <common/xr/types.hpp>
#include <xr/render_state.hpp>
#include <client/per_process.hpp>
#include <client/xr/webxr_session.hpp>

#include "../dom/event_target-inl.hpp"
#include "./common.hpp"
#include "./space.hpp"

namespace bindings
{
  class XRFrameCallbackDescriptor
  {
  public:
    XRFrameCallbackDescriptor(Napi::Function cb)
        : handle(++NEXT_HANDLE)
        , cancelled(false)
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

  class XRSession : public XRHandleWrap<XRSession, client_xr::XRSession, dombinding::EventTargetWrap>
  {
    friend class XRHandleWrap<XRSession, client_xr::XRSession, dombinding::EventTargetWrap>;
    friend class XRFrame;

  public:
    static void Init(Napi::Env env);
    static inline Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRSession> handle)
    {
      return XRHandleWrap<XRSession, client_xr::XRSession>::NewInstance(env, handle);
    }
    static inline bool IsInstanceOf(Napi::Value value)
    {
      return value.As<Napi::Object>().InstanceOf(constructor->Value());
    }

  public:
    XRSession(const Napi::CallbackInfo &info);
    ~XRSession();

  public:
    dom::DOMEventTargetType eventTargetType() const override final
    {
      return dom::DOMEventTargetType::kXRSession;
    }

  private:
    Napi::Value InputSourcesGetter(const Napi::CallbackInfo &info);
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

  public:
    inline int32_t id() const
    {
      return handle_->id;
    }
    inline client_xr::XRSessionMode mode() const
    {
      return handle_->mode;
    }
    inline client_xr::XRSessionRequestInit requestInit() const
    {
      return handle_->requestInit;
    }
    inline bool immersive() const
    {
      return handle_->immersive();
    }
    inline client_xr::XREnvironmentBlendMode environmentBlendMode() const
    {
      return handle_->environmentBlendMode();
    }

  private:
    Napi::ThreadSafeFunction frameDispatcherTsfn_;
    Napi::Reference<XRInputSourceArray> inputSourcesRef_;
    Napi::Reference<Napi::Array> enabledFeaturesRef_;

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
