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
#include "renderer/renderer.hpp"
#include "xr/device.hpp"

using namespace std;
using namespace std::filesystem;
using namespace events;
using namespace renderer;

class TrEmbedder;
class TrContentRuntime;
class TrContentManager;

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
  TrEventTarget *getNativeEventTarget();
  TrContentManager *getContentManager();
  TrRenderer *getRenderer();
  xr::Device *getXrDevice();
  TrEmbedder *getEmbedder();

public:
  bool onEvent(TrEvent &event, TrContentRuntime *content);

private:
  TrConstellationInit options;
  std::unique_ptr<TrEventTarget> nativeEventTarget;
  std::unique_ptr<TrContentManager> contentManager;
  std::unique_ptr<TrRenderer> renderer;
  std::unique_ptr<xr::Device> xrDevice;

private:
  TrEmbedder *embedder = nullptr;
  bool initialized = false;

private:
  static TrConstellation *s_Instance;
};
