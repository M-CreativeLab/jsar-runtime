#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <filesystem>

#include <dlfcn.h>
#include "debug.hpp"
#include "common/events/event_target.hpp"
#include "common/analytics/perf_counter.hpp"
#include "common/analytics/perf_fs.hpp"
#include "renderer/renderer.hpp"
#include "xr/device.hpp"

using namespace std;
using namespace std::filesystem;
using namespace events;
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
  std::unique_ptr<analytics::PerformanceValue<int>> fps;
  std::unique_ptr<analytics::PerformanceValue<int>> drawCallsPerFrame;
  std::unique_ptr<analytics::PerformanceValue<int>> drawCallsCountPerFrame;
  std::unique_ptr<analytics::PerformanceValue<double>> frameDuration;
};

class TrConstellation
{
public:
  TrConstellation(TrEmbedder *embedder);
  ~TrConstellation();

public:
  bool initialize(string initJson);
  void shutdown();
  void tick(analytics::PerformanceCounter &perfCounter);
  TrConstellationInit &getOptions();
  bool isInitialized();
  TrEmbedder *getEmbedder();

public:
  bool onEvent(TrEvent &event, TrContentRuntime *content);

public:
  shared_ptr<TrEventTarget> nativeEventTarget;
  shared_ptr<TrContentManager> contentManager;
  shared_ptr<TrMediaManager> mediaManager;
  shared_ptr<TrRenderer> renderer;
  shared_ptr<xr::Device> xrDevice;
  shared_ptr<TrHostPerformanceFileSystem> perfFs;

private:
  TrConstellationInit options;
  TrEmbedder *embedder = nullptr;
  bool initialized = false;

private:
  static TrConstellation *s_Instance;
};
