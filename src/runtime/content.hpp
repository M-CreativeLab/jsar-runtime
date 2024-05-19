#pragma once

#include <atomic>
#include <mutex>
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
  int eventChanPort;
  int frameChanPort;
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
  bool tickOnFrame();
  TrContentRuntime *makeContent();
  void disposeContent(TrContentRuntime *content);

private:
  void onRequestEvent(native_event::TrNativeEvent &event);

private:
  TrConstellation *constellation = nullptr;
  TrOneShotServer<CustomEvent> *eventChanServer = nullptr;
  thread *eventChanWatcher = nullptr;
  mutex eventChanMutex;
  vector<TrChannelReceiver<CustomEvent> *> eventChanReceivers;
  vector<TrContentRuntime *> contentRuntimes;
  atomic<bool> watcherRunning = false;

  friend class TrContentRuntime;
  friend class TrConstellation;
};
