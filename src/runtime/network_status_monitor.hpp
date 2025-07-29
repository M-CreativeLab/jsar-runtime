#pragma once

#include <functional>
#include <memory>
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>
#include "common/events_v2/native_event.hpp"

// Forward declarations
class TrConstellation;

namespace runtime
{
  /**
   * @class NetworkStatusMonitor
   * Monitors network connectivity status and dispatches online/offline events to all clients.
   * Uses platform-specific APIs when available, falls back to simple connectivity checks.
   * Runs on the server-side and broadcasts events to all connected clients.
   */
  class NetworkStatusMonitor
  {
  public:
    NetworkStatusMonitor(TrConstellation* constellation);
    ~NetworkStatusMonitor();

    /**
     * Get the current network online status
     */
    bool isOnline() const { return isOnline_.load(); }

    /**
     * Start monitoring network status changes
     */
    void startMonitoring();

    /**
     * Stop monitoring network status changes
     */
    void stopMonitoring();

    /**
     * Manually update network status (for testing or external updates)
     */
    void updateNetworkStatus(bool isOnline);

    /**
     * Check current network status using platform-specific APIs
     */
    bool checkNetworkStatus();

  private:
    /**
     * Initialize network status monitoring
     */
    void initializeNetworkStatus();

    /**
     * Monitoring loop that runs in a separate thread
     */
    void monitoringLoop();

    /**
     * Dispatch network status change event to all clients
     */
    void dispatchNetworkEventToClients(bool isOnline);

    /**
     * Platform-specific network status check implementations
     */
#if defined(__APPLE__)
    bool checkNetworkStatusApple();
#elif defined(__linux__)
    bool checkNetworkStatusLinux();
#elif defined(__ANDROID__)
    bool checkNetworkStatusAndroid();
#else
    bool checkNetworkStatusGeneric();
#endif

  private:
    TrConstellation* constellation_;
    std::atomic<bool> isOnline_{true};
    std::atomic<bool> isMonitoring_{false};
    std::unique_ptr<std::thread> monitoringThread_;
    static constexpr std::chrono::milliseconds POLLING_INTERVAL{5000}; // 5 seconds
  };

  /**
   * Initialize network monitoring for the runtime
   */
  void initializeNetworkMonitoring(TrConstellation* constellation);

  /**
   * Cleanup network monitoring
   */
  void cleanupNetworkMonitoring();
} // namespace runtime