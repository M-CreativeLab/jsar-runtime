#pragma once

#include <string>
#include "./constellation.hpp"

/**
 * The host engines that the JSAR runtime would be embedded with.
 */
enum class TrHostEngine
{
  // None means standalone runtime.
  None,
  // Unity engine.
  Unity,
  // Unreal engine.
  Unreal,
  // Cocos engine.
  Cocos,
};

/**
 * The main class for the embedder who want's to embed or integrate the Transmute runtime into their application or environment.
 */
class TrEmbedder
{
protected: // You should create your own embedder class and decide how to create `TrEmbedder` instace.
  /**
   * Construct a new `TrEmbedder` instance.
   *
   * @param hostEngine The host engine that the embedder is embedding with.
   */
  TrEmbedder(TrHostEngine hostEngine = TrHostEngine::None);
  virtual ~TrEmbedder() = default;

public: // API for configuration and operations
  /**
   * Configure the JSAR runtime.
   *
   * Note: if you configure `enableXR` to true, you should call `configureXrDevice` to configure the XR device.
   *
   * @param storageDirectory The directory for the JSAR storage including the socket, mmap, and cache files.
   * @param httpsProxyServer The HTTPS proxy server address, or empty string if not needed.
   * @param enableXR true if the XR device should be enabled, false otherwise.
   * @returns true if the configuration is successful, false otherwise.
   */
  bool configure(std::string storageDirectory, std::string httpsProxyServer, bool enableXR);
  /**
   * Configure the XR device, if you want to use this runtime in XR device, you should call this method with true.
   *
   * @param xrEnabled true if the XR device should be enabled, false otherwise.
   * @param init The XR device initialization configuration.
   * @returns true if the configuration is successful, false otherwise.
   */
  bool configureXrDevice(xr::TrDeviceInit &init);
  /**
   * The authorization-related headers in HTTP requests will be sent at the client-side. Call this method to configure
   * the raw headers which contains the authorization information for specific origins.
   *
   * @param rawHeaders the raw headers.
   * @param allowedOrigins the allowed origins for the raw headers, if empty, all origins are allowed.
   */
  void setRequestAuthorizationHeaders(std::string rawHeaders, std::vector<std::string> allowedOrigins = {});
  /**
   * Start the runtime.
   *
   * @returns true if the runtime is started, false otherwise.
   */
  inline bool start() { return constellation->initialize(); }
  /**
   * Shutdown the runtime.
   */
  inline void shutdown() { constellation->shutdown(); }
  /**
   * @returns the server-side fps of the runtime.
   */
  inline uint32_t getFps() { return constellation->renderer->getFps(); }
  /**
   * @returns the server-side uptime of the runtime.
   */
  inline uint32_t getUptime() { return constellation->renderer->getUptime(); }
  /**
   * @returns the version of the runtime.
   */
  inline std::string getVersion()
  {
#ifdef TR_VERSION
    return TR_VERSION;
#else
    return "unset";
#endif
  }

public: // API for lifecycle
  /**
   * The lifecycle `onFrame` should be called when your application is to render a frame.
   *
   * @returns `true` if the frame is rendered, `false` otherwise.
   */
  bool onFrame();
  /**
   * The lifecycle `onEvent` should be called once an event is received from a specfic (content) source.
   *
   * @param event The native event from the source.
   * @param content The content runtime which the event is from.
   * @returns true if the event is handled, false otherwise.
   */
  virtual bool onEvent(events_comm::TrNativeEvent &event, std::shared_ptr<TrContentRuntime> content) = 0;

public:
  /**
   * Check if the embedder is embedding with a specific host engine.
   *
   * @param engine The host engine to check, e.g. Unity, Unreal, Cocos.
   * @returns `true` if the embedder is embedding with the host engine, `false` otherwise.
   */
  inline bool isEmbeddingWith(TrHostEngine engine) { return hostEngine == engine; }

public:
  std::shared_ptr<TrConstellation> constellation;

protected:
  TrHostEngine hostEngine = TrHostEngine::None;
};
