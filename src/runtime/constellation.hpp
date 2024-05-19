#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <filesystem>

#include <dlfcn.h>
#include "debug.hpp"
#include "native_event.hpp"
#include "renderer/renderer.hpp"

using namespace std;
using namespace std::filesystem;
using namespace native_event;
using namespace renderer;

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
  TrConstellation();
  ~TrConstellation();

public:
  bool initialize(string initJson);
  void tick();
  TrConstellationInit &getOptions();
  bool isInitialized();
  TrNativeEventTarget *getNativeEventTarget();
  TrContentManager *getContentManager();
  TrRenderer *getRenderer();
  xr::Device *getXrDevice();

private:
  TrConstellationInit options;
  TrNativeEventTarget *nativeEventTarget = nullptr;
  TrContentManager *contentManager = nullptr;
  TrRenderer *renderer = nullptr;
  xr::Device *xrDevice = nullptr;
  bool initialized = false;

private:
  static TrConstellation *s_Instance;
};
