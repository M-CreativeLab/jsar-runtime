#pragma once

#include <memory>
#include "common/events_v2/event_target.hpp"
#include "./dom_event.hpp"

namespace dom
{
  // Forward declaration
  class Event;

  class EventListener : public events_comm::TrEventListener<DOMEventType, Event>
  {
  public:
    using events_comm::TrEventListener<DOMEventType, Event>::TrEventListener;
  };

  class DOMEventTarget : public events_comm::TrEventTarget<DOMEventType, Event>
  {
  public:
    DOMEventTarget() = default;
  };
}
