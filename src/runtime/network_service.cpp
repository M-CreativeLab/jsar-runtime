#include "network_service.hpp"
#include <iostream>

namespace runtime
{
  NetworkService::NetworkService()
  {
    monitor_ = NetworkMonitor::create();
  }

  NetworkService::~NetworkService()
  {
    stop();
  }

  bool NetworkService::start()
  {
    if (isRunning_ || !monitor_)
    {
      return false;
    }

    // Set up the callback to handle network status changes
    auto statusCallback = [this](NetworkStatus status)
    {
      onNetworkStatusChanged(status);
    };

    if (monitor_->startMonitoring(statusCallback))
    {
      isRunning_ = true;
      lastKnownStatus_ = monitor_->getCurrentStatus();
      return true;
    }

    return false;
  }

  void NetworkService::stop()
  {
    if (!isRunning_ || !monitor_)
    {
      return;
    }

    monitor_->stopMonitoring();
    isRunning_ = false;
  }

  NetworkStatus NetworkService::getCurrentStatus() const
  {
    if (monitor_)
    {
      return monitor_->getCurrentStatus();
    }
    return NetworkStatus::Offline;
  }

  NetworkService &NetworkService::getInstance()
  {
    static NetworkService instance;
    return instance;
  }

  void NetworkService::registerEventCallback(NetworkEventCallback callback)
  {
    eventDispatcher_.registerCallback(callback);
  }

  void NetworkService::unregisterEventCallback()
  {
    eventDispatcher_.unregisterCallback();
  }

  void NetworkService::onNetworkStatusChanged(NetworkStatus status)
  {
    // Only dispatch events if the status actually changed
    if (status != lastKnownStatus_)
    {
      lastKnownStatus_ = status;

      // Dispatch the network event
      eventDispatcher_.dispatchFromNetworkStatus(status);

      // Log the network status change for debugging
      const char *statusStr = (status == NetworkStatus::Online) ? "online" : "offline";
      std::cout << "[NetworkService] Network status changed to: " << statusStr << std::endl;
    }
  }

} // namespace runtime