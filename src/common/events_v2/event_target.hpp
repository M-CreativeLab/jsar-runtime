#pragma once

#include <stdlib.h>
#include <map>
#include <memory>

#include "./event_types.hpp"
#include "./event_listener.hpp"
#include "./event.hpp"

using namespace std;

namespace events_comm
{
  /**
   * The Event Target template class.
   */
  template <
      typename EventType,
      typename DetailType = TrEventDetailBase,
      typename EventInstance = TrEvent<EventType, DetailType>,
      typename EventCallback = function<void(EventType, EventInstance &)>,
      typename EventListener = TrEventListener<EventType, EventInstance>>
  class TrEventTarget
  {
  public:
    TrEventTarget() = default;
    virtual ~TrEventTarget() = default;

  public:
    /**
     * Dispatch an event to the event target.
     * 
     * @param type The type of the event.
     * @param detail The detail of the event, it is optional.
     */
    bool dispatchEvent(EventType type, std::optional<DetailType> detail = std::nullopt)
    {
      EventInstance event(type, detail);
      auto it = listeners.find(type);
      if (it != listeners.end())
      {
        auto listener = *it->second;
        listener(type, event);
      }
      if (globalListener != nullptr)
      {
        auto listener = *globalListener;
        listener(type, event);
      }
      return true;
    }
    /**
     * Add an event listener to the event target.
     */
    shared_ptr<EventListener> addEventListener(EventType type, EventCallback listenerCallback)
    {
      auto newListener = make_shared<EventListener>(listenerCallback);
      listeners.insert({type, newListener});
      return newListener;
    }
    /**
     * Remove an event listener from the event target.
     */
    void removeEventListener(shared_ptr<EventListener> listener)
    {
      for (auto it = listeners.begin(); it != listeners.end();)
      {
        if (listener == it->second)
        {
          it = listeners.erase(it);
          break;
        }
        else
        {
          ++it;
        }
      }
    }
    /**
     * Remove all event listeners of the specified type from the event target.
     */
    void removeEventListener(EventType type)
    {
      for (auto it = listeners.begin(); it != listeners.end();)
      {
        if (it->first == type)
          it = listeners.erase(it);
        else
          ++it;
      }
    }
    /**
     * Set the global event listener of the event target.
     * 
     * Global event listener is triggered when an event is dispatched to the event target no matter what the event type is.
     */
    void resetGlobalEventListener(EventCallback listenerCallback)
    {
      if (globalListener != nullptr)
        resetGlobalEventListener();
      globalListener = make_unique<EventListener>(listenerCallback);
    }
    /**
     * Reset the global event listener of the event target.
     */
    void resetGlobalEventListener()
    {
      globalListener.reset();
    }

  protected:
    /**
     * The map of event listeners.
     */
    map<EventType, shared_ptr<EventListener>> listeners;
    /**
     * The global event listener.
     */
    unique_ptr<EventListener> globalListener;
  };
}
