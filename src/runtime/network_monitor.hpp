#pragma once

#include <memory>
#include <functional>
#include <thread>
#include <atomic>

#include "./constellation.hpp"

namespace runtime
{
  /**
   * @enum NetworkStatus
   * Represents the current network connectivity status.
   */
  enum class NetworkStatus
  {
    Online, // Network is available
    Offline // Network is not available
  };

  /**
   * @class NetworkMonitor
   * Base class for monitoring network connectivity status across platforms.
   * Provides a callback mechanism to notify when network status changes.
   */
  class NetworkMonitor
  {
  public:
    using NetworkStatusCallback = std::function<void(NetworkStatus status)>;

    NetworkMonitor() = default;
    virtual ~NetworkMonitor() = default;

    /**
     * Start monitoring network status changes.
     * @param callback Function to call when network status changes
     * @return true if monitoring started successfully, false otherwise
     */
    virtual bool startMonitoring(NetworkStatusCallback callback) = 0;

    /**
     * Stop monitoring network status changes.
     */
    virtual void stopMonitoring() = 0;

    /**
     * Get the current network status.
     * @return Current network connectivity status
     */
    virtual NetworkStatus getCurrentStatus() const = 0;

    /**
     * Create a platform-specific network monitor instance.
     * @return Shared pointer to the appropriate NetworkMonitor implementation
     */
    static std::shared_ptr<NetworkMonitor> Create(TrConstellation *constellation);

  protected:
    NetworkStatusCallback statusCallback_;
    bool isMonitoring_ = false;
  };

  /**
   * @class DefaultNetworkMonitor
   * Common implementation using socket-based connectivity detection.
   * This can be used as a fallback or default implementation for platforms
   * that don't have platform-specific network monitoring APIs.
   */
  class DefaultNetworkMonitor : public NetworkMonitor
  {
  public:
    DefaultNetworkMonitor() = default;
    ~DefaultNetworkMonitor() override;

    bool startMonitoring(NetworkStatusCallback callback) override;
    void stopMonitoring() override;
    NetworkStatus getCurrentStatus() const override;

  protected:
    /**
     * Test network connectivity using socket connection.
     * @return true if network is available, false otherwise
     */
    bool testConnectivity() const;

  private:
    std::thread monitoringThread_;
    std::atomic<bool> stopRequested_{false};
  };

#if defined(__APPLE__)
  /**
   * @class MacOSNetworkMonitor
   * Darwin/macOS-specific implementation using SCNetworkReachabilitySetCallback.
   */
  class MacOSNetworkMonitor : public NetworkMonitor
  {
  public:
    MacOSNetworkMonitor() = default;
    ~MacOSNetworkMonitor() override;

    bool startMonitoring(NetworkStatusCallback callback) override;
    void stopMonitoring() override;
    NetworkStatus getCurrentStatus() const override;

  private:
    static void reachabilityCallback(void *target, uint32_t flags, void *info);
    void handleReachabilityChange(uint32_t flags);

    // Platform-specific members
    void *reachabilityRef_ = nullptr;
    void *runLoopSource_ = nullptr;
  };
#endif

} // namespace runtime