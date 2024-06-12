#pragma once

#include <string>
#include "constellation.hpp"
#include "common/events/event_target.hpp"

using namespace std;
using namespace renderer;
using namespace events;

/**
 * The main class for the embedder who want's to embed or integrate the Transmute runtime into their application or environment.
 */
class TrEmbedder
{
protected: // You should create your own embedder class and decide how to create `TrEmbedder` instace.
  TrEmbedder();
  ~TrEmbedder();

public: // API for configuration and operations
  /**
   * Configure the XR device, if you want to use Transmute in XR device, you should call this method with true.
   *
   * @param xrEnabled true if the XR device should be enabled, false otherwise.
   * @param init The XR device initialization configuration.
   */
  bool configureXrDevice(bool xrEnabled, xr::TrDeviceInit& init);
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
  virtual bool onEvent(TrEvent &event, TrContentRuntime *content) = 0;

public: // API for getting sub components
  TrConstellation *getConstellation();
  TrEventTarget *getNativeEventTarget();
  TrRenderer *getRenderer();
  xr::Device *getXrDevice();

protected:
  TrConstellation *constellation = nullptr;
};
