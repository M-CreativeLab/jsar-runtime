#include "./navigator.hpp"
#include <client/dom/network_status_monitor.hpp>

namespace browser
{
  Navigator::Navigator()
  {
    // Initialize with current network status
    auto& monitor = dom::getNetworkMonitor();
    online_status_ = monitor.isOnline();
  }

  bool Navigator::onLine() const
  {
    return online_status_;
  }

  void Navigator::updateOnlineStatus(bool isOnline)
  {
    online_status_ = isOnline;
  }

  Navigator& getNavigator()
  {
    static Navigator instance;
    return instance;
  }
} // namespace browser