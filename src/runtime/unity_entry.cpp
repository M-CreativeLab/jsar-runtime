#include <stdlib.h>
#include <common/debug.hpp>
#include <math/matrix.hpp>
#include <renderer/render_api.hpp>
#include <xr/device.hpp>

#include "base.hpp"
#include "./platform_base.hpp"
#include "./embedder.hpp"
#include "./constellation.hpp"
#include "./content_manager.hpp"

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
class UnityEmbedder final : public TrEmbedder
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

    /**
     * Check if the graphics debug log should be enabled by the property `jsar.renderer.graphics.debug`.
     */
#if defined(__ANDROID__) && (__ANDROID_API__ >= 26)
    char enableGraphicsDebugLogStr[PROP_VALUE_MAX];
    if (__system_property_get("jsar.renderer.graphics.debug", enableGraphicsDebugLogStr) >= 0)
    {
      if (strcmp(enableGraphicsDebugLogStr, "yes") == 0)
        api->EnableGraphicsDebugLog(true);
    }
#endif
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

  bool onEvent(events_comm::TrNativeEvent &event, std::shared_ptr<TrContentRuntime> content) override
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
  return s_EmbedderInstance;
}

#define TR_ENSURE_EMBEDDER(falseValue, block)    \
  auto embedder = UnityEmbedder::EnsureAndGet(); \
  if (TR_UNLIKELY(embedder == nullptr))          \
    return falseValue;                           \
  block

#define TR_ENSURE_COMPONENT(name, falseValue, block) \
  auto embedder = UnityEmbedder::EnsureAndGet();     \
  if (TR_UNLIKELY(embedder == nullptr))              \
    return falseValue;                               \
  auto name = embedder->constellation->name;         \
  if (TR_UNLIKELY(name == nullptr))                  \
    return falseValue;                               \
  block

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
  if (embedder == nullptr)
  {
    DEBUG(LOG_TAG_UNITY, "Skip setting for the platform, reason: embedder is not created.");
    return;
  }
  auto renderer = embedder->constellation->renderer;

#if defined(__ANDROID__) && (__ANDROID_API__ >= 26)
  // Update the runtime version
  __system_property_set("jsar.runtime.version", embedder->getVersion().c_str());
  // TODO: add "jsar.runtime.versions"?

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
      char setupEndpointStr[PROP_VALUE_MAX];
      if (__system_property_get("jsar.setup.threepio.api.endpoint", setupStr) >= 0)
        setenv("JSAR_SETUP_THREEPIO_API_ENDPOINT", setupStr, 1);
      char setupAPIProviderStr[PROP_VALUE_MAX];
      if (__system_property_get("jsar.setup.threepio.api.provider", setupAPIProviderStr) >= 0)
        setenv("JSAR_SETUP_THREEPIO_API_PROVIDER", setupAPIProviderStr, 1);
      char setupAPIKeyStr[PROP_VALUE_MAX];
      if (__system_property_get("jsar.setup.threepio.api.key", setupAPIKeyStr) >= 0)
        setenv("JSAR_SETUP_THREEPIO_API_KEY", setupAPIKeyStr, 1);
      char setupModeldStr[PROP_VALUE_MAX];
      if (__system_property_get("jsar.setup.threepio.api.modelid", setupModeldStr) >= 0)
        setenv("JSAR_SETUP_THREEPIO_API_MODELID", setupModeldStr, 1);

      char enableRendererTracingStr[PROP_VALUE_MAX];
      if (
          __system_property_get("jsar.renderer.tracing", enableRendererTracingStr) >= 0 &&
          strcmp(enableRendererTracingStr, "yes") == 0)
        renderer->enableTracing();

      char enablePrintHostContextSummaryStr[PROP_VALUE_MAX];
      if (
          __system_property_get("jsar.renderer.print_host_context_summary", enablePrintHostContextSummaryStr) >= 0 &&
          strcmp(enablePrintHostContextSummaryStr, "yes") == 0)
        renderer->enableHostContextSummary();

      char enablePrintAppContextSummaryStr[PROP_VALUE_MAX];
      if (
          __system_property_get("jsar.renderer.print_app_context_summary", enablePrintAppContextSummaryStr) >= 0 &&
          strcmp(enablePrintAppContextSummaryStr, "yes") == 0)
        renderer->enableAppContextSummary();

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
    UnityEmbedder *embedder = nullptr;
#if defined(UNITY_OSX) || defined(UNITY_WIN)
    return;
#else
    embedder = UnityEmbedder::Create(unityInterfaces);
#endif
    OnPlatformSetup(embedder);
  }

  /**
   * The unload hook for the plugin, it will be called when the plugin is unloaded.
   */
  DLL_PUBLIC void UnityPluginUnload()
  {
    TR_ENSURE_EMBEDDER(/** void */, { embedder->unload(); });
  }

  /**
   * The render event hook for the plugin, it will be called when the plugin is rendering.
   */
  static void OnUnityRenderEvent(int eventID)
  {
    TR_ENSURE_EMBEDDER(/** void */, { embedder->onFrame(); });
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
   *
   * @param configJson The JSON string to configure the runtime.
   */
  DLL_PUBLIC bool TransmuteUnity_Configure(const char *configJson)
  {
    auto embedder = UnityEmbedder::EnsureAndGet();
    if (embedder == nullptr)
      return false;

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

    return embedder->configure(applicationCacheDirectory, httpsProxyServer, enableXR);
  }

  /**
   * Configure the XR device, this is required to execute the JSAR in XR device.
   *
   * @param isDeviceActive Whether the XR device is active.
   * @param stereoRenderingMode The stereo rendering mode, 0 for mono, 1 for stereo.
   */
  DLL_PUBLIC bool TransmuteUnity_ConfigureXRDevice(bool isDeviceActive, int stereoRenderingMode)
  {
    auto embedder = UnityEmbedder::EnsureAndGet();
    if (embedder != nullptr)
    {
      xr::TrDeviceInit init;
      init.enabled = true;
      init.active = isDeviceActive;
      init.stereoRenderingMode = (xr::TrStereoRenderingMode)stereoRenderingMode;
      return embedder->configureXrDevice(init);
    }
    else
    {
      return false;
    }
  }

  /**
   * Starting the Transmute runtime.
   */
  DLL_PUBLIC bool TransmuteUnity_Start()
  {
    auto embedder = UnityEmbedder::EnsureAndGet();
    if (embedder != nullptr)
    {
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
    else
    {
      return false;
    }
  }

  /**
   * The options for opening an URL.
   */
  typedef struct
  {
    /**
     * Whether to disable the cache.
     */
    bool disableCache;
    /**
     * Whether rendering in preview mode.
     */
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
    auto embedder = UnityEmbedder::EnsureAndGet();
    if (embedder == nullptr || embedder->constellation == nullptr)
      return 0;

    TrDocumentRequestInit init;
    init.disableCache = unityInit.disableCache;
    init.isPreview = unityInit.isPreview;
    init.runScripts = TrScriptRunMode::Dangerously;
    return embedder->constellation->open(url, make_optional(init));
  }

  /**
   * Pause a specific document.
   *
   * The paused document will not receive updates from the host, and there will be no rendering, and the developer should resume the document
   * via `TransmuteUnity_Resume` to continue the rendering.
   *
   * The usecase of pausing a document is to hide a specific document when it's not visible or not needed to render.
   *
   * @param documentId The document id to pause.
   * @return Whether the document is paused successfully.
   */
  DLL_PUBLIC bool TransmuteUnity_Puase(int documentId)
  {
    TR_ENSURE_COMPONENT(contentManager, false, {});
    auto content = contentManager->getContent(documentId, false);
    if (content == nullptr)
    {
      DEBUG(LOG_TAG_UNITY, "Could not find the content with id: %d", documentId);
      return false;
    }
    content->pause();
    return true;
  }

  /**
   * Resume the paused document, see `TransmuteUnity_Pause()` for more details.
   *
   * @param documentId The document id to resume.
   * @return Whether the document is resumed successfully.
   */
  DLL_PUBLIC bool TransmuteUnity_Resume(int documentId)
  {
    TR_ENSURE_COMPONENT(contentManager, false, {});
    auto content = contentManager->getContent(documentId, false);
    if (content == nullptr)
    {
      DEBUG(LOG_TAG_UNITY, "Could not find the content with id: %d", documentId);
      return false;
    }
    content->resume();
    return true;
  }

  /**
   * Close the document with the given document id.
   *
   * @param documentId The document id to close.
   * @return Whether the document is closed successfully.
   */
  DLL_PUBLIC bool TransmuteUnity_Close(int documentId)
  {
    TR_ENSURE_COMPONENT(contentManager, false, {});
    auto content = contentManager->getContent(documentId, false);
    if (content == nullptr)
    {
      DEBUG(LOG_TAG_UNITY, "Could not find the content with id: %d", documentId);
      return false;
    }
    content->dispose(false);
    return true;
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
    TR_ENSURE_EMBEDDER(false, { return embedder->getEventHeader(id, type, size); });
  }

  /**
   * Fetch the event data from the JavaScript side.
   *
   * @param data The event data.
   * @return Whether the event data is fetched successfully.
   */
  DLL_PUBLIC void TransmuteUnity_GetEventDataFromJavaScript(const char *data)
  {
    TR_ENSURE_EMBEDDER(/** void */, { embedder->getEventData(data); });
  }

  /**
   * Dispatch the native event.
   *
   * @param type The event type.
   * @param data The event data.
   */
  DLL_PUBLIC bool TransmuteUnity_DispatchNativeEvent(int type, const char *data)
  {
    TR_ENSURE_COMPONENT(nativeEventTarget, false, { return nativeEventTarget->dispatchEvent(static_cast<events_comm::TrNativeEventType>(type), data); });
  }

  /**
   * Set the viewport size for current rendering.
   *
   * @param w The width of the viewport.
   * @param h The height of the viewport.
   */
  DLL_PUBLIC void TransmuteUnity_SetViewport(int w, int h)
  {
    TR_ENSURE_COMPONENT(renderer, /** void */, {
      TrViewport viewport(w, h);
      renderer->setDrawingViewport(viewport);
    });
  }

  /**
   * Set the field of view for the current rendering.
   *
   * @param fov The field of view to be set.
   */
  DLL_PUBLIC void TransmuteUnity_SetFov(float fov)
  {
    TR_ENSURE_COMPONENT(renderer, /** void */, {
      renderer->setRecommendedFov(fov);
    });
  }

  /**
   * Set the time for the current rendering.
   *
   * @param time The time to be set.
   */
  DLL_PUBLIC void TransmuteUnity_SetTime(float t)
  {
    TR_ENSURE_COMPONENT(renderer, /** void */, {
      renderer->setTime(t);
    });
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
    TR_ENSURE_COMPONENT(xrDevice, false, { return xrDevice->updateProjectionMatrix(eyeId, transform); });
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
    TR_ENSURE_COMPONENT(xrDevice, false, {
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
    });
  }

  /**
   * Update the viewer's stereo view matrix for a specific eye.
   *
   * @param eyeId The eye id, 0 for left and 1 for right.
   * @param translation The translation part of the transform, a 3-element float array.
   */
  DLL_PUBLIC bool TransmuteUnity_SetViewerStereoViewMatrixFromTRS(int eyeId, float *translation, float *rotation)
  {
    TR_ENSURE_COMPONENT(xrDevice, false, {
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
    });
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
    TR_ENSURE_COMPONENT(xrDevice, false, {
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
    });
  }

  /**
   * Get the collision box for a specific applet.
   *
   * @param id The applet id.
   * @param outMin The minimum point of the bounding box, a 3-element float array.
   * @param outMax The maximum point of the bounding box, a 3-element float array.
   * @return Whether the collision box is fetched successfully.
   */
  DLL_PUBLIC bool TransmuteUnity_GetCollisionBox(int id, float *outMin, float *outMax)
  {
    TR_ENSURE_COMPONENT(xrDevice, false, {});
    return xrDevice->getCollisionBoxByDocumentId(id, outMin, outMax);
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
   * Get the main controller's input source id, that is the key to call other input source related APIs.
   *
   * @returns The main controller's input source id.
   */
  DLL_PUBLIC int TransmuteUnity_GetMainControllerInputSource()
  {
    TR_ENSURE_COMPONENT(xrDevice, -1, {
      auto mainController = xrDevice->getMainControllerInputSource();
      return mainController != nullptr ? mainController->id : -1;
    });
  }

  /**
   * Get the hand input source id by the handness.
   *
   * @param handness The handness, 0 for left and 1 for right.
   * @returns The hand input source id.
   */
  DLL_PUBLIC int TransmuteUnity_GetHandInputSource(int handness)
  {
    TR_ENSURE_COMPONENT(xrDevice, -1, {
      auto hand = xrDevice->getHandInputSource(handness);
      return hand != nullptr ? hand->id : -1;
    });
  }

  /**
   * Get the screen controller input source id by the index.
   *
   * @param index The index of the screen controller.
   * @returns The screen controller input source id.
   */
  DLL_PUBLIC int TransmuteUnity_GetScreenControllerInputSource(int index)
  {
    TR_ENSURE_COMPONENT(xrDevice, -1, {
      auto screenController = xrDevice->getScreenControllerInputSource(index);
      return screenController != nullptr ? screenController->id : -1;
    });
  }

  /**
   * Update if the main controller's input source is enabled.
   *
   * @param enabled Whether the main controller's input source is enabled.
   */
  DLL_PUBLIC void TransmuteUnity_SetInputSourceEnabled(int id, bool enabled)
  {
    TR_ENSURE_COMPONENT(xrDevice, /** void */, {});
    auto inputSource = xrDevice->getInputSourceById(id);
    if (inputSource != nullptr)
      inputSource->enabled = enabled;
  }

  /**
   * Update the target ray pose for the specific input source.
   *
   * @param id The input source id.
   * @param translation The translation part of the transform, a 3-element float array.
   * @param rotation The rotation part of the transform, a 4-element float array that represents a quaternion.
   */
  DLL_PUBLIC void TranmusteUnity_SetInputSourceRayPose(int id, float *translation, float *rotation)
  {
    TR_ENSURE_COMPONENT(xrDevice, /** void */, {});
    if (id < 0)
    {
      DEBUG(LOG_TAG_UNITY, "Invalid input source id: %d", id);
      return;
    }
    auto inputSource = xrDevice->getInputSourceById(id);
    if (inputSource != nullptr)
    {
      auto baseMatrix = math::CreateMatrixFromTRS(translation, rotation, new float[3]{1, 1, 1}, s_WorldScalingFactor);
      inputSource->setTargetRayBaseMatrix(baseMatrix);
    }
    else
    {
      DEBUG(LOG_TAG_UNITY, "Failed to find the input source by id: %d", id);
    }
  }

  /**
   * Update the grip pose for the specific input source.
   *
   * @param id The input source id.
   * @param translation The translation part of the transform, a 3-element float array.
   * @param rotation The rotation part of the transform, a 4-element float array that represents a quaternion.
   */
  DLL_PUBLIC void TransmuteUnity_SetInputSourceGripPose(int id, float *translation, float *rotation)
  {
    TR_ENSURE_COMPONENT(xrDevice, /** void */, {});
    auto inputSource = xrDevice->getInputSourceById(id);
    if (inputSource != nullptr)
    {
      auto baseMatrix = math::CreateMatrixFromTRS(translation, rotation, new float[3]{1, 1, 1}, s_WorldScalingFactor);
      inputSource->setGripBaseMatrix(baseMatrix);
    }
    else
    {
      DEBUG(LOG_TAG_UNITY, "Failed to find the input source by id: %d", id);
    }
  }

  /**
   * Get the hit result of the target ray for the specific input source.
   *
   * @param id The input source id.
   * @param outHitPosition The hit position, a 3-element float array.
   * @param outHitRotationQuat The hit rotation quaternion, a 4-element float array.
   * @return Whether the hit result is valid.
   */
  DLL_PUBLIC bool TransmuteUnity_GetInputSourceRayHitResult(int id, float *outHitPosition, float *outHitRotationQuat)
  {
    TR_ENSURE_COMPONENT(xrDevice, false, {});
    auto inputSource = xrDevice->getInputSourceById(id);
    if (inputSource == nullptr)
    {
      DEBUG(LOG_TAG_UNITY, "Failed to find the input source by id: %d", id);
      return false;
    }

    if (!inputSource->targetRayHitResult.hit)
    {
      return false;
    }
    else
    {
      float *matrixValues = inputSource->targetRayHitResult.baseMatrix;
      glm::mat4 hitMatrix = glm::make_mat4(matrixValues);
      glm::vec3 position = glm::vec3(hitMatrix[3]);
      glm::quat rotation = glm::quat_cast(hitMatrix);

      // Convert to Unity Coordinate System
      outHitPosition[0] = position.x;
      outHitPosition[1] = position.y;
      outHitPosition[2] = -position.z;
      outHitRotationQuat[0] = -rotation.x;
      outHitRotationQuat[1] = -rotation.y;
      outHitRotationQuat[2] = rotation.z;
      outHitRotationQuat[3] = rotation.w;
      return true;
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
  DLL_PUBLIC void TransmuteUnity_SetInputSourceActionState(int id, int actionType, int state)
  {
    TR_ENSURE_COMPONENT(xrDevice, /** void */, {});
    auto inputSource = xrDevice->getInputSourceById(id);
    if (inputSource != nullptr)
    {
      if (actionType == xr::InputSourceActionType::XRPrimaryAction)
        inputSource->primaryActionPressed = state == 0; /** check if pressed */
      else if (actionType == xr::InputSourceActionType::XRSqueezeAction)
        inputSource->squeezeActionPressed = state == 0; /** check if pressed */
    }
  }

  /**
   * Update the hand input source's joint pose.
   *
   * @param handness The handness of the hand, 0 for left and 1 for right.
   * @param joint The joint index of the hand, 0 for wrist, 1 for thumb, 2 for index, 3 for middle, 4 for ring, 5 for pinky.
   * @param translation The position of the joint, a 3-element float array.
   * @param rotation The rotation of the joint, a 4-element float array that represents a quaternion.
   * @param radius The radius of the joint, a float value.
   */
  DLL_PUBLIC void TransmuteUnity_SetHandInputSourceJointPose(int handness, int joint, float *translation, float *rotation, float radius)
  {
    TR_ENSURE_COMPONENT(xrDevice, /** void */, {});
    auto hand = xrDevice->getHandInputSource(handness);
    if (hand == nullptr)
      return;
    if (joint < 0 || joint >= 25)
      return; // out of range

    float defaultScale[3] = {1, 1, 1};
    auto baseMatrix = math::CreateMatrixFromTRS(translation, rotation, defaultScale, s_WorldScalingFactor);
    hand->joints[joint].setBaseMatrix(baseMatrix);
  }
}
