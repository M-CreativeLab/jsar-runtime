#include <stdlib.h>
#include "unity_entry.hpp"
#include "debug.hpp"
#include "embedder.hpp"
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
  UnityEmbedder(IUnityInterfaces *unityInterfaces) : TrEmbedder(TrHostEngine::Unity),
                                                     interfaces(unityInterfaces)
  {
    graphics = unityInterfaces->Get<IUnityGraphics>();
    log = unityInterfaces->Get<IUnityLog>();
    SET_UNITY_LOG_HANDLE(log);

    graphics->RegisterDeviceEventCallback(UnityEmbedder::OnGraphicsDeviceEvent);
  }

public:
  void initialize()
  {
    assert(constellation != nullptr);

    deviceType = graphics->GetRenderer();
    // set the backend api to the renderer.
    auto api = RenderAPI::Create(deviceType, constellation.get());
    constellation->renderer->setApi(api);
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

  bool onStart(string argJson)
  {
    auto result = TrEmbedder::onStart(argJson);
    if (result)
    {
#if defined(__ANDROID__) && (__ANDROID_API__ >= 26)
      auto opts = constellation->getOptions();
      __system_property_set("jsar.init.cache_directory", opts.applicationCacheDirectory.c_str());
#endif
    }
    return result;
  }

  bool onEvent(events_comm::TrNativeEvent &event, TrContentRuntime *content) override
  {
    if (event.type == events_comm::TrNativeEventType::DocumentEvent)
    {
      auto documentEvent = event.detail<events_comm::TrDocumentEvent>();
      DEBUG(LOG_TAG_METRICS, "#%d Received %s", documentEvent.documentId, documentEvent.toString().c_str());
    }
    pendingEvents.push_back(make_shared<events_comm::TrNativeEvent>(event));
    return true;
  }

  bool getEventHeader(int *id, int *type, uint32_t *size)
  {
    if (pendingEvents.empty())
      return false;
    auto &event = pendingEvents.front();
    *id = event->id;
    *type = static_cast<int>(event->type);
    *size = event->getDetailByteLength();
    return true;
  }

  void getEventData(const char *outData)
  {
    auto first = pendingEvents.begin();
    auto &event = *first;
    auto &eventData = event->getDetailJson();
    memcpy((void *)outData, eventData.c_str(), eventData.size());

    // remove the event from the pending list
    pendingEvents.erase(first);
  }

private:
  UnityGfxRenderer deviceType = kUnityGfxRendererNull;
  IUnityInterfaces *interfaces = nullptr;
  IUnityGraphics *graphics = nullptr;
  IUnityLog *log = nullptr;
  vector<shared_ptr<events_comm::TrNativeEvent>> pendingEvents;

private:
  static UnityEmbedder *s_EmbedderInstance;
};

UnityEmbedder *UnityEmbedder::s_EmbedderInstance = nullptr;
UnityEmbedder *UnityEmbedder::Create(IUnityInterfaces *unityInterfaces)
{
  if (s_EmbedderInstance == nullptr)
  {
    s_EmbedderInstance = new UnityEmbedder(unityInterfaces);
    UnityEmbedder::OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
  }
  return s_EmbedderInstance;
}
UnityEmbedder *UnityEmbedder::EnsureAndGet()
{
  assert(s_EmbedderInstance != nullptr);
  return s_EmbedderInstance;
}

#define TR_ENSURE_COMPONENT(name) UnityEmbedder::EnsureAndGet()->constellation->name

extern "C"
{
  static float s_WorldScalingFactor = 1.0;
  static void OnPlatformSetup(UnityEmbedder *embedder)
  {
    auto renderer = embedder->constellation->renderer;

#if defined(__ANDROID__) && (__ANDROID_API__ >= 26)
    char deviceVendor[PROP_VALUE_MAX];
    if (
        __system_property_get("ro.product.vendor.brand", deviceVendor) >= 0 ||
        __system_property_get("ro.product.product.brand", deviceVendor) >= 0)
    {
      setenv("JSAR_DEVICE_VENDOR", deviceVendor, 1);
    }

    char enableWebglPlaceholders[PROP_VALUE_MAX];
    if (
        /**
         * When the property is set and the value is not "yes", we will disable the placeholder feature.
         */
        __system_property_get("jsar.webgl.placeholders", enableWebglPlaceholders) > 0 &&
        strcmp(enableWebglPlaceholders, "yes") != 0)
    {
      setenv("JSAR_WEBGL_PLACEHOLDERS", "no", 1);
    }
    else
    {
      // Enable the placeholder feature by default
      setenv("JSAR_WEBGL_PLACEHOLDERS", "yes", 1);
    }

    char targetClientFps[PROP_VALUE_MAX];
    if (__system_property_get("jsar.renderer.target_app_fps", targetClientFps) >= 0)
    {
      renderer->configureClientFrameRate(atoi(targetClientFps));
    }

    char debugEnabledStr[PROP_VALUE_MAX];
    if (__system_property_get("jsar.debug.enabled", debugEnabledStr) >= 0)
    {
      bool isDebugEnabled = strcmp(debugEnabledStr, "yes") == 0;
      setenv("JSAR_DEBUG_ENABLED", debugEnabledStr, 1);
      setenv("NODE_ENV", isDebugEnabled ? "dev" : "prod", 1);

      if (isDebugEnabled)
      {
        char exampleUrl[PROP_VALUE_MAX];
        if (__system_property_get("jsar.example.url", exampleUrl) >= 0)
          setenv("JSAR_EXAMPLE_URL", exampleUrl, 1);
        char enableResourcesCachingStr[PROP_VALUE_MAX];
        if (__system_property_get("jsar.resources.caching", enableResourcesCachingStr) >= 0)
          setenv("JSAR_RESOURCES_CACHING", enableResourcesCachingStr, 1);

        char enableRendererTracingStr[PROP_VALUE_MAX];
        if (
            __system_property_get("jsar.renderer.tracing", enableRendererTracingStr) >= 0 &&
            strcmp(enableRendererTracingStr, "yes") == 0)
          renderer->enableTracing();

        char logfilter[PROP_VALUE_MAX];
        if (__system_property_get("jsar.renderer.logfilter", logfilter) >= 0)
          renderer->setLogFilter(logfilter);
      }
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
    OnPlatformSetup(embedder);
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

  DLL_PUBLIC bool TransmuteNative_Start(const char *argJson)
  {
    return UnityEmbedder::EnsureAndGet()->onStart(string(argJson));
  }

  DLL_PUBLIC bool TransmuteNative_ConfigureXRDevice(bool enabled, bool isDeviceActive, int stereoRenderingMode)
  {
    xr::TrDeviceInit init;
    init.active = isDeviceActive;
    init.stereoRenderingMode = (xr::TrStereoRenderingMode)stereoRenderingMode;
    return UnityEmbedder::EnsureAndGet()->configureXrDevice(enabled, init);
  }

  DLL_PUBLIC bool TransmuteNative_GetEventFromJavaScript(int *id, int *type, uint32_t *size)
  {
    return UnityEmbedder::EnsureAndGet()->getEventHeader(id, type, size);
  }

  DLL_PUBLIC void TransmuteNative_GetEventDataFromJavaScript(const char *data)
  {
    return UnityEmbedder::EnsureAndGet()->getEventData(data);
  }

  DLL_PUBLIC void TransmuteNative_OnRenderFrame()
  {
    UnityEmbedder::EnsureAndGet()->onFrame();
  }

  DLL_PUBLIC void TransmuteNative_DispatchRuntimeEvent(int id)
  {
    // TODO
  }

  DLL_PUBLIC void TransmuteNative_DispatchNativeEvent(int type, const char *data)
  {
    TR_ENSURE_COMPONENT(nativeEventTarget)->dispatchEvent(static_cast<events_comm::TrNativeEventType>(type), data);
  }

  DLL_PUBLIC void TransmuteNative_SetViewport(int w, int h)
  {
    TrViewport viewport(w, h);
    TR_ENSURE_COMPONENT(renderer)->setDrawingViewport(viewport);
  }

  DLL_PUBLIC void TransmuteNative_SetFov(float fov)
  {
    TR_ENSURE_COMPONENT(renderer)->setRecommendedFov(fov);
  }

  DLL_PUBLIC void TransmuteNative_SetTime(float t)
  {
    TR_ENSURE_COMPONENT(renderer)->setTime(t);
  }

  DLL_PUBLIC bool TransmuteNative_SetViewerStereoProjectionMatrix(int eyeId, float *transform)
  {
    auto xrDevice = TR_ENSURE_COMPONENT(xrDevice);
    if (xrDevice == NULL)
      return false;
    return xrDevice->updateProjectionMatrix(eyeId, transform);
  }

  DLL_PUBLIC bool TransmuteNative_SetViewerTransformFromTRS(float *translation, float *rotation)
  {
    auto xrDevice = TR_ENSURE_COMPONENT(xrDevice);
    if (xrDevice == NULL)
      return false;

    float tx = translation[0];
    float ty = translation[1];
    float tz = translation[2];
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
    return xrDevice->updateViewerBaseMatrix(m);
  }

  DLL_PUBLIC bool TransmuteNative_SetViewerStereoViewMatrixFromTRS(int eyeId, float *translation, float *rotation)
  {
    auto xrDevice = TR_ENSURE_COMPONENT(xrDevice);
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
    return xrDevice->updateViewMatrix(eyeId, m);
  }

  DLL_PUBLIC bool TransmuteNative_SetLocalTransformFromTRS(int id, float *translation, float *rotation)
  {
    auto xrDevice = TR_ENSURE_COMPONENT(xrDevice);
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
    auto xrDevice = TR_ENSURE_COMPONENT(xrDevice);
    if (xrDevice == NULL)
      return;

    auto hand = xrDevice->getHandInputSource(handness);
    if (hand == nullptr)
      return;
    if (joint < 0 || joint >= 25)
      return; // out of range

    float defaultScale[3] = {1, 1, 1};
    auto baseMatrix = math::makeMatrixFromTRS(translation, rotation, defaultScale, s_WorldScalingFactor);
    hand->joints[joint].setBaseMatrix(baseMatrix);
  }

  DLL_PUBLIC void TransmuteNative_SetHandInputRayPose(int handness, float *translation, float *rotation)
  {
    auto xrDevice = TR_ENSURE_COMPONENT(xrDevice);
    if (xrDevice == NULL)
      return;

    auto hand = xrDevice->getHandInputSource(handness);
    if (hand == nullptr)
      return;
    auto baseMatrix = math::makeMatrixFromTRS(translation, rotation, new float[3]{1, 1, 1}, s_WorldScalingFactor);
    hand->setTargetRayBaseMatrix(baseMatrix);
  }

  DLL_PUBLIC void TransmuteNative_SetHandInputGripPose(int handness, float *translation, float *rotation)
  {
    auto xrDevice = TR_ENSURE_COMPONENT(xrDevice);
    if (xrDevice == NULL)
      return;

    auto hand = xrDevice->getHandInputSource(handness);
    if (hand == nullptr)
      return;
    auto baseMatrix = math::makeMatrixFromTRS(translation, rotation, new float[3]{1, 1, 1}, s_WorldScalingFactor);
    hand->setGripBaseMatrix(baseMatrix);
  }

  DLL_PUBLIC void TransmuteNative_SetHandInputActionState(int handness, int actionType, int state)
  {
    auto xrDevice = TR_ENSURE_COMPONENT(xrDevice);
    if (xrDevice == NULL)
      return;

    auto hand = xrDevice->getHandInputSource(handness);
    if (hand == nullptr)
      return;

    if (actionType == xr::InputSourceActionType::XRPrimaryAction)
      hand->primaryActionPressed = state == 0; /** check if pressed */
    else if (actionType == xr::InputSourceActionType::XRSqueezeAction)
      hand->squeezeActionPressed = state == 0; /** check if pressed */
  }
}
