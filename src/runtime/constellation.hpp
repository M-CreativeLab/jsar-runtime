#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <filesystem>

#include <dlfcn.h>
#include "debug.hpp"
#include "common/events/event_target.hpp"
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
  bool isXRSupported;
};

class TrConstellation
{
public:
  TrConstellation(TrEmbedder *embedder);
  ~TrConstellation();

public:
  bool initialize(string initJson);
  void tick();
  TrConstellationInit &getOptions();
  bool isInitialized();
  TrEventTarget *getNativeEventTarget();
  TrContentManager *getContentManager();
  TrRenderer *getRenderer();
  xr::Device *getXrDevice();

public:
  bool onEvent(TrEvent &event, TrContentRuntime *content);

private:
  TrConstellationInit options;
  TrEventTarget *nativeEventTarget = nullptr;
  TrContentManager *contentManager = nullptr;
  TrRenderer *renderer = nullptr;
  TrEmbedder *embedder = nullptr;
  xr::Device *xrDevice = nullptr;
  bool initialized = false;

private:
  static TrConstellation *s_Instance;
};
