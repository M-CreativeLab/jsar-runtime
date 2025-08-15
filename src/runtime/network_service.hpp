#pragma once

#include <memory>
#include <functional>

#include "./network_monitor.hpp"
#include "./constellation.hpp"

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
  using NetworkEventCallback = std::function<void(NetworkEventType eventType,
                                                  NetworkStatus status,
                                                  const std::string &timestamp)>;

  /**
   * @class NetworkService
   *
   * Service class that manages network monitoring and dispatches network events.
   * This class acts as a bridge between the platform-specific network monitor
   * and the event system used throughout the runtime.
   */
  class NetworkService
  {
  public:
    NetworkService(TrConstellation *);
    ~NetworkService();

    /**
     * Start the network monitoring service.
     * @return true if started successfully, false otherwise
     */
    bool start();

    /**
     * Shutdown the network monitoring service.
     */
    void shutdown();

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

  private:
    void onNetworkStatusChanged(NetworkStatus status);

  private:
    TrConstellation *constellation_;
    std::shared_ptr<NetworkMonitor> monitor_;

    bool isRunning_ = false;
    bool manualMode_ = false;
    NetworkStatus lastKnownStatus_ = NetworkStatus::Offline;
    NetworkStatus manualStatus_ = NetworkStatus::Online;
  };

} // namespace runtime
