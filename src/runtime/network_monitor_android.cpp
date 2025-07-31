#include "network_monitor.hpp"
#include "common/debug.hpp"

#if UNITY_ANDROID

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
    AndroidNetworkMonitor() = default;
    ~AndroidNetworkMonitor() override;

    bool startMonitoring(NetworkStatusCallback callback) override;
    void stopMonitoring() override;
    NetworkStatus getCurrentStatus() const override;

  private:
    bool initializeJNI();
    void cleanupJNI();
    bool isNetworkAvailable() const;
    void monitoringLoop();

    // JNI related members
    JavaVM* javaVM_ = nullptr;
    jobject contextObject_ = nullptr;
    jobject connectivityManager_ = nullptr;
    jclass contextClass_ = nullptr;
    jclass connectivityManagerClass_ = nullptr;
    jmethodID getSystemServiceMethod_ = nullptr;
    jmethodID getActiveNetworkMethod_ = nullptr;
    jmethodID getActiveNetworkInfoMethod_ = nullptr;
    jmethodID isConnectedMethod_ = nullptr;

    // Monitoring related members
    std::thread monitoringThread_;
    std::atomic<bool> stopRequested_{false};
    bool jniInitialized_ = false;
  };

  AndroidNetworkMonitor::~AndroidNetworkMonitor()
  {
    stopMonitoring();
    cleanupJNI();
  }

  bool AndroidNetworkMonitor::startMonitoring(NetworkStatusCallback callback)
  {
    if (isMonitoring_)
    {
      return false;
    }

    if (!initializeJNI())
    {
      DEBUG(LOG_TAG_JSAR, "Failed to initialize JNI for Android network monitoring");
      return false;
    }

    statusCallback_ = callback;
    isMonitoring_ = true;
    stopRequested_ = false;

    // Start background monitoring thread
    monitoringThread_ = std::thread([this]() { monitoringLoop(); });

    // Notify initial status
    if (statusCallback_)
    {
      statusCallback_(getCurrentStatus());
    }

    DEBUG(LOG_TAG_JSAR, "Android JNI network monitoring started");
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
    {
      monitoringThread_.join();
    }

    isMonitoring_ = false;
    statusCallback_ = nullptr;

    DEBUG(LOG_TAG_JSAR, "Android JNI network monitoring stopped");
  }

  NetworkStatus AndroidNetworkMonitor::getCurrentStatus() const
  {
    return isNetworkAvailable() ? NetworkStatus::Online : NetworkStatus::Offline;
  }

  bool AndroidNetworkMonitor::initializeJNI()
  {
    if (jniInitialized_)
    {
      return true;
    }

    // Get JavaVM - in Unity plugins, this is typically available
    // Unity provides access to the JavaVM through JNI
    JNIEnv* env = nullptr;
    
    // Try to get current JNI environment
    jint result = JNI_GetCreatedJavaVMs(&javaVM_, 1, nullptr);
    if (result != JNI_OK || javaVM_ == nullptr)
    {
      DEBUG(LOG_TAG_JSAR, "Failed to get JavaVM");
      return false;
    }

    result = javaVM_->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (result == JNI_EDETACHED)
    {
      result = javaVM_->AttachCurrentThread(&env, nullptr);
    }
    
    if (result != JNI_OK || env == nullptr)
    {
      DEBUG(LOG_TAG_JSAR, "Failed to get JNI environment");
      return false;
    }

    // Get Unity's current activity as context
    // Unity typically exposes the current activity through UnityPlayer.currentActivity
    jclass unityPlayerClass = env->FindClass("com/unity3d/player/UnityPlayer");
    if (unityPlayerClass == nullptr)
    {
      DEBUG(LOG_TAG_JSAR, "Failed to find UnityPlayer class");
      return false;
    }

    jfieldID currentActivityField = env->GetStaticFieldID(unityPlayerClass, "currentActivity", "Landroid/app/Activity;");
    if (currentActivityField == nullptr)
    {
      env->DeleteLocalRef(unityPlayerClass);
      DEBUG(LOG_TAG_JSAR, "Failed to get currentActivity field");
      return false;
    }

    jobject currentActivity = env->GetStaticObjectField(unityPlayerClass, currentActivityField);
    if (currentActivity == nullptr)
    {
      env->DeleteLocalRef(unityPlayerClass);
      DEBUG(LOG_TAG_JSAR, "Failed to get current activity");
      return false;
    }

    // Keep a global reference to the context
    contextObject_ = env->NewGlobalRef(currentActivity);
    
    // Get Context class and getSystemService method
    contextClass_ = (jclass)env->NewGlobalRef(env->FindClass("android/content/Context"));
    if (contextClass_ == nullptr)
    {
      DEBUG(LOG_TAG_JSAR, "Failed to find Context class");
      env->DeleteLocalRef(unityPlayerClass);
      env->DeleteLocalRef(currentActivity);
      return false;
    }

    getSystemServiceMethod_ = env->GetMethodID(contextClass_, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    if (getSystemServiceMethod_ == nullptr)
    {
      DEBUG(LOG_TAG_JSAR, "Failed to get getSystemService method");
      env->DeleteLocalRef(unityPlayerClass);
      env->DeleteLocalRef(currentActivity);
      return false;
    }

    // Get ConnectivityManager
    jstring serviceString = env->NewStringUTF("connectivity");
    jobject connectivityManagerObj = env->CallObjectMethod(contextObject_, getSystemServiceMethod_, serviceString);
    
    if (connectivityManagerObj == nullptr)
    {
      DEBUG(LOG_TAG_JSAR, "Failed to get ConnectivityManager");
      env->DeleteLocalRef(unityPlayerClass);
      env->DeleteLocalRef(currentActivity);
      env->DeleteLocalRef(serviceString);
      return false;
    }

    connectivityManager_ = env->NewGlobalRef(connectivityManagerObj);

    // Get ConnectivityManager class and methods
    connectivityManagerClass_ = (jclass)env->NewGlobalRef(env->FindClass("android/net/ConnectivityManager"));
    if (connectivityManagerClass_ == nullptr)
    {
      DEBUG(LOG_TAG_JSAR, "Failed to find ConnectivityManager class");
      env->DeleteLocalRef(unityPlayerClass);
      env->DeleteLocalRef(currentActivity);
      env->DeleteLocalRef(serviceString);
      env->DeleteLocalRef(connectivityManagerObj);
      return false;
    }

    // Get method IDs for network checking
    // Try modern API first (API 23+)
    getActiveNetworkMethod_ = env->GetMethodID(connectivityManagerClass_, "getActiveNetwork", "()Landroid/net/Network;");
    
    // Fallback to deprecated but widely supported API
    getActiveNetworkInfoMethod_ = env->GetMethodID(connectivityManagerClass_, "getActiveNetworkInfo", "()Landroid/net/NetworkInfo;");
    
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
    env->DeleteLocalRef(unityPlayerClass);
    env->DeleteLocalRef(currentActivity);
    env->DeleteLocalRef(serviceString);
    env->DeleteLocalRef(connectivityManagerObj);

    jniInitialized_ = true;
    DEBUG(LOG_TAG_JSAR, "Android JNI network monitoring initialized successfully");
    return true;
  }

  void AndroidNetworkMonitor::cleanupJNI()
  {
    if (!jniInitialized_ || javaVM_ == nullptr)
    {
      return;
    }

    JNIEnv* env = nullptr;
    jint result = javaVM_->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (result == JNI_EDETACHED)
    {
      result = javaVM_->AttachCurrentThread(&env, nullptr);
    }
    
    if (result == JNI_OK && env != nullptr)
    {
      if (contextObject_ != nullptr)
      {
        env->DeleteGlobalRef(contextObject_);
        contextObject_ = nullptr;
      }
      if (connectivityManager_ != nullptr)
      {
        env->DeleteGlobalRef(connectivityManager_);
        connectivityManager_ = nullptr;
      }
      if (contextClass_ != nullptr)
      {
        env->DeleteGlobalRef(contextClass_);
        contextClass_ = nullptr;
      }
      if (connectivityManagerClass_ != nullptr)
      {
        env->DeleteGlobalRef(connectivityManagerClass_);
        connectivityManagerClass_ = nullptr;
      }
    }

    jniInitialized_ = false;
  }

  bool AndroidNetworkMonitor::isNetworkAvailable() const
  {
    if (!jniInitialized_ || javaVM_ == nullptr || connectivityManager_ == nullptr)
    {
      return false;
    }

    JNIEnv* env = nullptr;
    jint result = javaVM_->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (result == JNI_EDETACHED)
    {
      result = javaVM_->AttachCurrentThread(&env, nullptr);
    }
    
    if (result != JNI_OK || env == nullptr)
    {
      return false;
    }

    bool isConnected = false;

    // Try modern approach first (API 23+)
    if (getActiveNetworkMethod_ != nullptr)
    {
      jobject activeNetwork = env->CallObjectMethod(connectivityManager_, getActiveNetworkMethod_);
      isConnected = (activeNetwork != nullptr);
      if (activeNetwork != nullptr)
      {
        env->DeleteLocalRef(activeNetwork);
      }
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
        {
          statusCallback_(currentStatus);
        }
        DEBUG(LOG_TAG_JSAR, "Network status changed to: %s", 
              currentStatus == NetworkStatus::Online ? "Online" : "Offline");
      }
      
      // Check every 2 seconds
      std::this_thread::sleep_for(std::chrono::seconds(2));
    }
  }

} // namespace runtime

#endif // UNITY_ANDROID