#pragma once

#include <string>
#include <memory>
#include "client/dom/dom_event_target.hpp"

namespace dom
{
  /**
   * @class NetworkEvent
   * Represents a network connectivity event (online/offline).
   * Follows the web standard for network events.
   */
  class NetworkEvent : public Event
  {
  public:
    NetworkEvent(const std::string &type, bool bubbles = false, bool cancelable = false)
        : Event(type, bubbles, cancelable)
    {
    }

    DOMEventConstructorType constructor() const override
    {
      return DOMEventConstructorType::kNetworkEvent;
    }

    /**
     * Create an "online" network event.
     */
    static std::shared_ptr<NetworkEvent> createOnlineEvent()
    {
      return std::make_shared<NetworkEvent>("online", false, false);
    }

    /**
     * Create an "offline" network event.
     */
    static std::shared_ptr<NetworkEvent> createOfflineEvent()
    {
      return std::make_shared<NetworkEvent>("offline", false, false);
    }
  };

} // namespace dom