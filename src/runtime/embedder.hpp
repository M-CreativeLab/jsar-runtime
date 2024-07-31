#pragma once

#include <string>
#include "constellation.hpp"

using namespace std;
using namespace renderer;

enum class TrHostEngine
{
  None,
  Unity,
  Unreal,
  Cocos,
};

/**
 * The main class for the embedder who want's to embed or integrate the Transmute runtime into their application or environment.
 */
class TrEmbedder
{
protected: // You should create your own embedder class and decide how to create `TrEmbedder` instace.
  TrEmbedder(TrHostEngine hostEngine = TrHostEngine::None);
  virtual ~TrEmbedder() = default;

public: // API for configuration and operations
  /**
   * Configure the XR device, if you want to use Transmute in XR device, you should call this method with true.
   *
   * @param xrEnabled true if the XR device should be enabled, false otherwise.
   * @param init The XR device initialization configuration.
   */
  bool configureXrDevice(bool xrEnabled, xr::TrDeviceInit &init);
  /**
   * Shutdown the Transmute runtime.
   */
  void shutdown();
  /**
   * Get the current FPS of the runtime.
   */
  uint32_t getFps();
  /**
   * Get the current uptime of the runtime.
   */
  uint32_t getUptime();

public: // API for lifecycle
  /**
   * The lifecycle `onStart` should be called when you are ready to start the Transmute runtime, this method will require the initialization and XR
   * device configuration to be done.
   *
   * @param argJson The JSON string of the runtime initialization arguments.
   */
  bool onStart(string argJson);
  /**
   * The lifecycle `onFrame` should be called when your application is to render a frame.
   */
  bool onFrame();
  /**
   * The lifecycle `onEvent` should be called once an event is received from a specfic (content) source.
   */
  virtual bool onEvent(events_comm::TrNativeEvent &event, TrContentRuntime *content) = 0;

public:
  bool isEmbeddingWith(TrHostEngine engine);

public:
  std::shared_ptr<TrConstellation> constellation;

protected:
  TrHostEngine hostEngine = TrHostEngine::None;
};
