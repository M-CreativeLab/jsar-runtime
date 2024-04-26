#include <stdlib.h>
#include "debug.hpp"
#include "entry.hpp"
#include "xr/device.hpp"

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

    // Run OnGraphicsDeviceEvent(initialize) manually on plugin load
    OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);

    // Create the `xr::Device` instance globally.
    xr::Device::Create();

    // Bootstrap the Node.js instance
    auto nodejsBootstrapper = NodeBootstrapper::GetOrCreateInstance();
    nodejsBootstrapper->initialize();
    nodejsBootstrapper->start();
  }

  DLL_PUBLIC void UnityPluginUnload()
  {
    s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
    xr::Device::Destroy();
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

#if defined(__ANDROID__) && (__ANDROID_API__ >= 26)
    char deviceVendor[PROP_VALUE_MAX];
    if (
      __system_property_get("ro.product.vendor.brand", deviceVendor) >= 0 ||
      __system_property_get("ro.product.product.brand", deviceVendor) >= 0
    )
    {
      setenv("JSAR_DEVICE_VENDOR", deviceVendor, 1);
    }

    char glLogKey[PROP_VALUE_MAX];
    if (__system_property_get("jsar.switch.gl.logkey", glLogKey) >= 0)
    {
      if (strcmp(glLogKey, "globals") == 0)
        s_CurrentAPI->EnableAppGlobalLog();
      else if (strcmp(glLogKey, "xrframe") == 0)
        s_CurrentAPI->EnableXRFrameLog();
      else if (strcmp(glLogKey, "all") == 0)
      {
        s_CurrentAPI->EnableAppGlobalLog();
        s_CurrentAPI->EnableXRFrameLog();
      }
    }

    char exampleUrl[PROP_VALUE_MAX];
    if (__system_property_get("jsar.example.url", exampleUrl) >= 0)
      setenv("JSAR_EXAMPLE_URL", exampleUrl, 1);

    char debugEnabled[PROP_VALUE_MAX];
    if (__system_property_get("jsar.debug.enabled", debugEnabled) >= 0)
    {
      setenv("JSAR_DEBUG_ENABLED", debugEnabled, 1);
      if (strcmp(debugEnabled, "yes") == 0)
        setenv("NODE_ENV", "dev", 1);
      else
        setenv("NODE_ENV", "prod", 1);
    }
#endif

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

  DLL_PUBLIC UnityRenderingEvent TransmuteNative_GetRenderEventFunc()
  {
    return OnUnityRenderEvent;
  }

  DLL_PUBLIC void TransmuteNative_Prepare()
  {
    DEBUG("transmute", "Prepare for TransmuteNative");
  }

  DLL_PUBLIC void TransmuteNative_InitializeXRDevice(bool enabled)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return;
    xrDevice->initialize(enabled);
  }

  DLL_PUBLIC bool TransmuteNative_GetEventFromJavaScript(int *id, int *type, uint32_t *size)
  {
    auto nativeEventTarget = messaging::UnityEventListenerWrap::GetInstance();
    if (nativeEventTarget == nullptr)
      return false;
    return nativeEventTarget->GetEvent(id, type, nullptr, size, false);
  }

  DLL_PUBLIC void TransmuteNative_GetEventDataFromJavaScript(const char *data)
  {
    auto nativeEventTarget = messaging::UnityEventListenerWrap::GetInstance();
    if (nativeEventTarget != nullptr)
      nativeEventTarget->GetEvent(nullptr, nullptr, data, nullptr, true);
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

  DLL_PUBLIC void TransmuteNative_DispatchNativeEvent(int id, int type, const char *data)
  {
    auto nativeEventTarget = messaging::UnityEventListenerWrap::GetInstance();
    if (nativeEventTarget != nullptr)
      nativeEventTarget->DispatchNativeEvent(id, type, data);
  }

  DLL_PUBLIC void TransmuteNative_SetRuntimeInit(const char *argJson)
  {
    auto nodejsBootstrapper = NodeBootstrapper::GetOrCreateInstance();
    nodejsBootstrapper->getEnv()->setRuntimeInit(argJson);
  }

  DLL_PUBLIC void TransmuteNative_SetViewport(int w, int h)
  {
    // if (s_CurrentAPI != NULL)
    //   s_CurrentAPI->SetViewport(w, h);
  }

  DLL_PUBLIC void TransmuteNative_SetFov(float fov)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return;
    xrDevice->updateFov(fov);
  }

  DLL_PUBLIC void TransmuteNative_SetTime(float t)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return;
    xrDevice->updateTime(t);
  }

  DLL_PUBLIC void TransmuteNative_SetStereoRenderingMode(int mode)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return;
    xrDevice->setStereoRenderingMode((xr::StereoRenderingMode)mode);
  }

  DLL_PUBLIC bool TransmuteNative_SetViewerTransform(float *transform)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return false;
    return xrDevice->updateViewerTransform(transform);
  }

  DLL_PUBLIC bool TransmuteNative_SetViewerTransformFromTRS(float *translation, float *rotation)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return false;

    float tx = translation[0];
    float ty = translation[1];
    float tz = translation[2];
    float rx = rotation[0];
    float ry = rotation[1];
    float rz = rotation[2];
    float rw = rotation[3];

    auto scalingMatrix = glm::scale(glm::mat4(1), glm::vec3(-1.0, 1.0, -1.0));
    auto translationMatrix = glm::translate(glm::mat4(1), glm::vec3(tx, ty, tz));
    auto rotationMatrix = glm::mat4_cast(glm::quat(rw, rx, ry, rz));
    auto base = translationMatrix * rotationMatrix * scalingMatrix;

    float m[16];
    for (int i = 0; i < 16; i++)
      m[i] = base[i / 4][i % 4];
    return xrDevice->updateViewerTransform(m);
  }

  DLL_PUBLIC bool TransmuteNative_SetViewerStereoViewMatrix(int eyeId, float *transform)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return false;
    return xrDevice->updateViewerStereoViewMatrix(eyeId, transform);
  }

  DLL_PUBLIC bool TransmuteNative_SetViewerStereoViewMatrixFromTRS(int eyeId, float *translation, float *rotation)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return false;

    float tx = translation[0];
    float ty = translation[1];
    float tz = translation[2];
    float rx = rotation[0];
    float ry = rotation[1];
    float rz = rotation[2];
    float rw = rotation[3];

    auto scalingMatrix = glm::scale(glm::mat4(1), glm::vec3(-1.0, 1.0, -1.0));
    auto translationMatrix = glm::translate(glm::mat4(1), glm::vec3(tx, ty, tz));
    auto rotationMatrix = glm::mat4_cast(glm::quat(rw, rx, ry, rz));
    auto base = translationMatrix * rotationMatrix * scalingMatrix;

    float m[16];
    for (int i = 0; i < 16; i++)
      m[i] = base[i / 4][i % 4];
    return xrDevice->updateViewerStereoViewMatrix(eyeId, m);
  }

  DLL_PUBLIC bool TransmuteNative_SetViewerStereoProjectionMatrix(int eyeId, float *transform)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return false;
    return xrDevice->updateViewerStereoProjectionMatrix(eyeId, transform);
  }

  DLL_PUBLIC bool TransmuteNative_SetLocalTransform(int id, float *transform)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return false;
    return xrDevice->updateLocalTransform(id, transform);
  }

  DLL_PUBLIC bool TransmuteNative_SetLocalTransformFromTRS(int id, float *translation, float *rotation)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return false;

    float tx = translation[0];
    float ty = translation[1];
    float tz = translation[2];
    float rx = rotation[0];
    float ry = rotation[1];
    float rz = rotation[2];
    float rw = rotation[3];

    auto scalingMatrix = glm::scale(glm::mat4(1), glm::vec3(1, 1, -1));
    auto translationMatrix = glm::translate(glm::mat4(1), glm::vec3(tx, ty, tz));
    auto rotationMatrix = glm::mat4_cast(glm::quat(rw, rx, ry, rz));
    auto base = translationMatrix * rotationMatrix * scalingMatrix;

    float m[16];
    for (int i = 0; i < 16; i++)
      m[i] = base[i / 4][i % 4];
    return xrDevice->updateLocalTransform(id, m);
  }
#endif
}
