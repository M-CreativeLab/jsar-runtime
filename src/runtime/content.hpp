#pragma once

#include <atomic>
#include <mutex>
#include <filesystem>

#include "common/classes.hpp"
#include "common/ipc.hpp"
#include "common/command_buffers.hpp"
#include "native_event.hpp"
#include "constellation.hpp"

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

public: // command buffer methods
  void setupWithCommandBufferClient(TrOneShotClient<TrCommandBufferMessage> *client);

private:
  void onClientProcess();
  bool testClientProcessExitOnFrame(); // true if the client process has exited
  void recvCommandBuffers(uint32_t timeout);
  bool tickOnFrame();

private:
  pid_t pid = -1;
  int eventChanPort;
  int frameChanPort;
  int commandBufferChanPort;
  native_event::TrXSMLRequestInit requestInit;
  TrConstellationInit constellationOptions;
  TrContentManager *contentManager;
  TrChannelReceiver<CustomEvent> *eventChanReceiver = nullptr;
  TrChannelSender<CustomEvent> *eventChanSender = nullptr;
  TrCommandBufferReceiver *commandBufferChanReceiver = nullptr;
  TrCommandBufferSender *commandBufferChanSender = nullptr;
  // Layout?
  // XR?

  friend class TrContentManager;
  friend class renderer::TrRenderer;
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
  TrContentRuntime *findContent(pid_t pid);
  void disposeContent(TrContentRuntime *content);

private:
  void onRequestEvent(native_event::TrNativeEvent &event);

private:
  TrConstellation *constellation = nullptr;
  mutex contentsMutex;
  vector<TrContentRuntime *> contents;

private: // event channel
  atomic<bool> watcherRunning = false;
  TrOneShotServer<CustomEvent> *eventChanServer = nullptr;
  thread *eventChanWatcher = nullptr;

private: // command buffer channel
  atomic<bool> commandBuffersWorkerRunning = false;
  thread *commandBuffersRecvWorker = nullptr;
  mutex commandBuffersMutex;

  friend class TrContentRuntime;
  friend class TrConstellation;
  friend class TrRenderer;
};
