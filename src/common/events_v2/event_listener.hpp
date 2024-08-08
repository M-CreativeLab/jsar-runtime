#pragma once

#include "idgen.hpp"

using namespace std;

namespace events_comm
{
  static TrIdGenerator listenerIdGenerator(1);
  template <typename EventType, typename EventInstance, typename Callback = function<void(EventType, EventInstance &)>>
  class TrEventListener
  {
  public:
    TrEventListener(Callback eventCallback)
        : eventCallback(eventCallback),
          id(listenerIdGenerator.get())
    {
    }
    void operator()(EventType type, EventInstance &event)
    {
      eventCallback(type, event);
    }

  public:
    bool equals(TrEventListener &listener)
    {
      return id == listener.id;
    }

  public:
    uint32_t id;

  private:
    Callback eventCallback;
  };
}
