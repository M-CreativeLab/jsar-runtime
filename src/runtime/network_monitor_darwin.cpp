#include "network_monitor.hpp"

#if defined(__APPLE__)
#include <SystemConfiguration/SystemConfiguration.h>
#include <CoreFoundation/CoreFoundation.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>

namespace runtime
{
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

    reachabilityRef_ = (void*)reachability;

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

} // namespace runtime

#endif // UNITY_OSX