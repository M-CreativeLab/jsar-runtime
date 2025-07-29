#include "network_monitor.hpp"
#include "debug.hpp"

#if UNITY_ANDROID
#include <jni.h>
#include <android/api-level.h>
#endif

#if UNITY_OSX
#include <SystemConfiguration/SystemConfiguration.h>
#include <CoreFoundation/CoreFoundation.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#endif

// Static factory method
std::shared_ptr<TrNetworkMonitor> TrNetworkMonitor::create(NetworkStatusCallback callback)
{
#if UNITY_ANDROID
  return std::make_shared<TrAndroidNetworkMonitor>(std::move(callback));
#elif UNITY_OSX
  return std::make_shared<TrMacOSNetworkMonitor>(std::move(callback));
#else
  // For other platforms, create a stub that always reports online
  class StubNetworkMonitor : public TrNetworkMonitor
  {
  public:
    StubNetworkMonitor(NetworkStatusCallback callback) : TrNetworkMonitor(std::move(callback)) {}
    bool start() override { return true; }
    void stop() override {}
    bool isOnline() override { return true; }
  };
  return std::make_shared<StubNetworkMonitor>(std::move(callback));
#endif
}

#if UNITY_ANDROID

class TrAndroidNetworkMonitor::Impl
{
public:
  Impl(NetworkStatusCallback callback) : callback_(std::move(callback)) {}
  
  bool start()
  {
    // TODO: Implement Android ConnectivityManager monitoring
    // This would require JNI calls to register network callbacks
    DEBUG(LOG_TAG_INFO, "Android network monitoring started (stub implementation)");
    return true;
  }
  
  void stop()
  {
    // TODO: Implement Android network callback unregistration
    DEBUG(LOG_TAG_INFO, "Android network monitoring stopped (stub implementation)");
  }
  
  bool isOnline()
  {
    // TODO: Implement actual network status check using ConnectivityManager
    // For now, assume we're online
    return true;
  }

private:
  NetworkStatusCallback callback_;
};

TrAndroidNetworkMonitor::TrAndroidNetworkMonitor(NetworkStatusCallback callback)
  : TrNetworkMonitor(std::move(callback))
  , impl_(std::make_unique<Impl>(callback_))
{
}

TrAndroidNetworkMonitor::~TrAndroidNetworkMonitor() = default;

bool TrAndroidNetworkMonitor::start()
{
  return impl_->start();
}

void TrAndroidNetworkMonitor::stop()
{
  impl_->stop();
}

bool TrAndroidNetworkMonitor::isOnline()
{
  return impl_->isOnline();
}

#endif // UNITY_ANDROID

#if UNITY_OSX

class TrMacOSNetworkMonitor::Impl
{
public:
  Impl(NetworkStatusCallback callback) : callback_(std::move(callback)), reachability_(nullptr) {}
  
  ~Impl()
  {
    stop();
  }
  
  bool start()
  {
    // Create a reachability reference for monitoring internet connectivity
    struct sockaddr_in zeroAddress;
    bzero(&zeroAddress, sizeof(zeroAddress));
    zeroAddress.sin_len = sizeof(zeroAddress);
    zeroAddress.sin_family = AF_INET;
    
    reachability_ = SCNetworkReachabilityCreateWithAddress(kCFAllocatorDefault,
                                                          (const struct sockaddr*)&zeroAddress);
    
    if (!reachability_) {
      DEBUG(LOG_TAG_ERROR, "Failed to create SCNetworkReachability reference");
      return false;
    }
    
    // Set up the callback context
    SCNetworkReachabilityContext context = {0, this, nullptr, nullptr, nullptr};
    
    // Set the callback
    if (!SCNetworkReachabilitySetCallback(reachability_, networkReachabilityCallback, &context)) {
      DEBUG(LOG_TAG_ERROR, "Failed to set SCNetworkReachability callback");
      CFRelease(reachability_);
      reachability_ = nullptr;
      return false;
    }
    
    // Schedule on the current run loop
    if (!SCNetworkReachabilityScheduleWithRunLoop(reachability_, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode)) {
      DEBUG(LOG_TAG_ERROR, "Failed to schedule SCNetworkReachability on run loop");
      SCNetworkReachabilitySetCallback(reachability_, nullptr, nullptr);
      CFRelease(reachability_);
      reachability_ = nullptr;
      return false;
    }
    
    DEBUG(LOG_TAG_INFO, "macOS network monitoring started");
    return true;
  }
  
  void stop()
  {
    if (reachability_) {
      SCNetworkReachabilityUnscheduleFromRunLoop(reachability_, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
      SCNetworkReachabilitySetCallback(reachability_, nullptr, nullptr);
      CFRelease(reachability_);
      reachability_ = nullptr;
      DEBUG(LOG_TAG_INFO, "macOS network monitoring stopped");
    }
  }
  
  bool isOnline()
  {
    if (!reachability_) {
      return true; // Default to online if not monitoring
    }
    
    SCNetworkReachabilityFlags flags;
    if (SCNetworkReachabilityGetFlags(reachability_, &flags)) {
      return isReachableWithFlags(flags);
    }
    
    return true; // Default to online if we can't get flags
  }

private:
  static void networkReachabilityCallback(SCNetworkReachabilityRef target,
                                        SCNetworkReachabilityFlags flags,
                                        void* info)
  {
    Impl* impl = static_cast<Impl*>(info);
    bool isOnline = impl->isReachableWithFlags(flags);
    
    DEBUG(LOG_TAG_INFO, "Network status changed: %s", isOnline ? "online" : "offline");
    
    if (impl->callback_) {
      impl->callback_(isOnline);
    }
  }
  
  bool isReachableWithFlags(SCNetworkReachabilityFlags flags)
  {
    // Check if the network is reachable
    if (!(flags & kSCNetworkReachabilityFlagsReachable)) {
      return false;
    }
    
    // If connection is required (dial-up, VPN, etc.) and there's no active connection,
    // then we're not really reachable
    if (flags & kSCNetworkReachabilityFlagsConnectionRequired) {
      if (!(flags & (kSCNetworkReachabilityFlagsConnectionOnDemand | 
                     kSCNetworkReachabilityFlagsConnectionOnTraffic))) {
        return false;
      }
    }
    
    // Check if user intervention is required
    if (flags & kSCNetworkReachabilityFlagsInterventionRequired) {
      return false;
    }
    
    return true;
  }

  NetworkStatusCallback callback_;
  SCNetworkReachabilityRef reachability_;
};

TrMacOSNetworkMonitor::TrMacOSNetworkMonitor(NetworkStatusCallback callback)
  : TrNetworkMonitor(std::move(callback))
  , impl_(std::make_unique<Impl>(callback_))
{
}

TrMacOSNetworkMonitor::~TrMacOSNetworkMonitor() = default;

bool TrMacOSNetworkMonitor::start()
{
  return impl_->start();
}

void TrMacOSNetworkMonitor::stop()
{
  impl_->stop();
}

bool TrMacOSNetworkMonitor::isOnline()
{
  return impl_->isOnline();
}

#endif // UNITY_OSX