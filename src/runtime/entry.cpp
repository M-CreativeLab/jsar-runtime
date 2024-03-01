#include "debug.hpp"
#include "entry.hpp"

#if defined(__ANDROID__) && (__ANDROID_API__ >= 26)
#include <sys/system_properties.h>
#endif

using namespace std;

extern "C"
{
#ifndef TRANSMUTE_STANDALONE
  static IUnityInterfaces *s_UnityInterfaces = NULL;
  static IUnityGraphics *s_Graphics = NULL;
  static void OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType);

  DLL_PUBLIC void UnityPluginLoad(IUnityInterfaces *unityInterfaces)
  {
    s_UnityInterfaces = unityInterfaces;
    SET_UNITY_LOG_HANDLE(s_UnityInterfaces->Get<IUnityLog>());

    s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
    s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);
    DEBUG("transmute", "UnityPluginLoad");

    // Run OnGraphicsDeviceEvent(initialize) manually on plugin load
    OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);

    // Bootstrap the Node.js instance
    auto nodejsBootstrapper = NodeBootstrapper::GetOrCreateInstance();
    nodejsBootstrapper->initialize();
    nodejsBootstrapper->start();
  }

  DLL_PUBLIC void UnityPluginUnload()
  {
    s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
  }

  static RenderAPI *s_CurrentAPI = NULL;
  static UnityGfxRenderer s_DeviceType = kUnityGfxRendererNull;
  static void OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
  {
    // Create graphics API implementation upon initialization
    if (eventType == kUnityGfxDeviceEventInitialize)
    {
      s_DeviceType = s_Graphics->GetRenderer();
      s_CurrentAPI = RenderAPI::Create(s_DeviceType);
    }

    // Let the implementation process the device related events
    if (s_CurrentAPI)
      s_CurrentAPI->ProcessDeviceEvent(eventType, s_UnityInterfaces);

    // Cleanup graphics API implementation upon shutdown
    if (eventType == kUnityGfxDeviceEventShutdown)
    {
      delete s_CurrentAPI;
      s_CurrentAPI = NULL;
      s_DeviceType = kUnityGfxRendererNull;
    }
  }

  static void OnUnityRenderEvent(int eventID)
  {
    if (s_CurrentAPI == NULL || !NodeBootstrapper::IsInstanceRunning())
      return;

    auto code = s_CurrentAPI->ExecuteFrame();
    switch (code)
    {
    case kFrameExecutionNotAvailable:
      DEBUG("transmute", "RenderLoop(JS) is not available");
      break;
    default:
      break;
    }
  }

  DLL_PUBLIC void TransmuteNative_Prepare()
  {
    DEBUG("transmute", "Prepare for TransmuteNative");
  }

  DLL_PUBLIC bool TransmuteNative_IsRuntimeUp()
  {
    auto nodejsBootstrapper = NodeBootstrapper::GetOrCreateInstance();
    if (!nodejsBootstrapper->isRunning())
      return false;

    // TODO: Check if the runtime is up
    return true;
  }

  DLL_PUBLIC bool TransmuteNative_IsRuntimeAvailable()
  {
    auto nodejsBootstrapper = NodeBootstrapper::GetOrCreateInstance();
    return nodejsBootstrapper->isRuntimeAvailable();
  }

  DLL_PUBLIC void TransmuteNative_DispatchRuntimeEvent(int id)
  {
    // TODO
  }

  DLL_PUBLIC void TransmuteNative_SetRuntimeInit(const char *argJson)
  {
    auto nodejsBootstrapper = NodeBootstrapper::GetOrCreateInstance();
    nodejsBootstrapper->getEnv()->setRuntimeInit(argJson);
  }

  DLL_PUBLIC void TransmuteNative_SetViewport(int w, int h)
  {
    if (s_CurrentAPI != NULL)
      s_CurrentAPI->SetViewport(w, h);
  }

  DLL_PUBLIC void TransmuteNative_SetCameraInit(float fov, float near, float far)
  {
    // TODO
  }

  DLL_PUBLIC void TransmuteNative_SetCameraPose(float x, float y, float z, float qx, float qy, float qz, float qw)
  {
    // TODO
  }

  DLL_PUBLIC void TransmuteNative_SetContainerPose(int uuid, float x, float y, float z, float qx, float qy, float qz, float qw)
  {
    // TODO
  }

  DLL_PUBLIC void TransmuteNative_SetTime(float t)
  {
    if (s_CurrentAPI != NULL)
      s_CurrentAPI->SetTime(t);
  }

  DLL_PUBLIC UnityRenderingEvent TransmuteNative_GetRenderEventFunc()
  {
    return OnUnityRenderEvent;
  }
#endif
}
