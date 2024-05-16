#pragma once

#include <atomic>
#include "native_event.hpp"
#include "constellation.hpp"
#include "per_process.hpp"

using namespace std;

class TrContentManager;
class TrContentRuntime
{
public:
  TrContentRuntime(TrContentManager *manager);
  ~TrContentRuntime();

public:
  void start(native_event::TrXSMLRequestInit init);
  void pause();
  void resume();
  void terminate();
  void dispose();

private:
  void onClientProcess();

private:
  pid_t pid;
  bool isClient;
  native_event::TrXSMLRequestInit requestInit;
  TrConstellationInit constellationOptions;
  TrContentManager *contentManager;

  /**
   * The followings are per-process instances, which should be created for child processes.
   */
  TrClientContextPerProcess *clientContext;
  TrScriptRuntimePerProcess *scriptRuntime;
  // Layout?
  // XR?
};

/**
 * A `TrContentManager` is to manage the lifecycle of content instances, that is, to create, run, and dispose of JavaScript runtime
 * environments.
 */
class TrContentManager
{
private:
  TrContentManager(TrConstellation *constellation);
  ~TrContentManager();

public:
  bool initialize();
  TrContentRuntime *makeContent();
  void disposeContent(TrContentRuntime *content);

private:
  void onRequestEvent(native_event::TrNativeEvent &event);

private:
  TrConstellation *constellation;
  vector<TrContentRuntime *> contentRuntimes;

  friend class TrContentRuntime;
  friend class TrConstellation;
};
