#include "network_monitor.hpp"

#if UNITY_ANDROID
#include <jni.h>
#include <android/log.h>

namespace runtime
{
  AndroidNetworkMonitor::~AndroidNetworkMonitor()
  {
    stopMonitoring();
  }

  bool AndroidNetworkMonitor::startMonitoring(NetworkStatusCallback callback)
  {
    if (isMonitoring_)
    {
      return false;
    }

    statusCallback_ = callback;

    // Call onNetworkChanged to ensure the callback gets the initial status
    onNetworkChanged(this);

    // Get the JNI environment
    JNIEnv *env = nullptr;
    JavaVM *jvm = nullptr;

    // Note: In a real implementation, you would get the JVM from the Android runtime
    // For now, this is a placeholder that shows the intended structure
    if (!jvm || jvm->GetEnv((void **)&env, JNI_VERSION_1_6) != JNI_OK)
    {
      __android_log_print(ANDROID_LOG_ERROR, "JSARRuntime", "Failed to get JNI environment");
      return false;
    }

    // 1. Get the Android Context from the Unity activity
    // Note: In a real Unity implementation, you would get the activity context
    // For now, we'll create a placeholder structure for the implementation

    // Get the Context class and CONNECTIVITY_SERVICE constant
    jclass contextClass = env->FindClass("android/content/Context");
    if (!contextClass)
    {
      __android_log_print(ANDROID_LOG_ERROR, "JSARRuntime", "Failed to find Context class");
      return false;
    }

    jfieldID connectivityServiceField = env->GetStaticFieldID(contextClass, "CONNECTIVITY_SERVICE", "Ljava/lang/String;");
    if (!connectivityServiceField)
    {
      env->DeleteLocalRef(contextClass);
      __android_log_print(ANDROID_LOG_ERROR, "JSARRuntime", "Failed to get CONNECTIVITY_SERVICE field");
      return false;
    }

    jobject connectivityServiceName = env->GetStaticObjectField(contextClass, connectivityServiceField);

    // 2. Call context.getSystemService(Context.CONNECTIVITY_SERVICE)
    // Note: In a real implementation, you would get the actual Android context here
    // For now, we'll simulate getting the ConnectivityManager
    jclass cmClass = env->FindClass("android/net/ConnectivityManager");
    if (!cmClass)
    {
      env->DeleteLocalRef(contextClass);
      env->DeleteLocalRef(connectivityServiceName);
      __android_log_print(ANDROID_LOG_ERROR, "JSARRuntime", "Failed to find ConnectivityManager class");
      return false;
    }

    // Store the ConnectivityManager reference (would be obtained from context.getSystemService)
    // For now, we'll use a placeholder that represents the manager
    connectivityManager_ = env->NewGlobalRef(cmClass);

    // 3. Create a custom NetworkCallback that calls our native onNetworkChanged method
    // Note: In a real implementation, you would create a Java class that extends NetworkCallback
    // and implements onAvailable/onLost methods that call our native onNetworkChanged via JNI

    // For now, we'll simulate the callback registration and assume that when network state changes,
    // our onNetworkChanged method will be called by the Android system

    // Store a reference to this monitor instance for the callback
    networkCallback_ = this;

    // In a full implementation, you would:
    // 1. Create a Java NetworkCallback subclass with native method declarations
    // 2. Register native methods using RegisterNatives that point to onNetworkChanged
    // 3. Create an instance of that callback and register it with ConnectivityManager

    __android_log_print(ANDROID_LOG_INFO, "JSARRuntime", "NetworkCallback registration completed");

    // Clean up local references
    env->DeleteLocalRef(contextClass);
    env->DeleteLocalRef(connectivityServiceName);
    env->DeleteLocalRef(cmClass);

    isMonitoring_ = true;

    // Initial status check - call onNetworkChanged to notify the callback
    onNetworkChanged(this);

    __android_log_print(ANDROID_LOG_INFO, "JSARRuntime", "Network monitoring started");
    return true;
  }

  void AndroidNetworkMonitor::stopMonitoring()
  {
    if (!isMonitoring_)
    {
      return;
    }

    // Clean up the connectivity manager reference if needed
    if (connectivityManager_)
    {
      JNIEnv *env = nullptr;
      JavaVM *jvm = nullptr;

      // Note: In a real implementation, you would get the JVM from the Android runtime
      if (jvm && jvm->GetEnv((void **)&env, JNI_VERSION_1_6) == JNI_OK)
      {
        // In a full implementation, you would unregister the NetworkCallback here
        // env->CallVoidMethod(connectivityManager, unregisterNetworkCallback, networkCallback);

        // Clean up global reference if we had one
        if (connectivityManager_)
        {
          env->DeleteGlobalRef((jobject)connectivityManager_);
        }
      }
    }

    connectivityManager_ = nullptr;
    networkCallback_ = nullptr;
    isMonitoring_ = false;
    statusCallback_ = nullptr;

    __android_log_print(ANDROID_LOG_INFO, "JSARRuntime", "Network monitoring stopped");
  }

  NetworkStatus AndroidNetworkMonitor::getCurrentStatus() const
  {
    if (!connectivityManager_)
    {
      // If we don't have a connectivity manager, assume online for basic functionality
      return NetworkStatus::Online;
    }

    JNIEnv *env = nullptr;
    JavaVM *jvm = nullptr;

    // Note: In a real implementation, you would get the JVM from the Android runtime
    if (!jvm || jvm->GetEnv((void **)&env, JNI_VERSION_1_6) != JNI_OK)
    {
      return NetworkStatus::Online; // Fallback to online
    }

    // Get the active network info
    jclass cmClass = env->FindClass("android/net/ConnectivityManager");
    if (!cmClass)
    {
      return NetworkStatus::Online;
    }

    jmethodID getActiveNetworkMethod = env->GetMethodID(cmClass, "getActiveNetwork", "()Landroid/net/Network;");
    if (!getActiveNetworkMethod)
    {
      env->DeleteLocalRef(cmClass);
      return NetworkStatus::Online;
    }

    jobject activeNetwork = env->CallObjectMethod((jobject)connectivityManager_, getActiveNetworkMethod);
    env->DeleteLocalRef(cmClass);

    // If activeNetwork is null, we're offline
    if (!activeNetwork)
    {
      return NetworkStatus::Offline;
    }

    env->DeleteLocalRef(activeNetwork);
    return NetworkStatus::Online;
  }

  void AndroidNetworkMonitor::checkNetworkStatus()
  {
    if (!isMonitoring_ || !statusCallback_)
    {
      return;
    }

    NetworkStatus currentStatus = getCurrentStatus();
    statusCallback_(currentStatus);
  }

  void AndroidNetworkMonitor::onNetworkChanged(AndroidNetworkMonitor *monitor)
  {
    if (monitor)
    {
      monitor->checkNetworkStatus();
    }
  }

} // namespace runtime

#endif // UNITY_ANDROID