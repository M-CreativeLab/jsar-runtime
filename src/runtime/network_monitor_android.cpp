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
    
    // 3. Register a NetworkCallback with ConnectivityManager.registerDefaultNetworkCallback()
    jclass networkCallbackClass = env->FindClass("android/net/ConnectivityManager$NetworkCallback");
    if (!networkCallbackClass)
    {
      env->DeleteLocalRef(contextClass);
      env->DeleteLocalRef(connectivityServiceName);
      env->DeleteLocalRef(cmClass);
      __android_log_print(ANDROID_LOG_ERROR, "JSARRuntime", "Failed to find NetworkCallback class");
      return false;
    }

    // Create a NetworkCallback instance (simplified version)
    jmethodID callbackConstructor = env->GetMethodID(networkCallbackClass, "<init>", "()V");
    if (!callbackConstructor)
    {
      env->DeleteLocalRef(contextClass);
      env->DeleteLocalRef(connectivityServiceName);
      env->DeleteLocalRef(cmClass);
      env->DeleteLocalRef(networkCallbackClass);
      __android_log_print(ANDROID_LOG_ERROR, "JSARRuntime", "Failed to get NetworkCallback constructor");
      return false;
    }

    jobject networkCallback = env->NewObject(networkCallbackClass, callbackConstructor);
    if (!networkCallback)
    {
      env->DeleteLocalRef(contextClass);
      env->DeleteLocalRef(connectivityServiceName);
      env->DeleteLocalRef(cmClass);
      env->DeleteLocalRef(networkCallbackClass);
      __android_log_print(ANDROID_LOG_ERROR, "JSARRuntime", "Failed to create NetworkCallback instance");
      return false;
    }

    // 4. Store references for cleanup in stopMonitoring()
    networkCallback_ = env->NewGlobalRef(networkCallback);

    // Register the network callback (would call registerDefaultNetworkCallback in real implementation)
    jmethodID registerMethod = env->GetMethodID(cmClass, "registerDefaultNetworkCallback", 
                                               "(Landroid/net/ConnectivityManager$NetworkCallback;)V");
    if (registerMethod)
    {
      // Note: This would actually register with the real ConnectivityManager instance
      // For now, we just simulate the registration
      __android_log_print(ANDROID_LOG_INFO, "JSARRuntime", "NetworkCallback registration simulated");
    }

    // Clean up local references
    env->DeleteLocalRef(contextClass);
    env->DeleteLocalRef(connectivityServiceName);
    env->DeleteLocalRef(cmClass);
    env->DeleteLocalRef(networkCallbackClass);
    env->DeleteLocalRef(networkCallback);

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