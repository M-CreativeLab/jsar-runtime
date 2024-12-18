#pragma once

#include <napi.h>
#include <client/xr/webxr_session_events.hpp>
#include "../event.hpp"

namespace dombinding::events
{
  class XRInputSourceEvent : public dombinding::EventWrap<XRInputSourceEvent, client_xr::XRInputSourceEvent>
  {
    friend class dombinding::EventWrap<XRInputSourceEvent, client_xr::XRInputSourceEvent>;

  public:
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRInputSourceEvent> nativeEvent);
    static Napi::Object Init(Napi::Env env);

  public:
    XRInputSourceEvent(const Napi::CallbackInfo &info);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
