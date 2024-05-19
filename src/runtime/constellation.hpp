#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <filesystem>

#include <dlfcn.h>
#include "debug.hpp"
#include "renderer/renderer.hpp"

using namespace std;
using namespace std::filesystem;

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
  static TrConstellation *Create();
  static TrConstellation *Get();

private:
  TrConstellation();
  ~TrConstellation();

public:
  void initialize(const char *initJson);
  void tick();
  TrConstellationInit &getOptions();
  bool isInitialized();
  TrContentManager *getContentManager();
  renderer::TrRenderer *getRenderer();
  xr::Device *getXrDevice();

private:
  TrConstellationInit options;
  TrContentManager *contentManager = nullptr;
  renderer::TrRenderer *renderer = nullptr;
  xr::Device *xrDevice = nullptr;
  bool initialized = false;

private:
  static TrConstellation *s_Instance;
};
