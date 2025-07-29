#pragma once

#include <memory>
#include <functional>
#include <common/events_v2/event_target.hpp>
#include "network_monitor.hpp"
#include "network_events.hpp"

namespace runtime
{
  /**
   * @class NetworkService
   * Service class that manages network monitoring and dispatches network events.
   * This class acts as a bridge between the platform-specific network monitor
   * and the generic event system used throughout the runtime.
   */
  class NetworkService : public events_comm::TrEventTarget<NetworkEventType, NetworkEvent>
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
     * Get the singleton instance of the network service.
     * @return Reference to the global network service instance
     */
    static NetworkService &getInstance();

  private:
    void onNetworkStatusChanged(NetworkStatus status);

  private:
    std::shared_ptr<NetworkMonitor> monitor_;
    bool isRunning_ = false;
    NetworkStatus lastKnownStatus_ = NetworkStatus::Online;
  };

} // namespace runtime