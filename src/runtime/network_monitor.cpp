#include "network_monitor.hpp"

#if UNITY_ANDROID
#include <android/log.h>
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <chrono>

namespace runtime
{
  std::shared_ptr<NetworkMonitor> NetworkMonitor::create()
  {
#if UNITY_ANDROID
    return std::make_shared<AndroidNetworkMonitor>();
#elif UNITY_OSX
    return std::make_shared<MacOSNetworkMonitor>();
#else
    // For other platforms, use the socket-based implementation as default
    return std::make_shared<SocketBasedNetworkMonitor>();
#endif
  }

  // SocketBasedNetworkMonitor implementation
  SocketBasedNetworkMonitor::~SocketBasedNetworkMonitor()
  {
    stopMonitoring();
  }

  bool SocketBasedNetworkMonitor::startMonitoring(NetworkStatusCallback callback)
  {
    if (isMonitoring_)
    {
      return false;
    }

    statusCallback_ = callback;
    isMonitoring_ = true;
    stopRequested_ = false;

    // Start a background thread to monitor network status
    monitoringThread_ = std::thread([this]()
                                    {
      NetworkStatus lastStatus = NetworkStatus::Offline;
      
      while (!stopRequested_)
      {
        NetworkStatus currentStatus = getCurrentStatus();
        
        // Only notify if status has changed
        if (currentStatus != lastStatus)
        {
          lastStatus = currentStatus;
          if (statusCallback_)
          {
            statusCallback_(currentStatus);
          }
#if UNITY_ANDROID
          __android_log_print(ANDROID_LOG_INFO, "JSARRuntime", 
                              "Network status changed to: %s", 
                              currentStatus == NetworkStatus::Online ? "Online" : "Offline");
#endif
        }
        
        // Check every 2 seconds
        std::this_thread::sleep_for(std::chrono::seconds(2));
      } });

    // Notify initial status
    if (statusCallback_)
    {
      statusCallback_(getCurrentStatus());
    }

#if UNITY_ANDROID
    __android_log_print(ANDROID_LOG_INFO, "JSARRuntime", "Network monitoring started");
#endif
    return true;
  }

  void SocketBasedNetworkMonitor::stopMonitoring()
  {
    if (!isMonitoring_)
    {
      return;
    }

    // Signal the monitoring thread to stop
    stopRequested_ = true;

    // Wait for the thread to finish
    if (monitoringThread_.joinable())
    {
      monitoringThread_.join();
    }

    isMonitoring_ = false;
    statusCallback_ = nullptr;

#if UNITY_ANDROID
    __android_log_print(ANDROID_LOG_INFO, "JSARRuntime", "Network monitoring stopped");
#endif
  }

  NetworkStatus SocketBasedNetworkMonitor::getCurrentStatus() const
  {
    return testConnectivity() ? NetworkStatus::Online : NetworkStatus::Offline;
  }

  bool SocketBasedNetworkMonitor::testConnectivity() const
  {
    // Use a simple socket-based connectivity test
    // Try to create a socket and connect to a well-known server
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
      return false;
    }

    // Set socket to non-blocking for quick timeout
    struct timeval timeout;
    timeout.tv_sec = 2; // 2 second timeout
    timeout.tv_usec = 0;

    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0 ||
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0)
    {
      close(sock);
      return false;
    }

    // Try to connect to Google's public DNS (8.8.8.8) on port 53
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(53); // DNS port
    server_addr.sin_addr.s_addr = inet_addr("8.8.8.8");

    int connect_result = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    close(sock);

    // If connection succeeded, we have network connectivity
    return (connect_result == 0);
  }

} // namespace runtime