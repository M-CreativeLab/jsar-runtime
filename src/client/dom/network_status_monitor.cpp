#include "./network_status_monitor.hpp"
#include <client/browser/window.hpp>
#include <client/browser/navigator.hpp>
#include <common/events_v2/event.hpp>
#include <src/runtime/platform_base.hpp>

// Platform-specific includes
#if defined(UNITY_WIN)
#include <windows.h>
#include <netlistmgr.h>
#include <comdef.h>
#pragma comment(lib, "ole32.lib")
#elif defined(UNITY_OSX) || defined(UNITY_IOS) || defined(UNITY_TVOS)
#include <SystemConfiguration/SystemConfiguration.h>
#include <CoreFoundation/CoreFoundation.h>
#elif defined(UNITY_LINUX) || defined(UNITY_EMBEDDED_LINUX) || defined(UNITY_EMBEDDED_LINUX_GL)
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <fcntl.h>
#include <errno.h>
#elif defined(UNITY_ANDROID)
// Android-specific headers would go here
// For now, use generic implementation
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#endif

#if !defined(UNITY_WIN)
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#endif

namespace dom
{
  // Global instance
  static std::unique_ptr<NetworkStatusMonitor> g_networkMonitor = nullptr;

  NetworkStatusMonitor::NetworkStatusMonitor()
      : DOMEventTarget()
  {
    initializeNetworkStatus();
  }

  NetworkStatusMonitor::~NetworkStatusMonitor()
  {
    stopMonitoring();
  }

  void NetworkStatusMonitor::initializeNetworkStatus()
  {
    // Set initial status
    isOnline_.store(checkNetworkStatus());
  }

  void NetworkStatusMonitor::startMonitoring()
  {
    if (isMonitoring_.load())
    {
      return; // Already monitoring
    }

    isMonitoring_.store(true);
    monitoringThread_ = std::make_unique<std::thread>(&NetworkStatusMonitor::monitoringLoop, this);
  }

  void NetworkStatusMonitor::stopMonitoring()
  {
    isMonitoring_.store(false);
    if (monitoringThread_ && monitoringThread_->joinable())
    {
      monitoringThread_->join();
      monitoringThread_.reset();
    }
  }

  void NetworkStatusMonitor::monitoringLoop()
  {
    while (isMonitoring_.load())
    {
      bool currentStatus = checkNetworkStatus();
      
      if (currentStatus != isOnline_.load())
      {
        isOnline_.store(currentStatus);
        dispatchNetworkEvent(currentStatus);
      }

      std::this_thread::sleep_for(POLLING_INTERVAL);
    }
  }

  void NetworkStatusMonitor::updateNetworkStatus(bool isOnline)
  {
    if (isOnline != isOnline_.load())
    {
      isOnline_.store(isOnline);
      dispatchNetworkEvent(isOnline);
    }
  }

  void NetworkStatusMonitor::dispatchNetworkEvent(bool isOnline)
  {
    // Update navigator.onLine
    auto& navigator = browser::getNavigator();
    navigator.updateOnlineStatus(isOnline);
    
    // Notify all registered callbacks
    for (auto& callback : statusChangeCallbacks_)
    {
      callback(isOnline);
    }
  }

  void NetworkStatusMonitor::addStatusChangeCallback(std::function<void(bool)> callback)
  {
    statusChangeCallbacks_.push_back(callback);
  }

  void NetworkStatusMonitor::clearStatusChangeCallbacks()
  {
    statusChangeCallbacks_.clear();
  }

  bool NetworkStatusMonitor::checkNetworkStatus()
  {
#if defined(UNITY_WIN)
    return checkNetworkStatusWindows();
#elif defined(UNITY_OSX) || defined(UNITY_IOS) || defined(UNITY_TVOS)
    return checkNetworkStatusApple();
#elif defined(UNITY_LINUX) || defined(UNITY_EMBEDDED_LINUX) || defined(UNITY_EMBEDDED_LINUX_GL)
    return checkNetworkStatusLinux();
#elif defined(UNITY_ANDROID)
    return checkNetworkStatusAndroid();
#else
    return checkNetworkStatusGeneric();
#endif
  }

#if defined(UNITY_WIN)
  bool NetworkStatusMonitor::checkNetworkStatusWindows()
  {
    try
    {
      // Use Windows Network Location Awareness (NLA) API
      HRESULT hr = CoInitialize(NULL);
      if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
      {
        return true; // Default to online if COM initialization fails
      }

      INetworkListManager* pNetworkListManager = nullptr;
      hr = CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_ALL,
                           IID_INetworkListManager, (LPVOID*)&pNetworkListManager);

      if (SUCCEEDED(hr) && pNetworkListManager)
      {
        NLM_CONNECTIVITY connectivity;
        hr = pNetworkListManager->GetConnectivity(&connectivity);
        pNetworkListManager->Release();
        
        if (SUCCEEDED(hr))
        {
          // Check if we have internet connectivity
          bool hasConnectivity = (connectivity & NLM_CONNECTIVITY_IPV4_INTERNET) ||
                                (connectivity & NLM_CONNECTIVITY_IPV6_INTERNET);
          return hasConnectivity;
        }
      }

      CoUninitialize();
    }
    catch (...)
    {
      // If anything fails, default to online
    }
    
    return true; // Default to online
  }
#endif

#if defined(UNITY_OSX) || defined(UNITY_IOS) || defined(UNITY_TVOS)
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
#endif

#if defined(UNITY_LINUX) || defined(UNITY_EMBEDDED_LINUX) || defined(UNITY_EMBEDDED_LINUX_GL)
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
#endif

#if defined(UNITY_ANDROID)
  bool NetworkStatusMonitor::checkNetworkStatusAndroid()
  {
    // For Android, we would typically use the ConnectivityManager
    // This requires JNI calls to the Android API
    // For now, use the generic implementation
    return checkNetworkStatusGeneric();
  }
#endif

  bool NetworkStatusMonitor::checkNetworkStatusGeneric()
  {
    // Generic fallback: simple socket test
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
      return true; // Default to online if we can't create socket
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(53); // DNS port
    inet_pton(AF_INET, "8.8.8.8", &addr.sin_addr); // Google DNS

    // Set socket to non-blocking for quick test
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    int result = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    close(sock);

    // If connect succeeds immediately or returns EINPROGRESS, we likely have connectivity
    return (result == 0 || errno == EINPROGRESS);
  }

  NetworkStatusMonitor& getNetworkMonitor()
  {
    if (!g_networkMonitor)
    {
      g_networkMonitor = std::make_unique<NetworkStatusMonitor>();
    }
    return *g_networkMonitor;
  }

  void initializeNetworkMonitoring()
  {
    auto& monitor = getNetworkMonitor();
    monitor.startMonitoring();
  }

  void cleanupNetworkMonitoring()
  {
    if (g_networkMonitor)
    {
      g_networkMonitor->stopMonitoring();
      g_networkMonitor.reset();
    }
  }
} // namespace dom