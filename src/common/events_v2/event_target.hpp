#pragma once

#include <stdlib.h>
#include <set>
#include <map>
#include <memory>

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
      typename EventInstance = TrEvent<EventType>,
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
     * @param event The event instance reference to dispatch.
     */
    bool dispatchEvent(EventInstance &event)
    {
      auto it = eventToListenersMap.find(event.type);
      if (it != eventToListenersMap.end())
      {
        auto listeners = it->second;
        for (auto listenerPtr : listeners)
        {
          auto listener = *listenerPtr;
          listener(event.type, event);
        }
      }
      if (globalListener != nullptr)
      {
        auto listener = *globalListener;
        listener(event.type, event);
      }
      return true;
    }
    /**
     * Dispatch an event to the event target.
     *
     * @param type The type of the event.
     * @param detail The detail of the event, it is optional.
     */
    template <typename DetailObjectType>
    bool dispatchEvent(EventType type, optional<DetailObjectType> detail = nullopt)
    {
      EventInstance event = EventInstance::MakeEvent(type, detail);
      return dispatchEvent(event);
    }
    /**
     * Dispatch an event to the event target, it creates the event instance from a detail string(JSON).
     *
     * @param type The type of the event.
     * @param detailJsonPtr The detail string(JSON) pointer of the event, it is optional.
     */
    bool dispatchEvent(EventType type, const char *detailJsonPtr = nullptr)
    {
      EventInstance event = EventInstance::MakeEventWithString(type, detailJsonPtr);
      return dispatchEvent(event);
    }
    /**
     * Add an event listener to the event target.
     * 
     * @param type The event type
     * @param listenerCallback The event listener callback
     */
    shared_ptr<EventListener> addEventListener(EventType type, EventCallback listenerCallback)
    {
      auto newListener = make_shared<EventListener>(listenerCallback);
      auto it = eventToListenersMap.find(type);
      if (it == eventToListenersMap.end())
      {
        eventToListenersMap.insert({type, {newListener}});
      }
      else
      {
        auto listeners = it->second;
        listeners.push_back(newListener);
      }
      return newListener;
    }
    /**
     * Remove an event listener from the event target.
     * 
     * @param type The event type
     * @param listener The event listener to remove
     */
    void removeEventListener(EventType type, shared_ptr<EventListener> listener)
    {
      if (listener == nullptr)
        return; // Just return if the listener is nullptr

      auto it = eventToListenersMap.find(type);
      if (it != eventToListenersMap.end())
      {
        auto listeners = it->second;
        for (auto it = listeners.begin(); it != listeners.end();)
        {
          if (listener == *it)
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
    }
    /**
     * Remove all event listeners of the specified type from the event target.
     * 
     * @param type The event type
     */
    void removeEventListener(EventType type)
    {
      auto it = eventToListenersMap.find(type);
      if (it != eventToListenersMap.end())
        eventToListenersMap.erase(it);
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
    map<EventType, vector<shared_ptr<EventListener>>> eventToListenersMap;
    /**
     * The global event listener.
     */
    unique_ptr<EventListener> globalListener;
  };
}
