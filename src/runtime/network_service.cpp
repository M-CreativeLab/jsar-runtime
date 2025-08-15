#include "network_service.hpp"
#include <iostream>

namespace runtime
{
  using namespace std;

  NetworkService::NetworkService(TrConstellation *constellation)
      : constellation_(constellation)
  {
  }

  NetworkService::~NetworkService()
  {
    stop();
  }

  bool NetworkService::start()
  {
    if (!monitor_)
      monitor_ = NetworkMonitor::Create(constellation_);
    if (isRunning_ || !monitor_)
      return false;

    // Only start automatic monitoring if not in manual mode
    if (!manualMode_)
    {
      // Set up the callback to handle network status changes
      auto statusCallback = [this](NetworkStatus status)
      {
        onNetworkStatusChanged(status);
      };

      if (!monitor_->startMonitoring(statusCallback))
      {
        return false;
      }
    }

    isRunning_ = true;
    lastKnownStatus_ = getCurrentStatus();
    return true;
  }

  void NetworkService::stop()
  {
    if (!isRunning_)
    {
      return;
    }

    // Stop monitoring if not in manual mode
    if (!manualMode_ && monitor_)
    {
      monitor_->stopMonitoring();
    }

    isRunning_ = false;
  }

  NetworkStatus NetworkService::getCurrentStatus() const
  {
    if (manualMode_)
    {
      return manualStatus_;
    }

    if (monitor_)
    {
      return monitor_->getCurrentStatus();
    }
    return NetworkStatus::Offline;
  }

  void NetworkService::onNetworkStatusChanged(NetworkStatus status)
  {
    // Only dispatch events if the status actually changed
    if (status != lastKnownStatus_)
    {
      lastKnownStatus_ = status;

      // Dispatch the network event
      // TODO(yorkie): dispatch to contents

      // Log the network status change for debugging
      const char *statusStr = (status == NetworkStatus::Online) ? "online" : "offline";
      DEBUG(LOG_TAG_JSAR, "[NetworkService] Network status changed to: %s", statusStr);
    }
  }

  void NetworkService::setManualMode(bool enabled)
  {
    if (manualMode_ == enabled)
    {
      return; // No change needed
    }

    manualMode_ = enabled;

    if (isRunning_)
    {
      if (enabled)
      {
        // Switch to manual mode: stop automatic monitoring
        if (monitor_)
        {
          monitor_->stopMonitoring();
        }
        cout << "[NetworkService] Switched to manual network monitoring mode" << endl;
      }
      else
      {
        // Switch to automatic mode: start monitoring
        if (monitor_)
        {
          auto statusCallback = [this](NetworkStatus status)
          {
            onNetworkStatusChanged(status);
          };
          monitor_->startMonitoring(statusCallback);
        }
        cout << "[NetworkService] Switched to automatic network monitoring mode" << endl;
      }
    }
  }

  bool NetworkService::setNetworkStatus(NetworkStatus status)
  {
    if (!manualMode_)
    {
      return false; // Manual mode must be enabled
    }

    NetworkStatus oldStatus = manualStatus_;
    manualStatus_ = status;

    // Trigger status change notification if the status actually changed
    if (oldStatus != status)
    {
      onNetworkStatusChanged(status);
    }

    return true;
  }

} // namespace runtime