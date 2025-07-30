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
     * Enable manual network status control.
     * When enabled, the automatic monitoring is disabled and network status
     * must be set manually using setNetworkStatus().
     * @param enabled true to enable manual control, false to resume automatic monitoring
     */
    void setManualMode(bool enabled);

    /**
     * Check if manual mode is enabled.
     * @return true if manual mode is active
     */
    bool isManualMode() const
    {
      return manualMode_;
    }

    /**
     * Set the network status manually.
     * This only works when manual mode is enabled.
     * @param status The network status to set
     * @return true if status was set successfully, false if manual mode is not enabled
     */
    bool setNetworkStatus(NetworkStatus status);

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
    bool manualMode_ = false;
    NetworkStatus lastKnownStatus_ = NetworkStatus::Online;
    NetworkStatus manualStatus_ = NetworkStatus::Online;
  };

} // namespace runtime