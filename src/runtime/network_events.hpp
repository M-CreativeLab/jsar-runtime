#pragma once

#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <functional>
#include "network_monitor.hpp"

namespace runtime
{
  /**
   * @enum NetworkEventType
   * Types of network-related events that can be dispatched.
   */
  enum class NetworkEventType
  {
    Online, // Network connectivity is available
    Offline // Network connectivity is lost
  };

  /**
   * @class NetworkEventCallback
   * Simple callback interface for network status changes.
   */
  using NetworkEventCallback = std::function<void(NetworkEventType eventType, NetworkStatus status, const std::string& timestamp)>;

  /**
   * @class NetworkEventDispatcher
   * Basic event dispatcher for network status changes in runtime.
   */
  class NetworkEventDispatcher
  {
  public:
    NetworkEventDispatcher() = default;
    ~NetworkEventDispatcher() = default;

    /**
     * Register a callback for network events.
     */
    void registerCallback(NetworkEventCallback callback)
    {
      callback_ = callback;
    }

    /**
     * Unregister the callback.
     */
    void unregisterCallback()
    {
      callback_ = nullptr;
    }

    /**
     * Dispatch a network event.
     */
    void dispatchEvent(NetworkEventType eventType, NetworkStatus status)
    {
      if (callback_)
      {
        std::string timestamp = getCurrentTimestamp();
        callback_(eventType, status, timestamp);
      }
    }

    /**
     * Create and dispatch a network event from status.
     */
    void dispatchFromNetworkStatus(NetworkStatus status)
    {
      NetworkEventType eventType = (status == NetworkStatus::Online)
                                     ? NetworkEventType::Online
                                     : NetworkEventType::Offline;
      dispatchEvent(eventType, status);
    }

  private:
    static std::string getCurrentTimestamp()
    {
      auto now = std::chrono::system_clock::now();
      auto time_t = std::chrono::system_clock::to_time_t(now);
      std::stringstream ss;
      ss << std::put_time(std::localtime(&time_t), "%Y-%m-%dT%H:%M:%S");
      return ss.str();
    }

    NetworkEventCallback callback_ = nullptr;
  };

} // namespace runtime