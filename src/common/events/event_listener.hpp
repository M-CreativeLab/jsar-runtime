#pragma once

#include "idgen.hpp"
#include "./classes.hpp"
#include "./event_type.hpp"

using namespace std;

namespace events
{
  static TrIdGenerator listenerIdGenerator(1);

  using TrEventCallback = function<void(TrEventType type, TrEvent &event)>;
  class TrEventListener
  {
  public:
    TrEventListener(TrEventCallback onEvent) : onEvent(onEvent), id(listenerIdGenerator.get())
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
