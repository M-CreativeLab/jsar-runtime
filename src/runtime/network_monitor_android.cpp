#include <cassert>
#include <common/debug.hpp>
#include "./network_monitor.hpp"
#include "./network_monitor_android.hpp"

#if defined(__ANDROID__)

#include <jni.h>
#include <thread>
#include <chrono>

namespace runtime
{
  AndroidNetworkMonitor::AndroidNetworkMonitor(TrEmbedder *embedder)
      : embedder_(embedder)
  {
    assert(embedder_ != nullptr && embedder_->isJNIInitialized() &&
           "AndroidNetworkMonitor requires a valid embedder with JNI initialized");

    this->javaVM_ = embedder_->getJavaVM();
    this->contextClass_ = embedder_->getJavaContextClass();
    this->contextObject_ = embedder_->getJavaContextObject();
  }

  AndroidNetworkMonitor::~AndroidNetworkMonitor()
  {
    stopMonitoring();
    cleanup();
  }

  bool AndroidNetworkMonitor::startMonitoring(NetworkStatusCallback callback)
  {
    if (isMonitoring_)
      return false;

    if (!initialize())
    {
      DEBUG(LOG_TAG_ERROR, "Failed to initialize for Android network monitoring");
      return false;
    }

    statusCallback_ = callback;
    isMonitoring_ = true;
    stopRequested_ = false;

    // Start background monitoring thread
    monitoringThread_ = std::thread([this]()
                                    { monitoringLoop(); });

    // Notify initial status
    if (statusCallback_)
      statusCallback_(getCurrentStatus());

    return true;
  }

  void AndroidNetworkMonitor::stopMonitoring()
  {
    if (!isMonitoring_)
    {
      return;
    }

    stopRequested_ = true;

    if (monitoringThread_.joinable())
      monitoringThread_.join();

    isMonitoring_ = false;
    statusCallback_ = nullptr;
    DEBUG(LOG_TAG_JSAR, "Android JNI network monitoring stopped");
  }

  NetworkStatus AndroidNetworkMonitor::getCurrentStatus() const
  {
    return isNetworkAvailable()
             ? NetworkStatus::Online
             : NetworkStatus::Offline;
  }

  bool AndroidNetworkMonitor::initialize()
  {
    if (initialized_)
      return true;

    auto env = embedder_->getJavaEnv();
    getSystemServiceMethod_ = env->GetMethodID(contextClass_,
                                               "getSystemService",
                                               "(Ljava/lang/String;)Ljava/lang/Object;");
    if (getSystemServiceMethod_ == nullptr)
    {
      DEBUG(LOG_TAG_ERROR, "Failed to get getSystemService method");
      return false;
    }

    // Get ConnectivityManager
    jstring serviceString = env->NewStringUTF("connectivity");
    jobject connectivityManagerObj = env->CallObjectMethod(contextObject_, getSystemServiceMethod_, serviceString);
    if (connectivityManagerObj == nullptr)
    {
      DEBUG(LOG_TAG_ERROR, "Failed to get ConnectivityManager");
      env->DeleteLocalRef(serviceString);
      return false;
    }

    connectivityManager_ = env->NewGlobalRef(connectivityManagerObj);
    connectivityManagerClass_ = (jclass)env->NewGlobalRef(env->FindClass("android/net/ConnectivityManager"));
    if (connectivityManagerClass_ == nullptr)
    {
      DEBUG(LOG_TAG_ERROR, "Failed to find ConnectivityManager class");
      env->DeleteLocalRef(serviceString);
      env->DeleteLocalRef(connectivityManagerObj);
      return false;
    }

    // Get method IDs for network checking
    // Try modern API first (API 23+)
    getActiveNetworkMethod_ = env->GetMethodID(connectivityManagerClass_,
                                               "getActiveNetwork",
                                               "()Landroid/net/Network;");

    // Fallback to deprecated but widely supported API
    getActiveNetworkInfoMethod_ = env->GetMethodID(connectivityManagerClass_,
                                                   "getActiveNetworkInfo",
                                                   "()Landroid/net/NetworkInfo;");

    if (getActiveNetworkInfoMethod_ != nullptr)
    {
      // Get NetworkInfo.isConnected method
      jclass networkInfoClass = env->FindClass("android/net/NetworkInfo");
      if (networkInfoClass != nullptr)
      {
        isConnectedMethod_ = env->GetMethodID(networkInfoClass, "isConnected", "()Z");
        env->DeleteLocalRef(networkInfoClass);
      }
    }

    // Cleanup local references
    env->DeleteLocalRef(serviceString);
    env->DeleteLocalRef(connectivityManagerObj);

    DEBUG(LOG_TAG_JSAR, "Android JNI network monitoring initialized successfully");
    initialized_ = true;
    return true;
  }

  void AndroidNetworkMonitor::cleanup()
  {
    if (!initialized_ || javaVM_ == nullptr)
      return;

    JNIEnv *env = embedder_->getJavaEnv();
    if (connectivityManager_ != nullptr)
    {
      env->DeleteGlobalRef(connectivityManager_);
      connectivityManager_ = nullptr;
    }
    if (connectivityManagerClass_ != nullptr)
    {
      env->DeleteGlobalRef(connectivityManagerClass_);
      connectivityManagerClass_ = nullptr;
    }
    initialized_ = false;
  }

  bool AndroidNetworkMonitor::isNetworkAvailable() const
  {
    if (!initialized_ || javaVM_ == nullptr || connectivityManager_ == nullptr)
    {
      DEBUG(LOG_TAG_ERROR, "JNI not initialized or ConnectivityManager is null");
      return false;
    }

    JNIEnv *env = embedder_->getJavaEnv();
    bool isConnected = false;

    // Try modern approach first (API 23+)
    if (getActiveNetworkMethod_ != nullptr)
    {
      jobject activeNetwork = env->CallObjectMethod(connectivityManager_, getActiveNetworkMethod_);
      isConnected = (activeNetwork != nullptr);
      if (activeNetwork != nullptr)
        env->DeleteLocalRef(activeNetwork);
    }
    // Fallback to deprecated but widely supported approach
    else if (getActiveNetworkInfoMethod_ != nullptr && isConnectedMethod_ != nullptr)
    {
      jobject networkInfo = env->CallObjectMethod(connectivityManager_, getActiveNetworkInfoMethod_);
      if (networkInfo != nullptr)
      {
        isConnected = env->CallBooleanMethod(networkInfo, isConnectedMethod_);
        env->DeleteLocalRef(networkInfo);
      }
    }
    else
    {
      DEBUG(LOG_TAG_ERROR, "No valid method to check network connectivity");
      return false;
    }
    return isConnected;
  }

  void AndroidNetworkMonitor::monitoringLoop()
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
          statusCallback_(currentStatus);

        // Check every 2 seconds
        std::this_thread::sleep_for(std::chrono::seconds(2));
      }
    }
  }
}
#endif
