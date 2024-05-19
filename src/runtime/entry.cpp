#include <stdlib.h>
#include "entry.hpp"
#include "debug.hpp"
#include "embedder.hpp"
#include "native_event.hpp"
#include "xr/device.hpp"

#if defined(__ANDROID__) && (__ANDROID_API__ >= 26)
#include <sys/system_properties.h>
#endif

using namespace std;

/**
 * The embedder class for Unity, it depends on the Unity native interfaces to embed the Transmute runtime into Unity.
 */
class UnityEmbedder : public TrEmbedder
{
public:
  static UnityEmbedder *Create(IUnityInterfaces *unityInterfaces);
  static UnityEmbedder *EnsureAndGet();

private:
  static void OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
  {
    auto embedder = UnityEmbedder::EnsureAndGet();
    if (eventType == kUnityGfxDeviceEventInitialize)
      embedder->initialize();
    else if (eventType == kUnityGfxDeviceEventShutdown)
      embedder->shutdown();
  }

private:
  UnityEmbedder(IUnityInterfaces *unityInterfaces) : TrEmbedder(),
                                                     interfaces(unityInterfaces)
  {
    graphics = unityInterfaces->Get<IUnityGraphics>();
    log = unityInterfaces->Get<IUnityLog>();

    graphics->RegisterDeviceEventCallback(UnityEmbedder::OnGraphicsDeviceEvent);
    UnityEmbedder::OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
  }

public:
  void initialize()
  {
    deviceType = graphics->GetRenderer();
    TrEmbedder::initialize();
  }

  void unload()
  {
    graphics->UnregisterDeviceEventCallback(UnityEmbedder::OnGraphicsDeviceEvent);
    // TODO: stop xr device and renderer?
  }

  void shutdown()
  {
    TrEmbedder::shutdown();
    deviceType = kUnityGfxRendererNull;
    interfaces = nullptr;
    graphics = nullptr;
    log = nullptr;
  }

private:
  UnityGfxRenderer deviceType = kUnityGfxRendererNull;
  IUnityInterfaces *interfaces = nullptr;
  IUnityGraphics *graphics = nullptr;
  IUnityLog *log = nullptr;

private:
  static UnityEmbedder *s_EmbedderInstance;
};

UnityEmbedder *UnityEmbedder::s_EmbedderInstance = nullptr;
UnityEmbedder *UnityEmbedder::Create(IUnityInterfaces *unityInterfaces)
{
  if (s_EmbedderInstance == nullptr)
    s_EmbedderInstance = new UnityEmbedder(unityInterfaces);
  return s_EmbedderInstance;
}
UnityEmbedder *UnityEmbedder::EnsureAndGet()
{
  assert(s_EmbedderInstance != nullptr);
  return s_EmbedderInstance;
}

extern "C"
{
  static float s_WorldScalingFactor = 1.0;
  static void OnPlatformSetup()
  {
    auto embedder = UnityEmbedder::EnsureAndGet();

#if defined(__ANDROID__) && (__ANDROID_API__ >= 26)
    char deviceVendor[PROP_VALUE_MAX];
    if (
        __system_property_get("ro.product.vendor.brand", deviceVendor) >= 0 ||
        __system_property_get("ro.product.product.brand", deviceVendor) >= 0)
    {
      setenv("JSAR_DEVICE_VENDOR", deviceVendor, 1);
    }

    char logfilter[PROP_VALUE_MAX];
    if (__system_property_get("jsar.renderer.logfilter", logfilter) >= 0)
      embedder->getRenderer()->setLogFilter(logfilter);

    char enableWebglPlaceholder[PROP_VALUE_MAX];
    if (
        /**
         * When the property is set and the value is not "yes", we will disable the placeholder feature.
         */
        __system_property_get("jsar.webgl.placeholder", enableWebglPlaceholder) > 0 &&
        strcmp(enableWebglPlaceholder, "yes") != 0)
    {
      setenv("JSAR_WEBGL_PLACEHOLDER", "no", 1);
    }
    else
    {
      // Enable the placeholder feature by default
      setenv("JSAR_WEBGL_PLACEHOLDER", "yes", 1);
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

    char xrFrameRate[PROP_VALUE_MAX];
    if (__system_property_get("jsar.xr.framerate", xrFrameRate) >= 0)
    {
      int frameRate = atoi(xrFrameRate);
      embedder->getXrDevice()->setFrameRate(frameRate);
    }
#endif
  }

#ifndef TRANSMUTE_STANDALONE
  DLL_PUBLIC void UnityPluginLoad(IUnityInterfaces *unityInterfaces)
  {
    auto embedder = UnityEmbedder::Create(unityInterfaces);
    if (embedder == nullptr)
    {
      DEBUG(LOG_TAG_UNITY, "Failed to create UnityEmbedder instance");
      return;
    }

    OnPlatformSetup();
    embedder->initialize();
  }

  DLL_PUBLIC void UnityPluginUnload()
  {
    UnityEmbedder::EnsureAndGet()->unload();
  }

  static void OnUnityRenderEvent(int eventID)
  {
    TransmuteNative_OnRenderFrame();
  }

  DLL_PUBLIC UnityRenderingEvent TransmuteNative_GetRenderEventFunc()
  {
    return OnUnityRenderEvent;
  }

#endif

  DLL_PUBLIC void TransmuteNative_Start()
  {
    // TODO: Bootstrap?
  }

  DLL_PUBLIC void TransmuteNative_Prepare()
  {
    DEBUG("transmute", "Prepare for TransmuteNative");
  }

  DLL_PUBLIC void TransmuteNative_InitializeXRDevice(bool enabled)
  {
    UnityEmbedder::EnsureAndGet()->configureXrDevice(enabled);
  }

  DLL_PUBLIC bool TransmuteNative_GetEventFromJavaScript(int *id, int *type, uint32_t *size)
  {
    // auto nativeEventTarget = messaging::UnityEventListenerWrap::GetInstance();
    // if (nativeEventTarget == nullptr)
    //   return false;
    // return nativeEventTarget->GetEvent(id, type, nullptr, size, false);
    return false;
  }

  DLL_PUBLIC void TransmuteNative_GetEventDataFromJavaScript(const char *data)
  {
    // auto nativeEventTarget = messaging::UnityEventListenerWrap::GetInstance();
    // if (nativeEventTarget != nullptr)
    //   nativeEventTarget->GetEvent(nullptr, nullptr, data, nullptr, true);
  }

  DLL_PUBLIC bool TransmuteNative_IsRuntimeUp()
  {
    // auto nodejsBootstrapper = NodeBootstrapper::GetOrCreateInstance();
    // if (!nodejsBootstrapper->isRunning())
    //   return false;

    // TODO: Check if the runtime is up
    return true;
  }

  DLL_PUBLIC bool TransmuteNative_IsRuntimeAvailable()
  {
    // auto nodejsBootstrapper = NodeBootstrapper::GetOrCreateInstance();
    // return nodejsBootstrapper->isRuntimeAvailable();
    return true;
  }

  DLL_PUBLIC void TransmuteNative_OnRenderFrame()
  {
    UnityEmbedder::EnsureAndGet()->onFrame();
    // if (s_CurrentAPI == NULL)
    //   return;
    // auto constellation = TrConstellation::Get();
    // if (constellation == nullptr || !constellation->isInitialized())
    //   return;
    // constellation->tick();

    // auto code = s_CurrentAPI->ExecuteFrame();
    // switch (code)
    // {
    // case kFrameExecutionNotAvailable:
    //   DEBUG("transmute", "RenderLoop(JS) is not available");
    //   break;
    // default:
    //   break;
    // }
  }

  DLL_PUBLIC void TransmuteNative_DispatchRuntimeEvent(int id)
  {
    // TODO
  }

  DLL_PUBLIC void TransmuteNative_DispatchNativeEvent(int id, int type, const char *data)
  {
    // auto nativeEventTarget = messaging::UnityEventListenerWrap::GetInstance();
    // if (nativeEventTarget != nullptr)
    //   nativeEventTarget->DispatchNativeEvent(id, type, data);
    auto eventTarget = UnityEmbedder::EnsureAndGet()->getNativeEventTarget();
    eventTarget->dispatchEvent(id, (native_event::TrEventType)type, data);
  }

  DLL_PUBLIC void TransmuteNative_SetRuntimeInit(const char *argJson)
  {
    // auto nodejsBootstrapper = NodeBootstrapper::GetOrCreateInstance();
    // nodejsBootstrapper->getEnv()->setRuntimeInit(argJson);
    UnityEmbedder::EnsureAndGet()->onStart(string(argJson));
  }

  DLL_PUBLIC void TransmuteNative_SetViewport(int w, int h)
  {
    TrViewport viewport(w, h);
    UnityEmbedder::EnsureAndGet()->getRenderer()->setViewport(viewport);
  }

  DLL_PUBLIC void TransmuteNative_SetFov(float fov)
  {
    UnityEmbedder::EnsureAndGet()->getRenderer()->setRecommendedFov(fov);
  }

  DLL_PUBLIC void TransmuteNative_SetTime(float t)
  {
    UnityEmbedder::EnsureAndGet()->getRenderer()->setTime(t);
  }

  DLL_PUBLIC void TransmuteNative_SetStereoRenderingMode(int mode)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return;
    xrDevice->setStereoRenderingMode((xr::StereoRenderingMode)mode);
  }

  DLL_PUBLIC bool TransmuteNative_SetViewerStereoProjectionMatrix(int eyeId, float *transform)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return false;
    return xrDevice->updateViewerStereoProjectionMatrix(eyeId, transform);
  }

  DLL_PUBLIC bool TransmuteNative_SetViewerTransformFromTRS(float *translation, float *rotation)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return false;

    float tx = translation[0] * s_WorldScalingFactor;
    float ty = translation[1] * s_WorldScalingFactor;
    float tz = translation[2] * s_WorldScalingFactor;
    float rx = rotation[0];
    float ry = rotation[1];
    float rz = rotation[2];
    float rw = rotation[3];

    auto scalingMatrix = glm::scale(glm::mat4(1), glm::vec3(1.0, 1.0, 1.0));
    auto translationMatrix = glm::translate(glm::mat4(1), glm::vec3(tx, ty, tz));
    auto rotationMatrix = glm::mat4_cast(glm::quat(rw, rx, ry, rz));
    auto base = translationMatrix * rotationMatrix * scalingMatrix;

    float m[16];
    for (int i = 0; i < 16; i++)
      m[i] = base[i / 4][i % 4];
    return xrDevice->updateViewerTransform(m);
  }

  DLL_PUBLIC bool TransmuteNative_SetViewerStereoViewMatrixFromTRS(int eyeId, float *translation, float *rotation)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return false;

    float tx = translation[0] * s_WorldScalingFactor;
    float ty = translation[1] * s_WorldScalingFactor;
    float tz = translation[2] * s_WorldScalingFactor;
    float rx = rotation[0];
    float ry = rotation[1];
    float rz = rotation[2];
    float rw = rotation[3];

    auto S = glm::scale(glm::mat4(1), glm::vec3(-1, 1, -1));
    auto T = glm::translate(glm::mat4(1), glm::vec3(tx, ty, tz));
    auto R = glm::mat4_cast(glm::quat(rw, rx, ry, rz));
    auto base = T * R * S;

    float m[16];
    for (int i = 0; i < 16; i++)
      m[i] = base[i / 4][i % 4];
    return xrDevice->updateViewerStereoViewMatrix(eyeId, m);
  }

  DLL_PUBLIC bool TransmuteNative_SetLocalTransformFromTRS(int id, float *translation, float *rotation)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return false;

    float tx = translation[0] * s_WorldScalingFactor;
    float ty = translation[1] * s_WorldScalingFactor;
    float tz = translation[2] * s_WorldScalingFactor;
    float rx = rotation[0];
    float ry = rotation[1];
    float rz = rotation[2];
    float rw = rotation[3];

    auto S = glm::scale(glm::mat4(1), glm::vec3(1, 1, 1));
    auto T = glm::translate(glm::mat4(1), glm::vec3(tx, ty, tz));
    auto R = glm::mat4_cast(glm::quat(rw, rx, ry, rz));
    auto base = T * R * S;

    float m[16];
    for (int i = 0; i < 16; i++)
      m[i] = base[i / 4][i % 4];
    return xrDevice->updateLocalTransform(id, m);
  }

  DLL_PUBLIC void TransmuteNative_SetHandInputPose(int handness, int joint, float *translation, float *rotation, float radius)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return;

    xr::Handness id = handness == 0 ? xr::Handness::Left : xr::Handness::Right;
    auto hand = xrDevice->getHandInputSource(id);
    if (hand == nullptr)
      return;
    if (joint < 0 || joint >= 25)
      return; // out of range

    float defaultScale[3] = {1, 1, 1};
    hand->joints[joint].baseMatrix = math::makeMatrixFromTRS(translation, rotation, defaultScale, s_WorldScalingFactor);
  }

  DLL_PUBLIC void TransmuteNative_SetHandInputRayPose(int handness, float *translation, float *rotation)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return;

    xr::Handness id = handness == 0 ? xr::Handness::Left : xr::Handness::Right;
    auto hand = xrDevice->getHandInputSource(id);
    if (hand == nullptr)
      return;
    hand->targetRayBaseMatrix = math::makeMatrixFromTRS(translation, rotation, new float[3]{1, 1, 1}, s_WorldScalingFactor);
  }

  DLL_PUBLIC void TransmuteNative_SetHandInputGripPose(int handness, float *translation, float *rotation)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return;

    xr::Handness id = handness == 0 ? xr::Handness::Left : xr::Handness::Right;
    auto hand = xrDevice->getHandInputSource(id);
    if (hand == nullptr)
      return;
    hand->gripBaseMatrix = math::makeMatrixFromTRS(translation, rotation, new float[3]{1, 1, 1}, s_WorldScalingFactor);
  }

  DLL_PUBLIC void TransmuteNative_SetHandInputActionState(int handness, int actionType, int state)
  {
    auto xrDevice = xr::Device::GetInstance();
    if (xrDevice == NULL)
      return;

    xr::Handness id = handness == 0 ? xr::Handness::Left : xr::Handness::Right;
    auto hand = xrDevice->getHandInputSource(id);
    if (hand == nullptr)
      return;

    if (actionType == xr::InputSourceActionType::XRPrimaryAction)
      hand->primaryActionPressed = state == 0; /** check if pressed */
    else if (actionType == xr::InputSourceActionType::XRSqueezeAction)
      hand->squeezeActionPressed = state == 0; /** check if pressed */
  }
}
