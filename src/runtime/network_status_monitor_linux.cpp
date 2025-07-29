#include "./network_status_monitor.hpp"

#if defined(__linux__)

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <fcntl.h>
#include <errno.h>

namespace runtime
{
  bool NetworkStatusMonitor::checkNetworkStatusLinux()
  {
    // Simple approach: check if we have any active network interfaces
    struct ifaddrs *ifaddrs_ptr = nullptr;
    
    if (getifaddrs(&ifaddrs_ptr) != 0)
    {
      return true; // Default to online if we can't get interface info
    }

    bool hasActiveInterface = false;
    
    for (struct ifaddrs *ifa = ifaddrs_ptr; ifa != nullptr; ifa = ifa->ifa_next)
    {
      if (ifa->ifa_addr == nullptr)
        continue;
        
      // Check for IPv4 or IPv6 interfaces that are up and not loopback
      if ((ifa->ifa_addr->sa_family == AF_INET || ifa->ifa_addr->sa_family == AF_INET6) &&
          (ifa->ifa_flags & IFF_UP) && !(ifa->ifa_flags & IFF_LOOPBACK))
      {
        hasActiveInterface = true;
        break;
      }
    }
    
    freeifaddrs(ifaddrs_ptr);
    return hasActiveInterface;
  }
} // namespace runtime

#endif