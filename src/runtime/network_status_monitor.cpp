#include "./network_status_monitor.hpp"
#include "./constellation.hpp"
#include <common/events_v2/native_event.hpp>
#include <runtime/platform_base.hpp>

// Common socket includes for generic implementation
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

namespace runtime
{
  // Global instance
  static std::unique_ptr<NetworkStatusMonitor> g_networkMonitor = nullptr;

  NetworkStatusMonitor::NetworkStatusMonitor(TrConstellation* constellation)
    : constellation_(constellation)
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
        dispatchNetworkEventToClients(currentStatus);
      }

      std::this_thread::sleep_for(POLLING_INTERVAL);
    }
  }

  void NetworkStatusMonitor::updateNetworkStatus(bool isOnline)
  {
    if (isOnline != isOnline_.load())
    {
      isOnline_.store(isOnline);
      dispatchNetworkEventToClients(isOnline);
    }
  }

  void NetworkStatusMonitor::dispatchNetworkEventToClients(bool isOnline)
  {
    // Create network status event and dispatch to all clients
    if (constellation_ && constellation_->nativeEventTarget)
    {
      auto networkEvent = std::make_shared<events_comm::TrNetworkStatusChanged>(isOnline);
      auto event = std::make_shared<events_comm::TrNativeEvent>(
        events_comm::TrNativeEventType::NetworkStatusChanged,
        networkEvent
      );
      
      // Dispatch to all connected clients
      constellation_->nativeEventTarget->dispatchEvent(
        events_comm::TrNativeEventType::NetworkStatusChanged,
        event
      );
    }
  }

  bool NetworkStatusMonitor::checkNetworkStatus()
  {
#if defined(__APPLE__)
    return checkNetworkStatusApple();
#elif defined(__linux__)
    return checkNetworkStatusLinux();
#elif defined(__ANDROID__)
    return checkNetworkStatusAndroid();
#else
    return checkNetworkStatusGeneric();
#endif
  }

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

  void initializeNetworkMonitoring(TrConstellation* constellation)
  {
    if (!g_networkMonitor)
    {
      g_networkMonitor = std::make_unique<NetworkStatusMonitor>(constellation);
      g_networkMonitor->startMonitoring();
    }
  }

  void cleanupNetworkMonitoring()
  {
    if (g_networkMonitor)
    {
      g_networkMonitor->stopMonitoring();
      g_networkMonitor.reset();
    }
  }
} // namespace runtime