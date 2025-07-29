#include "network_monitor.hpp"

namespace runtime
{
  std::shared_ptr<NetworkMonitor> NetworkMonitor::create()
  {
#if UNITY_ANDROID
    return std::make_shared<AndroidNetworkMonitor>();
#elif UNITY_OSX
    return std::make_shared<MacOSNetworkMonitor>();
#else
    // For other platforms, return a basic implementation that always reports online
    class BasicNetworkMonitor : public NetworkMonitor
    {
    public:
      bool startMonitoring(NetworkStatusCallback callback) override
      {
        statusCallback_ = callback;
        isMonitoring_ = true;
        return true;
      }

      void stopMonitoring() override
      {
        isMonitoring_ = false;
        statusCallback_ = nullptr;
      }

      NetworkStatus getCurrentStatus() const override
      {
        return NetworkStatus::Online; // Assume online for unsupported platforms
      }
    };
    return std::make_shared<BasicNetworkMonitor>();
#endif
  }

} // namespace runtime