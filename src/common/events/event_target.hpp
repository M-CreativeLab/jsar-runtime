#pragma once

#include <stdlib.h>
#include <map>

#include "./classes.hpp"
#include "./event_type.hpp"
#include "./event_listener.hpp"
#include "./event.hpp"

using namespace std;

namespace events
{
  class TrEventTarget
  {
  public:
    TrEventTarget()
    {
    }
    ~TrEventTarget()
    {
    }

  public:
    bool dispatchEvent(int id, TrEventType type, const char *data)
    {
      TrEvent event(id, type, data);
      auto it = listeners.find(type);
      if (it != listeners.end())
      {
        auto listener = *it->second;
        listener(type, event);
      }
      return true;
    }
    TrEventListener *addEventListener(TrEventType type, TrEventCallback listenerCallback)
    {
      auto listener = new TrEventListener(listenerCallback);
      listeners.insert({type, listener});
      return listener;
    }
    void removeEventListener(TrEventListener *listener)
    {
      for (auto it = listeners.begin(); it != listeners.end();)
      {
        if (listener == it->second)
        {
          delete listener;
          it = listeners.erase(it);
          break;
        }
        else
        {
          ++it;
        }
      }
    }
    void removeEventListener(TrEventType type)
    {
      for (auto it = listeners.begin(); it != listeners.end();)
      {
        if (it->first == type)
        {
          delete it->second;
          it = listeners.erase(it);
        }
        else
        {
          ++it;
        }
      }
    }

  protected:
    map<TrEventType, TrEventListener *> listeners;
  };
}
