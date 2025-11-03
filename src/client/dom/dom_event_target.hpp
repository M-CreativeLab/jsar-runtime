#pragma once

#include <memory>
#include <common/events_v2/event_target.hpp>
#include <client/scripting_base/v8_object_holder.hpp>
#include "./dom_event.hpp"

namespace endor
{
  namespace dom
  {
    // Forward declaration
    class Event;

    class EventListener : public events_comm::TrEventListener<DOMEventType, Event>
    {
      using events_comm::TrEventListener<DOMEventType, Event>::TrEventListener;
    };

    class DOMEventTarget : public events_comm::TrEventTarget<DOMEventType, Event>,
                           public scripting_base::JSObjectHolder
    {
    public:
      DOMEventTarget() = default;
      virtual ~DOMEventTarget() = default;
    };
  }
} // namespace endor
