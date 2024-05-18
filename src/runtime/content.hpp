#pragma once

#include <atomic>
#include <filesystem>

#include "native_event.hpp"
#include "constellation.hpp"
#include "ipc.hpp"

using namespace std;
using namespace ipc;

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
  int channelServerPort;
  native_event::TrXSMLRequestInit requestInit;
  TrConstellationInit constellationOptions;
  TrContentManager *contentManager;
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
  TrOneShotServer<CustomEvent> *eventChanServer;
  thread *eventChanWorker;
  vector<TrContentRuntime *> contentRuntimes;

  friend class TrContentRuntime;
  friend class TrConstellation;
};
