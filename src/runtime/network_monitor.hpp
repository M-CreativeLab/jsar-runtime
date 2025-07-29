#pragma once

#include <functional>
#include <memory>
#include "platform_base.hpp"
#include "common/events_v2/native_event.hpp"

/**
 * Network status monitoring interface for different platforms.
 * Provides callbacks when network connectivity changes.
 */
class TrNetworkMonitor
{
public:
  using NetworkStatusCallback = std::function<void(bool isOnline)>;

  /**
   * Create a platform-specific network monitor instance.
   * 
   * @param callback Function to call when network status changes
   * @return Shared pointer to the network monitor instance
   */
  static std::shared_ptr<TrNetworkMonitor> create(NetworkStatusCallback callback);

  virtual ~TrNetworkMonitor() = default;

  /**
   * Start monitoring network status changes.
   * 
   * @return true if monitoring started successfully, false otherwise
   */
  virtual bool start() = 0;

  /**
   * Stop monitoring network status changes.
   */
  virtual void stop() = 0;

  /**
   * Get the current network status.
   * 
   * @return true if network is online, false otherwise
   */
  virtual bool isOnline() = 0;

protected:
  TrNetworkMonitor(NetworkStatusCallback callback) : callback_(std::move(callback)) {}
  
  NetworkStatusCallback callback_;
};

#if UNITY_ANDROID
/**
 * Android-specific network monitor using ConnectivityManager.
 */
class TrAndroidNetworkMonitor : public TrNetworkMonitor
{
public:
  TrAndroidNetworkMonitor(NetworkStatusCallback callback);
  ~TrAndroidNetworkMonitor();

  bool start() override;
  void stop() override;
  bool isOnline() override;

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};
#endif

#if UNITY_OSX
/**
 * macOS-specific network monitor using SystemConfiguration framework.
 */
class TrMacOSNetworkMonitor : public TrNetworkMonitor
{
public:
  TrMacOSNetworkMonitor(NetworkStatusCallback callback);
  ~TrMacOSNetworkMonitor();

  bool start() override;
  void stop() override;
  bool isOnline() override;

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};
#endif