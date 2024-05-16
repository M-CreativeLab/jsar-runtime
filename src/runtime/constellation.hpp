#pragma once

#include <string>
#include <vector>
#include "debug.hpp"

using namespace std;

class TrContentRuntime;
class TrContentManager;

class TrConstellationInit
{
public:
  TrConstellationInit();

public:
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
  TrContentManager *getContentManager();
  TrConstellationInit& getOptions();
  bool isInitialized();

private:
  TrConstellationInit options;
  TrContentManager *contentManager = nullptr;
  bool initialized = false;

private:
  static TrConstellation *s_Instance;
};
