#include "./network_status_monitor.hpp"

#if defined(__APPLE__)

#include <SystemConfiguration/SystemConfiguration.h>
#include <CoreFoundation/CoreFoundation.h>

namespace runtime
{
  bool NetworkStatusMonitor::checkNetworkStatusApple()
  {
    // Use SystemConfiguration framework to check network reachability
    SCNetworkReachabilityRef reachability = SCNetworkReachabilityCreateWithName(NULL, "8.8.8.8");
    if (reachability == NULL)
    {
      return true; // Default to online if we can't create reachability
    }

    SCNetworkReachabilityFlags flags;
    Boolean success = SCNetworkReachabilityGetFlags(reachability, &flags);
    CFRelease(reachability);

    if (!success)
    {
      return true; // Default to online if we can't get flags
    }

    // Check if network is reachable
    bool isReachable = (flags & kSCNetworkReachabilityFlagsReachable) != 0;
    bool needsConnection = (flags & kSCNetworkReachabilityFlagsConnectionRequired) != 0;
    
    return isReachable && !needsConnection;
  }
} // namespace runtime

#endif