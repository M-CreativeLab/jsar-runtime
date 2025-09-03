#include <algorithm>

#include "./event_proxy.hpp"
#include "./content.hpp"

namespace jsar::example
{
  using namespace std;

  EventProxy::EventProxy()
  {
  }

  EventProxy::~EventProxy()
  {
  }

  void EventProxy::registerHandler(const std::string &eventType, EventHandler handler)
  {
    handlers_[eventType].push_back(handler);
  }

  void EventProxy::unregisterHandlers(const std::string &eventType)
  {
    auto it = handlers_.find(eventType);
    if (it != handlers_.end())
    {
      handlers_.erase(it);
    }
  }

  bool EventProxy::forwardEvent(Content *content, const std::string &eventType, void *eventData)
  {
    auto it = handlers_.find(eventType);
    if (it == handlers_.end())
    {
      return false; // No handlers for this event type
    }

    bool handled = false;
    for (const auto &handler : it->second)
    {
      if (handler(content, eventType, eventData))
      {
        handled = true;
      }
    }

    return handled;
  }

  bool EventProxy::hasHandlers(const std::string &eventType) const
  {
    auto it = handlers_.find(eventType);
    return it != handlers_.end() && !it->second.empty();
  }
}