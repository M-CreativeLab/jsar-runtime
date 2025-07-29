#include "network_monitor.hpp"
#include <iostream>

#if UNITY_ANDROID
#include <jni.h>
#include <android/log.h>
#endif

#if UNITY_OSX
#include <SystemConfiguration/SystemConfiguration.h>
#include <CoreFoundation/CoreFoundation.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#endif

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

#if UNITY_ANDROID
  AndroidNetworkMonitor::~AndroidNetworkMonitor()
  {
    stopMonitoring();
  }

  bool AndroidNetworkMonitor::startMonitoring(NetworkStatusCallback callback)
  {
    if (isMonitoring_)
    {
      return false;
    }

    statusCallback_ = callback;

    // TODO: Implement Android ConnectivityManager integration
    // This would involve:
    // 1. Getting ConnectivityManager system service
    // 2. Registering NetworkCallback
    // 3. Handling network state changes

    // For now, just report current status and mark as monitoring
    isMonitoring_ = true;

    // Initial status check
    if (statusCallback_)
    {
      statusCallback_(getCurrentStatus());
    }

    __android_log_print(ANDROID_LOG_INFO, "JSARRuntime", "Network monitoring started");
    return true;
  }

  void AndroidNetworkMonitor::stopMonitoring()
  {
    if (!isMonitoring_)
    {
      return;
    }

    // TODO: Unregister NetworkCallback

    isMonitoring_ = false;
    statusCallback_ = nullptr;

    __android_log_print(ANDROID_LOG_INFO, "JSARRuntime", "Network monitoring stopped");
  }

  NetworkStatus AndroidNetworkMonitor::getCurrentStatus() const
  {
    // TODO: Implement actual network status check using ConnectivityManager
    // For now, assume online
    return NetworkStatus::Online;
  }

  void AndroidNetworkMonitor::checkNetworkStatus()
  {
    if (!isMonitoring_ || !statusCallback_)
    {
      return;
    }

    NetworkStatus currentStatus = getCurrentStatus();
    statusCallback_(currentStatus);
  }

  void AndroidNetworkMonitor::onNetworkChanged(AndroidNetworkMonitor *monitor)
  {
    if (monitor)
    {
      monitor->checkNetworkStatus();
    }
  }
#endif

#if UNITY_OSX
  MacOSNetworkMonitor::~MacOSNetworkMonitor()
  {
    stopMonitoring();
  }

  bool MacOSNetworkMonitor::startMonitoring(NetworkStatusCallback callback)
  {
    if (isMonitoring_)
    {
      return false;
    }

    statusCallback_ = callback;

    // Create reachability reference for general internet connectivity
    struct sockaddr_in zeroAddress;
    bzero(&zeroAddress, sizeof(zeroAddress));
    zeroAddress.sin_len = sizeof(zeroAddress);
    zeroAddress.sin_family = AF_INET;

    SCNetworkReachabilityRef reachability = SCNetworkReachabilityCreateWithAddress(
      kCFAllocatorDefault, (const struct sockaddr *)&zeroAddress);

    if (!reachability)
    {
      return false;
    }

    reachabilityRef_ = reachability;

    // Set up the callback
    SCNetworkReachabilityContext context = {0, this, NULL, NULL, NULL};
    if (!SCNetworkReachabilitySetCallback(reachability,
                                          reinterpret_cast<SCNetworkReachabilityCallBack>(reachabilityCallback),
                                          &context))
    {
      CFRelease(reachability);
      reachabilityRef_ = nullptr;
      return false;
    }

    // Schedule with the run loop
    if (!SCNetworkReachabilityScheduleWithRunLoop(reachability, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode))
    {
      SCNetworkReachabilitySetCallback(reachability, NULL, NULL);
      CFRelease(reachability);
      reachabilityRef_ = nullptr;
      return false;
    }

    isMonitoring_ = true;

    // Get initial status
    if (statusCallback_)
    {
      statusCallback_(getCurrentStatus());
    }

    return true;
  }

  void MacOSNetworkMonitor::stopMonitoring()
  {
    if (!isMonitoring_)
    {
      return;
    }

    if (reachabilityRef_)
    {
      SCNetworkReachabilityRef reachability = static_cast<SCNetworkReachabilityRef>(reachabilityRef_);
      SCNetworkReachabilityUnscheduleFromRunLoop(reachability, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
      SCNetworkReachabilitySetCallback(reachability, NULL, NULL);
      CFRelease(reachability);
      reachabilityRef_ = nullptr;
    }

    isMonitoring_ = false;
    statusCallback_ = nullptr;
  }

  NetworkStatus MacOSNetworkMonitor::getCurrentStatus() const
  {
    if (!reachabilityRef_)
    {
      return NetworkStatus::Offline;
    }

    SCNetworkReachabilityFlags flags;
    SCNetworkReachabilityRef reachability = static_cast<SCNetworkReachabilityRef>(reachabilityRef_);

    if (SCNetworkReachabilityGetFlags(reachability, &flags))
    {
      // Check if network is reachable
      bool isReachable = (flags & kSCNetworkReachabilityFlagsReachable) != 0;
      bool needsConnection = (flags & kSCNetworkReachabilityFlagsConnectionRequired) != 0;

      return (isReachable && !needsConnection) ? NetworkStatus::Online : NetworkStatus::Offline;
    }

    return NetworkStatus::Offline;
  }

  void MacOSNetworkMonitor::reachabilityCallback(void *target, uint32_t flags, void *info)
  {
    MacOSNetworkMonitor *monitor = static_cast<MacOSNetworkMonitor *>(info);
    if (monitor)
    {
      monitor->handleReachabilityChange(flags);
    }
  }

  void MacOSNetworkMonitor::handleReachabilityChange(uint32_t flags)
  {
    if (!isMonitoring_ || !statusCallback_)
    {
      return;
    }

    NetworkStatus currentStatus = getCurrentStatus();
    statusCallback_(currentStatus);
  }
#endif

} // namespace runtime