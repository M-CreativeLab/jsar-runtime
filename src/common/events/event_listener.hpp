#pragma once

#include "idgen.hpp"
#include "./classes.hpp"
#include "./event_type.hpp"
#include "./event.hpp"

using namespace std;

namespace events
{
  static TrIdGenerator eventIdGenerator(1);

  using TrEventCallback = function<void(TrEventType type, TrEvent &event)>;
  class TrEventListener
  {
  public:
    TrEventListener(TrEventCallback onEvent) : onEvent(onEvent), id(eventIdGenerator.get())
    {
    }
    void operator()(TrEventType type, TrEvent &event)
    {
      onEvent(type, event);
    }

  private:
    uint32_t id;
    TrEventCallback onEvent;
  };
}
