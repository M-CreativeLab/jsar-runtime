#pragma once

#include <string>
#include <functional>
#include <unordered_map>

namespace jsar::example
{
  class Content;

  /**
   * EventProxy provides a simple mechanism for forwarding events between components.
   * Allows decoupling of 3D scene logic from UI event handling by providing
   * a clean interface for event communication between ContentBar3d and ContentBarCanvas.
   */
  class EventProxy
  {
  public:
    // Event handler function type
    using EventHandler = std::function<bool(Content *, const std::string &, void *)>;

    EventProxy();
    ~EventProxy();

    /**
     * Register an event handler for a specific event type.
     * Multiple handlers can be registered for the same event type.
     */
    void registerHandler(const std::string &eventType, EventHandler handler);

    /**
     * Unregister all handlers for a specific event type.
     */
    void unregisterHandlers(const std::string &eventType);

    /**
     * Forward an event to all registered handlers for the event type.
     * Returns true if any handler processed the event.
     */
    bool forwardEvent(Content *content, const std::string &eventType, void *eventData);

    /**
     * Check if there are any handlers registered for an event type.
     */
    bool hasHandlers(const std::string &eventType) const;

  private:
    std::unordered_map<std::string, std::vector<EventHandler>> handlers_;
  };
}