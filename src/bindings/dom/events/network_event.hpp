#pragma once

#include <memory>
#include <napi.h>
#include "client/dom/events/network_event.hpp"
#include "../event.hpp"

namespace dombinding
{
  namespace events
  {
    /**
     * JavaScript binding for NetworkEvent.
     */
    class NetworkEvent : public EventWrap<NetworkEvent, dom::NetworkEvent>
    {
      friend class EventWrap<NetworkEvent, dom::NetworkEvent>;

    public:
      static void Init(Napi::Env env);
      static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<dom::NetworkEvent> nativeEvent);

    public:
      NetworkEvent(const Napi::CallbackInfo &info);

    private:
      static thread_local Napi::FunctionReference *constructor;
    };
  } // namespace events
} // namespace dombinding