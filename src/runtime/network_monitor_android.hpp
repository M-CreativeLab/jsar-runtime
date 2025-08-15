#pragma once

#include "./network_monitor.hpp"
#include "./embedder.hpp"

#if defined(__ANDROID__)

#include <jni.h>
#include <thread>
#include <chrono>

namespace runtime
{
  /**
   * AndroidNetworkMonitor - JNI-based implementation using ConnectivityManager
   */
  class AndroidNetworkMonitor : public NetworkMonitor
  {
  public:
    AndroidNetworkMonitor(TrEmbedder *);
    ~AndroidNetworkMonitor() override;

    bool startMonitoring(NetworkStatusCallback callback) override;
    void stopMonitoring() override;
    NetworkStatus getCurrentStatus() const override;

  private:
    bool initialize();
    void cleanup();

    bool isNetworkAvailable() const;
    void monitoringLoop();

  private:
    TrEmbedder *embedder_ = nullptr;
    JavaVM *javaVM_ = nullptr;
    jclass contextClass_ = nullptr;
    jobject contextObject_ = nullptr;

    jobject connectivityManager_ = nullptr;
    jclass connectivityManagerClass_ = nullptr;
    jmethodID getSystemServiceMethod_ = nullptr;
    jmethodID getActiveNetworkMethod_ = nullptr;
    jmethodID getActiveNetworkInfoMethod_ = nullptr;
    jmethodID isConnectedMethod_ = nullptr;

    // Monitoring related members
    std::thread monitoringThread_;
    std::atomic<bool> stopRequested_{false};
    bool initialized_ = false;
  };
} // namespace runtime

#endif // UNITY_ANDROID