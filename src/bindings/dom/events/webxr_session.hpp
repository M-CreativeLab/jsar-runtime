#pragma once

#include <napi.h>
#include <client/xr/webxr_session_events.hpp>
#include "../event.hpp"

namespace dombinding::events
{
  class XRSessionEvent : public dombinding::EventWrap<XRSessionEvent, client_xr::XRSessionEvent>
  {
    friend class dombinding::EventWrap<XRSessionEvent, client_xr::XRSessionEvent>;

  public:
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRSessionEvent> nativeEvent);
    static Napi::Object Init(Napi::Env env);

  public:
    XRSessionEvent(const Napi::CallbackInfo &info);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
