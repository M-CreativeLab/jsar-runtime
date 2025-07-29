#include "./navigator.hpp"

namespace browser
{
  Navigator::Navigator()
  {
    // Initialize with default online status (will be updated by server events)
    online_status_ = true;
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