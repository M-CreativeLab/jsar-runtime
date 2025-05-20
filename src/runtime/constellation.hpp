#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <filesystem>

#include <dlfcn.h>
#include "debug.hpp"
#include "common/options.hpp"
#include "common/events_v2/native_event.hpp"
#include "common/analytics/perf_counter.hpp"
#include "common/analytics/perf_fs.hpp"
#include "renderer/renderer.hpp"
#include "xr/device.hpp"

class TrEmbedder;
class TrContentRuntime;
class TrContentManager;
class TrMediaManager;
#ifdef TR_ENABLE_INSPECTOR
class TrInspector;
#endif

/**
 * The constellation initialization options.
 *
 * - `applicationCacheDirectory`: The root directory to store the runtime files.
 * - `runDirectory`: The directory that the library is running in.
 * - `httpsProxyServer`: The https proxy server.
 * - `enableV8Profiling`: Enable the v8 profiling at client-side.
 * - `isXRSupported`: Supports the WebXR API.
 */
class TrConstellationInit final
{
public:
  TrConstellationInit() = default;

public:
  /**
   * The root directory to store the runtime files.
   */
  std::string applicationCacheDirectory;
  /**
   * The directory that the library is running in.
   */
  std::string runDirectory;
  /**
   * The https proxy server.
   */
  std::string httpsProxyServer;
  /**
   * Enable the v8 profiling at client-side.
   */
  bool enableV8Profiling = false;
  /**
   * Supports the WebXR API.
   */
  bool isXRSupported = false;

public:
  /**
   * @returns The runtime directory to store the Transmute exectable files or the runtime files.
   */
  inline std::string runtimeDirectory() { return applicationCacheDirectory + "/runtime"; }
  /**
   * @returns The scripts directory to store the scripts such as the bundle scripts.
   */
  inline std::string scriptsDirectory() { return applicationCacheDirectory + "/scripts"; }

public:
  /**
   * Make the environment ready, calling this function before the runtime starts to fix the environment issues.
   */
  void fixEnvIfNeeded()
  {
    if (!filesystem::exists(applicationCacheDirectory))
      filesystem::create_directory(applicationCacheDirectory);

    // TODO: Ensure the runtime and scripts directory?
  }
  /**
   * Get the zone directory name.
   *
   * NOTE: call this function will have side effect that it will create the directory if it doesn't exist.
   *
   * @param subDir The sub directory name, `nullopt` means no sub directory.
   * @returns The zone directory name.
   */
  std::string getZoneDirname(optional<std::string> subDir = std::nullopt)
  {
    auto zoneDirname = applicationCacheDirectory + "/.zones";
    if (!filesystem::exists(zoneDirname))
      filesystem::create_directory(zoneDirname);

    // Ensure the sub directory.
    if (subDir.has_value())
    {
      zoneDirname += "/" + subDir.value();
      if (!filesystem::exists(zoneDirname))
        filesystem::create_directory(zoneDirname);
    }
    return zoneDirname;
  }
  /**
   * Get the full path by its zone name.
   *
   * NOTE: call this function will have side effect that it will create the directory if it doesn't exist.
   *
   * @returns The full path of the zone filename.
   */
  inline std::string getZoneFilename(std::string zoneName)
  {
    return getZoneDirname() + "/" + zoneName;
  }
  /**
   * Get the full path by its zone name with a sub directory: {zoneRoot}/{subDir}/{zoneName}.
   *
   * NOTE: call this function will have side effect that it will create the directory if it doesn't exist.
   *
   * @returns The full path of the zone filename.
   */
  inline std::string getZoneFilename(std::string zoneName, std::string subDir)
  {
    return getZoneDirname(subDir) + "/" + zoneName;
  }
};

class TrHostPerformanceFileSystem : public analytics::PerformanceFileSystem
{
public:
  TrHostPerformanceFileSystem(TrConstellationInit &init)
      : analytics::PerformanceFileSystem(init.applicationCacheDirectory)
  {
    fps = makeValue<int>("host_fps", -1);
    drawCallsPerFrame = makeValue<int>("host_drawcalls_per_frame", -1);
    drawCallsCountPerFrame = makeValue<int>("host_drawcalls_count_per_frame", -1);
    frameDuration = makeValue<double>("host_frame_duration", -1.0);
  }
  ~TrHostPerformanceFileSystem() = default;

public:
  inline void setFps(int value) { fps->set(value); }
  inline void setDrawCallsPerFrame(int value) { drawCallsPerFrame->set(value); }
  inline void setDrawCallsCountPerFrame(int value) { drawCallsCountPerFrame->set(value); }
  inline void setFrameDuration(double value) { frameDuration->set(value); }

public:
  unique_ptr<analytics::PerformanceValue<int>> fps;
  unique_ptr<analytics::PerformanceValue<int>> drawCallsPerFrame;
  unique_ptr<analytics::PerformanceValue<int>> drawCallsCountPerFrame;
  unique_ptr<analytics::PerformanceValue<double>> frameDuration;
};

/**
 * The constellation class contains the content manager, media manager, renderer, etc. This class is the main entry
 * point for the runtime, it manages the lifecycle of the runtime and sub-systems.
 */
class TrConstellation
{
public:
  /**
   * Construct a new constellation with a given embedder instance.
   *
   * @param embedder The embedder instance.
   * @return The new constellation instance.
   */
  TrConstellation(TrEmbedder *embedder);
  ~TrConstellation();

public:
  /**
   * Configure the constellation, such as setting the storage directory, proxy server, etc.
   *
   * @param init The constellation initialization.
   * @returns If the constellation is configured successfully.
   */
  bool configure(TrConstellationInit &init);
  /**
   * This starts the constellation, initializing the content manager, media manager, renderer, etc, it's better to
   * configure the constellation before this, otherwise, it will use the default configuration.
   *
   * @returns If the constellation is initialized successfully.
   */
  bool initialize();
  /**
   * Shutdown the constellation.
   */
  void shutdown();
  /**
   * This method `tick()` should be called in the main loop of the runtime, it will update the sub-systems.
   *
   * @param perfCounter The performance counter to record the time.
   */
  void tick(analytics::PerformanceCounter &perfCounter);
  /**
   * @returns The reference to the constellation options.
   */
  TrConstellationInit &getOptions();
  /**
   * @returns If the constellation is initialized.
   */
  bool isInitialized();
  /**
   * @returns If the runtime is ready.
   */
  bool isRuntimeReady();
  /**
   * @returns The embedder pointer.
   */
  TrEmbedder *getEmbedder();

public:
  /**
   * Open a document that sends a request to the given URL.
   *
   * @param url The URL to be requested.
   * @param init The request initialization.
   * @returns The document id.
   */
  uint32_t open(string url, optional<TrDocumentRequestInit> init = nullopt);
  /**
   * Close the document with the given document id.
   * 
   * @param id The document/content id to be closed.
   * @returns If the document is closed successfully.
   */
  bool close(uint32_t documentId);
  /**
   * Reset all the contents, this will remove all the contents and free the resources.
   */
  void resetContents();
  /**
   * Dispatch an event to the specified content.
   *
   * @param event The native event to be dispatched.
   * @param content The content to dispatch the event.
   * @returns If the event is dispatched successfully.
   */
  bool dispatchNativeEvent(events_comm::TrNativeEvent &event, std::shared_ptr<TrContentRuntime> content);

public:
  /**
   * The init options of the constellation, it's updated once when the `configure()` method is called.
   */
  TrConstellationInit options;
  /**
   * The smart pointer to the native event target.
   */
  std::shared_ptr<events_comm::TrNativeEventTarget> nativeEventTarget;
  /**
   * The smart pointer to the contents manager.
   *
   * A content manager does manage the lifecycle of the contents, it creates, updates, and destroys the contents
   * based on the requests from the embedder.
   */
  std::shared_ptr<TrContentManager> contentManager;
  /**
   * The smart pointer to the media manager.
   */
  std::shared_ptr<TrMediaManager> mediaManager;
  /**
   * The smart pointer to the renderer.
   *
   * A renderer does control the rendering of each content, and it's responsible for managing the graphics context
   * recording and restoring, etc.
   */
  std::shared_ptr<renderer::TrRenderer> renderer;
  /**
   * The smart pointer to the XR device.
   *
   * An XR device does manage the XR session, frames, input sources, etc.
   */
  std::shared_ptr<xr::Device> xrDevice;
  /**
   * The performance file system for the host.
   */
  std::shared_ptr<TrHostPerformanceFileSystem> perfFs;

#ifdef TR_ENABLE_INSPECTOR
  /**
   * The smart pointer to the inspector.
   */
  std::shared_ptr<TrInspector> inspector;
#endif

private:
  TrEmbedder *embedder = nullptr;
  bool initialized = false;
  bool disableTicking = false;

private:
  static TrConstellation *s_Instance;
};
