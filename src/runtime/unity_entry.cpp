#include <stdlib.h>
#include "math/matrix.hpp"
#include "base.hpp"
#include "debug.hpp"
#include "platform_base.hpp"
#include "embedder.hpp"
#include "constellation.hpp"
#include "content.hpp"
#include "renderer/render_api.hpp"
#include "xr/device.hpp"

#if defined(__ANDROID__) && (__ANDROID_API__ >= 26)
#include <sys/system_properties.h>
#endif

#ifndef TRANSMUTE_STANDALONE
/**
 * @brief It includes the unity-related headers
 */
#include <Unity/IUnityGraphics.h>
#include <Unity/IUnityLog.h>
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

  bool onEvent(events_comm::TrNativeEvent &event, TrContentRuntime *content) override
  {
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

/**
 * The world scaling factor.
 */
static float s_WorldScalingFactor = 1.0;

/**
 * The platform setup.
 *
 * It fetches the platform properties and configures the embedder accordingly.
 */
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

/**
 * The entry point for the Unity plugin.
 */
extern "C"
{
#ifndef TRANSMUTE_STANDALONE

  /**
   * The load hook for the plugin, it will be called when the plugin is loaded.
   *
   * @param unityInterfaces The Unity interfaces.
   */
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

  /**
   * The unload hook for the plugin, it will be called when the plugin is unloaded.
   */
  DLL_PUBLIC void UnityPluginUnload()
  {
    UnityEmbedder::EnsureAndGet()->unload();
  }

  /**
   * The render event hook for the plugin, it will be called when the plugin is rendering.
   */
  static void OnUnityRenderEvent(int eventID)
  {
    UnityEmbedder::EnsureAndGet()->onFrame();
  }

  /**
   * Get the render event function.
   */
  DLL_PUBLIC UnityRenderingEvent TransmuteUnity_GetRenderEventFunc()
  {
    return OnUnityRenderEvent;
  }

#endif
  /**
   * This parses from the JSON string and configure the Transmute runtime.
   */
  DLL_PUBLIC bool TransmuteUnity_Configure(const char *configJson)
  {
    rapidjson::Document configDoc;
    configDoc.Parse(configJson);
    if (configDoc.HasParseError())
    {
      DEBUG(LOG_TAG_CONSTELLATION, "Failed to parse init json: %s", configJson);
      return false;
    }

    string applicationCacheDirectory = "";
    string httpsProxyServer = "";
    bool enableXR = false;

    if (configDoc.HasMember("applicationCacheDirectory"))
      applicationCacheDirectory = configDoc["applicationCacheDirectory"].GetString();
    if (configDoc.HasMember("httpsProxyServer"))
      httpsProxyServer = configDoc["httpsProxyServer"].GetString();
    if (configDoc.HasMember("isXRSupported") && configDoc["isXRSupported"].IsBool())
      enableXR = configDoc["isXRSupported"].GetBool();

    return UnityEmbedder::EnsureAndGet()->configure(applicationCacheDirectory, httpsProxyServer, enableXR);
  }

  /**
   * Configure the XR device, this is required to execute the JSAR in XR device.
   *
   * @param enabled Whether the XR device is enabled.
   * @param isDeviceActive Whether the XR device is active.
   * @param stereoRenderingMode The stereo rendering mode, 0 for mono, 1 for stereo.
   */
  DLL_PUBLIC bool TransmuteUnity_ConfigureXRDevice(bool isDeviceActive, int stereoRenderingMode)
  {
    xr::TrDeviceInit init;
    init.enabled = true;
    init.active = isDeviceActive;
    init.stereoRenderingMode = (xr::TrStereoRenderingMode)stereoRenderingMode;
    return UnityEmbedder::EnsureAndGet()->configureXrDevice(init);
  }

  /**
   * Starting the Transmute runtime.
   *
   * @param argJson The JSON string of the runtime initialization arguments.
   */
  DLL_PUBLIC bool TransmuteUnity_Start()
  {
    auto embedder = UnityEmbedder::EnsureAndGet();
    bool result = embedder->start();
    if (result)
    {
#if defined(__ANDROID__) && (__ANDROID_API__ >= 26)
      auto opts = embedder->constellation->getOptions();
      __system_property_set("jsar.init.cache_directory", opts.applicationCacheDirectory.c_str());
#endif
    }
    return result;
  }

  /**
   * The options for opening the URL.
   */
  typedef struct
  {
    bool disableCache;
    bool isPreview;
  } UnityDocumentRequestInit;

  /**
   * Open the given URL on a specific content id.
   *
   * @param url The URL to open.
   * @return The document id or 0 if failed.
   */
  DLL_PUBLIC int TransmuteUnity_Open(const char *url, UnityDocumentRequestInit unityInit)
  {
    auto constellation = UnityEmbedder::EnsureAndGet()->constellation;
    TrDocumentRequestInit init;
    init.disableCache = unityInit.disableCache;
    init.isPreview = unityInit.isPreview;
    init.runScripts = TrScriptRunMode::Dangerously;
    return constellation->open(url, make_optional(init));
  }

  /**
   * Fetch the event from the JavaScript side.
   *
   * @param id The event id.
   * @param type The event type.
   * @param size The event data size.
   */
  DLL_PUBLIC bool TransmuteUnity_GetEventFromJavaScript(int *id, int *type, uint32_t *size)
  {
    return UnityEmbedder::EnsureAndGet()->getEventHeader(id, type, size);
  }

  /**
   * Fetch the event data from the JavaScript side.
   *
   * @param data The event data.
   * @return Whether the event data is fetched successfully.
   */
  DLL_PUBLIC void TransmuteUnity_GetEventDataFromJavaScript(const char *data)
  {
    return UnityEmbedder::EnsureAndGet()->getEventData(data);
  }

  /**
   * Dispatch the native event.
   *
   * @param type The event type.
   * @param data The event data.
   */
  DLL_PUBLIC void TransmuteUnity_DispatchNativeEvent(int type, const char *data)
  {
    TR_ENSURE_COMPONENT(nativeEventTarget)->dispatchEvent(static_cast<events_comm::TrNativeEventType>(type), data);
  }

  /**
   * Set the viewport size for current rendering.
   *
   * @param w The width of the viewport.
   * @param h The height of the viewport.
   */
  DLL_PUBLIC void TransmuteUnity_SetViewport(int w, int h)
  {
    TrViewport viewport(w, h);
    TR_ENSURE_COMPONENT(renderer)->setDrawingViewport(viewport);
  }

  /**
   * Set the field of view for the current rendering.
   *
   * @param fov The field of view to be set.
   */
  DLL_PUBLIC void TransmuteUnity_SetFov(float fov)
  {
    TR_ENSURE_COMPONENT(renderer)->setRecommendedFov(fov);
  }

  /**
   * Set the time for the current rendering.
   *
   * @param time The time to be set.
   */
  DLL_PUBLIC void TransmuteUnity_SetTime(float t)
  {
    TR_ENSURE_COMPONENT(renderer)->setTime(t);
  }

  /**
   * Update the projection matrix for a specific eye.
   *
   * @param eyeId The eye id, 0 for left and 1 for right.
   * @param transform The projection matrix to be set, a valid transform is a 16-element float array that represents a 4x4 matrix,
   *                  and it's in column-major order.
   */
  DLL_PUBLIC bool TransmuteUnity_SetViewerStereoProjectionMatrix(int eyeId, float *transform)
  {
    auto xrDevice = TR_ENSURE_COMPONENT(xrDevice);
    if (xrDevice == NULL)
      return false;
    return xrDevice->updateProjectionMatrix(eyeId, transform);
  }

  /**
   * Update the viewer's transform matrix, namely the matrix that describes how to transform the viewer's model to the world space.
   * It will be used to calculate the viewer's view matrix.
   *
   * @param translation The translation part of the transform, a 3-element float array.
   * @param rotation The rotation part of the transform, a 4-element float array that represents a quaternion.
   */
  DLL_PUBLIC bool TransmuteUnity_SetViewerTransformFromTRS(float *translation, float *rotation)
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

  /**
   * Update the viewer's stereo view matrix for a specific eye.
   *
   * @param eyeId The eye id, 0 for left and 1 for right.
   * @param translation The translation part of the transform, a 3-element float array.
   */
  DLL_PUBLIC bool TransmuteUnity_SetViewerStereoViewMatrixFromTRS(int eyeId, float *translation, float *rotation)
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

  /**
   * Update the local transform matrix for a specific applet.
   *
   * @param id The applet id, namely the XR session id.
   * @param translation The translation part of the transform, a 3-element float array.
   * @param rotation The rotation part of the transform, a 4-element float array that represents a quaternion.
   * @param scale The scale part of the transform, a 3-element float array.
   */
  DLL_PUBLIC bool TransmuteUnity_SetLocalTransformFromTRS(int id, float *translation, float *rotation)
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
    return xrDevice->updateLocalTransformByDocumentId(id, m);
  }

  /**
   * __Input Source Updates__
   *
   * Input source represents the input device, such as the controller, hand, etc, in WebXR and OpenXR, it's called input source.
   * Every input source has a unique id, target ray space, grip space basically, and especially for the hand input source, it has
   * the handness, joint pose, etc.
   *
   * Note: as for the hand input source, we use its handness as its id.
   */

  /**
   * Configure the main controller's input source, only this function will be called once, the main controller is able to be used
   * at application-side.
   *
   * @param usingTouch If the main controller is using touch-based input.
   */
  DLL_PUBLIC void TransmuteUnity_ConfigureMainControllerInputSource(bool enabled, bool usingTouch)
  {
    auto xrDevice = TR_ENSURE_COMPONENT(xrDevice);
    if (xrDevice == NULL)
      return;
    xrDevice->configureMainControllerInputSource(enabled, usingTouch);
  }

  /**
   * Update the main controller input source's targetRay pose.
   *
   * @param translation The translation part of the transform, a 3-element float array.
   * @param rotation The rotation part of the transform, a 4-element float array that represents a quaternion.
   */
  DLL_PUBLIC void TransmuteUnity_SetMainControllerInputRayPose(float *translation, float *rotation)
  {
    auto xrDevice = TR_ENSURE_COMPONENT(xrDevice);
    if (xrDevice == NULL)
      return;

    auto mainController = xrDevice->getMainControllerInputSource();
    if (mainController != nullptr)
    {
      auto baseMatrix = math::makeMatrixFromTRS(translation, rotation, new float[3]{1, 1, 1}, s_WorldScalingFactor);
      mainController->setTargetRayBaseMatrix(baseMatrix);
    }
  }

  /**
   * An action is a special type of event that's triggered by the input source, such as the controller button press, etc. Calling
   * this function will not trigger an event to the client side, it just updates the action state such as pressed, released, etc.
   * Then the client side will fetch the action state in a frame and dispatch the events accordingly.
   *
   * See https://developer.mozilla.org/en-US/docs/Web/API/WebXR_Device_API/Inputs#actions for more details.
   *
   * @param action The action type: primary(0), squeeze(1).
   * @param state The action state: pressed(0), released(1).
   */
  DLL_PUBLIC void TransmuteUnity_SetMainControllerInputActionState(int actionType, int state)
  {
    auto xrDevice = TR_ENSURE_COMPONENT(xrDevice);
    if (xrDevice == NULL)
      return;

    auto mainController = xrDevice->getMainControllerInputSource();
    if (mainController != nullptr)
    {
      if (actionType == xr::InputSourceActionType::XRPrimaryAction)
        mainController->primaryActionPressed = state == 0; /** check if pressed */
      else if (actionType == xr::InputSourceActionType::XRSqueezeAction)
        mainController->squeezeActionPressed = state == 0; /** check if pressed */
    }
  }

  /**
   * Update if the hand input source is enabled.
   */
  DLL_PUBLIC void TransmuteUnity_SetHandInputEnabled(int handness, bool enabled)
  {
    auto xrDevice = TR_ENSURE_COMPONENT(xrDevice);
    if (xrDevice == NULL)
      return;

    auto hand = xrDevice->getHandInputSource(handness);
    if (hand != nullptr)
      hand->enabled = enabled;
  }

  /**
   * Update the hand input source's pose.
   *
   * @param handness The handness of the hand, 0 for left and 1 for right.
   * @param joint The joint index of the hand, 0 for wrist, 1 for thumb, 2 for index, 3 for middle, 4 for ring, 5 for pinky.
   * @param translation The position of the joint, a 3-element float array.
   * @param rotation The rotation of the joint, a 4-element float array that represents a quaternion.
   * @param radius The radius of the joint, a float value.
   */
  DLL_PUBLIC void TransmuteUnity_SetHandInputPose(int handness, int joint, float *translation, float *rotation, float radius)
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

  /**
   * Update the target ray pose for the hand input source.
   *
   * @param handness The handness of the hand, 0 for left and 1 for right.
   * @param translation The translation part of the transform, a 3-element float array.
   * @param rotation The rotation part of the transform, a 4-element float array that represents a quaternion.
   */
  DLL_PUBLIC void TransmuteUnity_SetHandInputRayPose(int handness, float *translation, float *rotation)
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

  /**
   * Update the grip pose for the hand input source.
   *
   * @param handness The handness of the hand, 0 for left and 1 for right.
   * @param translation The translation part of the transform, a 3-element float array.
   * @param rotation The rotation part of the transform, a 4-element float array that represents a quaternion.
   */
  DLL_PUBLIC void TransmuteUnity_SetHandInputGripPose(int handness, float *translation, float *rotation)
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

  /**
   * An action is a special type of event that's triggered by the input source, such as the controller button press, etc. Calling
   * this function will not trigger an event to the client side, it just updates the action state such as pressed, released, etc.
   * Then the client side will fetch the action state in a frame and dispatch the events accordingly.
   *
   * See https://developer.mozilla.org/en-US/docs/Web/API/WebXR_Device_API/Inputs#actions for more details.
   *
   * @param handness The handness of the hand, 0 for left and 1 for right.
   * @param action The action type: primary(0), squeeze(1).
   * @param state The action state: pressed(0), released(1).
   */
  DLL_PUBLIC void TransmuteUnity_SetHandInputActionState(int handness, int actionType, int state)
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
