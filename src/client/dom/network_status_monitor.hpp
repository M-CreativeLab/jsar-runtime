#pragma once

#include <functional>
#include <memory>
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>

// Forward declarations
namespace browser
{
  class Window;
}

namespace dom
{
  /**
   * @class NetworkStatusMonitor
   * Monitors network connectivity status and dispatches online/offline events.
   * Uses platform-specific APIs when available, falls back to simple connectivity checks.
   */
  class NetworkStatusMonitor
  {
  public:
    NetworkStatusMonitor();
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
     * Add a callback to be notified of network status changes
     */
    void addStatusChangeCallback(std::function<void(bool)> callback);

    /**
     * Remove all status change callbacks
     */
    void clearStatusChangeCallbacks();

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
     * Dispatch network status change event
     */
    void dispatchNetworkEvent(bool isOnline);

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
    std::atomic<bool> isOnline_{true};
    std::atomic<bool> isMonitoring_{false};
    std::unique_ptr<std::thread> monitoringThread_;
    std::vector<std::function<void(bool)>> statusChangeCallbacks_;
    static constexpr std::chrono::milliseconds POLLING_INTERVAL{5000}; // 5 seconds
  };

  /**
   * Get the global network monitor instance
   */
  NetworkStatusMonitor& getNetworkMonitor();

  /**
   * Initialize network monitoring for the window object
   */
  void initializeNetworkMonitoring();

  /**
   * Cleanup network monitoring
   */
  void cleanupNetworkMonitoring();
} // namespace dom