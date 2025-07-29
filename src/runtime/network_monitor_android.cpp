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

    // Get the JNI environment
    JNIEnv* env = nullptr;
    JavaVM* jvm = nullptr;
    
    // Note: In a real implementation, you would get the JVM from the Android runtime
    // For now, this is a placeholder that shows the intended structure
    if (!jvm || jvm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK)
    {
      __android_log_print(ANDROID_LOG_ERROR, "JSARRuntime", "Failed to get JNI environment");
      return false;
    }

    // Get the ConnectivityManager system service
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
    env->DeleteLocalRef(contextClass);

    // Note: In a real implementation, you would:
    // 1. Get the Android Context from the Unity activity
    // 2. Call context.getSystemService(Context.CONNECTIVITY_SERVICE)
    // 3. Register a NetworkCallback with ConnectivityManager.registerDefaultNetworkCallback()
    // 4. Store references for cleanup in stopMonitoring()

    isMonitoring_ = true;

    // Initial status check
    if (statusCallback_)
    {
      statusCallback_(getCurrentStatus());
    }

    __android_log_print(ANDROID_LOG_INFO, "JSARRuntime", "Network monitoring started");
    return true;
  }

  void AndroidNetworkMonitor::stopMonitoring()
  {
    if (!isMonitoring_)
    {
      return;
    }

    // Unregister NetworkCallback
    if (connectivityManager_ && networkCallback_)
    {
      JNIEnv* env = nullptr;
      JavaVM* jvm = nullptr;
      
      // Note: In a real implementation, you would get the JVM from the Android runtime
      if (jvm && jvm->GetEnv((void**)&env, JNI_VERSION_1_6) == JNI_OK)
      {
        // Get ConnectivityManager class
        jclass cmClass = env->FindClass("android/net/ConnectivityManager");
        if (cmClass)
        {
          jmethodID unregisterMethod = env->GetMethodID(cmClass, "unregisterNetworkCallback", 
                                                       "(Landroid/net/ConnectivityManager$NetworkCallback;)V");
          if (unregisterMethod)
          {
            env->CallVoidMethod((jobject)connectivityManager_, unregisterMethod, (jobject)networkCallback_);
          }
          env->DeleteLocalRef(cmClass);
        }
        
        // Clean up global references
        env->DeleteGlobalRef((jobject)connectivityManager_);
        env->DeleteGlobalRef((jobject)networkCallback_);
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

    JNIEnv* env = nullptr;
    JavaVM* jvm = nullptr;
    
    // Note: In a real implementation, you would get the JVM from the Android runtime
    if (!jvm || jvm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK)
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