#pragma once

#include <napi.h>
#include <client/xr/webxr_session_events.hpp>
#include "../event.hpp"

namespace dombinding::events
{
  class XRInputSourcesChangeEvent : public dombinding::EventWrap<XRInputSourcesChangeEvent,
                                                                 client_xr::XRInputSourcesChangeEvent>
  {
    friend class dombinding::EventWrap<XRInputSourcesChangeEvent, client_xr::XRInputSourcesChangeEvent>;

  public:
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRInputSourcesChangeEvent> nativeEvent);
    static Napi::Object Init(Napi::Env env);

  public:
    XRInputSourcesChangeEvent(const Napi::CallbackInfo &info);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
