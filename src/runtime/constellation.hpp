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

using namespace std;
using namespace std::filesystem;
using namespace renderer;

class TrEmbedder;
class TrContentRuntime;
class TrContentManager;
class TrMediaManager;

class TrConstellationInit
{
public:
  TrConstellationInit();

public:
  string runtimeDirectory;
  string applicationCacheDirectory;
  string httpsProxyServer;
  bool enableV8Profiling = false;
  bool isXRSupported = false;

public:
  /**
   * Fix the environment if needed.
   */
  void fixEnvIfNeeded()
  {
    if (!filesystem::exists(applicationCacheDirectory))
      filesystem::create_directory(applicationCacheDirectory);
  }
  string getZoneDirname()
  {
    string zoneDirname = applicationCacheDirectory + "/.zones";
    if (!filesystem::exists(zoneDirname))
      filesystem::create_directory(zoneDirname);
    return zoneDirname;
  }
  string getZoneFilename(string zoneName)
  {
    return getZoneDirname() + "/" + zoneName;
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

class TrConstellation
{
public:
  TrConstellation(TrEmbedder *embedder);
  ~TrConstellation();

public:
  /**
   * Configure the constellation, such as setting the storage directory, proxy server, etc.
   * 
   * @param init The constellation initialization.
   */
  bool configure(TrConstellationInit& init);
  /**
   * This starts the constellation, initializing the content manager, media manager, renderer, etc, it's better to
   * configure the constellation before this, otherwise, it will use the default configuration.
   */
  bool initialize();
  /**
   * Shutdown the constellation.
   */
  void shutdown();
  /**
   * Tick.
   */
  void tick(analytics::PerformanceCounter &perfCounter);
  /**
   * Get the constellation options.
   */
  TrConstellationInit &getOptions();
  /**
   * Check if the constellation is initialized.
   */
  bool isInitialized();
  /**
   * Check if the runtime is ready to accept the request.
   */
  bool isRuntimeReady();
  /**
   * Get the current embedder.
   */
  TrEmbedder *getEmbedder();

public:
  /**
   * Open a document that sends a request to the given URL.
   * 
   * @param url The URL to be requested.
   * @param init The request initialization.
   * @returns The document ID.
   */
  uint32_t open(string url, optional<TrDocumentRequestInit> init = nullopt);
  /**
   * Send the event to the embedder implementation.
   */
  bool dispatchNativeEvent(events_comm::TrNativeEvent &event, TrContentRuntime *content);

public:
  TrConstellationInit options;
  shared_ptr<events_comm::TrNativeEventTarget> nativeEventTarget;
  shared_ptr<TrContentManager> contentManager;
  shared_ptr<TrMediaManager> mediaManager;
  shared_ptr<TrRenderer> renderer;
  shared_ptr<xr::Device> xrDevice;
  shared_ptr<TrHostPerformanceFileSystem> perfFs;

private:
  TrEmbedder *embedder = nullptr;
  bool initialized = false;

private:
  static TrConstellation *s_Instance;
};
