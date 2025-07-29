#pragma once

#include <memory>
#include <functional>
#include "network_monitor.hpp"
#include "network_events.hpp"

namespace runtime
{
  /**
   * @class NetworkService
   * Service class that manages network monitoring and dispatches network events.
   * This class acts as a bridge between the platform-specific network monitor
   * and the event system used throughout the runtime.
   */
  class NetworkService
  {
  public:
    NetworkService();
    ~NetworkService();

    /**
     * Start the network monitoring service.
     * @return true if started successfully, false otherwise
     */
    bool start();

    /**
     * Stop the network monitoring service.
     */
    void stop();

    /**
     * Check if the service is currently running.
     * @return true if monitoring is active
     */
    bool isRunning() const
    {
      return isRunning_;
    }

    /**
     * Get the current network status.
     * @return Current network connectivity status
     */
    NetworkStatus getCurrentStatus() const;

    /**
     * Register a callback for network events.
     * @param callback Function to call when network status changes
     */
    void registerEventCallback(NetworkEventCallback callback);

    /**
     * Unregister the network event callback.
     */
    void unregisterEventCallback();

    /**
     * Get the singleton instance of the network service.
     * @return Reference to the global network service instance
     */
    static NetworkService &getInstance();

  private:
    void onNetworkStatusChanged(NetworkStatus status);

  private:
    std::shared_ptr<NetworkMonitor> monitor_;
    NetworkEventDispatcher eventDispatcher_;
    bool isRunning_ = false;
    NetworkStatus lastKnownStatus_ = NetworkStatus::Online;
  };

} // namespace runtime