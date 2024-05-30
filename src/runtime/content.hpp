#pragma once

#include <atomic>
#include <mutex>
#include <filesystem>

#include "common/classes.hpp"
#include "common/ipc.hpp"
#include "common/command_buffers/message.hpp"
#include "common/command_buffers/sender.hpp"
#include "common/command_buffers/receiver.hpp"
#include "common/command_buffers/command_buffers.hpp"

#include "common/events/event_type.hpp"
#include "common/events/event_target.hpp"
#include "common/events/message.hpp"
#include "common/events/sender.hpp"
#include "common/events/receiver.hpp"
#include "constellation.hpp"

using namespace std;
using namespace ipc;
using namespace events;

class TrContentManager;
class TrContentRuntime
{
public:
  TrContentRuntime(TrContentManager *manager);
  ~TrContentRuntime();

public:
  void start(TrXSMLRequestInit init);
  void pause();
  void resume();
  void terminate();
  void dispose();

public: // lifecycle which is called by other classes
  /**
   * When the content's command buffers are executed. Internally this method will clear the command buffer requests.
   */
  void onCommandBuffersExecuted();

public: // reference methods
  TrConstellation *getConstellation();
  xr::Device *getXrDevice();

public: // command buffer methods
  void setupWithCommandBufferClient(TrOneShotClient<TrCommandBufferMessage> *client);
  bool sendCommandBufferResponse(TrCommandBufferResponse &res);

private:
  void onClientProcess();
  bool testClientProcessExitOnFrame(); // true if the client process has exited
  void recvCommandBuffers(uint32_t timeout);
  bool tickOnFrame();

public:
  pid_t pid = -1;

private:
  int eventChanPort;
  int frameChanPort;
  int commandBufferChanPort;
  TrXSMLRequestInit requestInit;
  TrConstellationInit constellationOptions;
  TrContentManager *contentManager;
  TrEventReceiver *eventChanReceiver = nullptr;
  TrEventSender *eventChanSender = nullptr;
  TrCommandBufferReceiver *commandBufferChanReceiver = nullptr;
  TrCommandBufferSender *commandBufferChanSender = nullptr;
  vector<TrCommandBufferBase *> commandBufferRequests;
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
  void onRequestEvent(TrEvent &event);

private:
  TrConstellation *constellation = nullptr;
  mutex contentsMutex;
  vector<TrContentRuntime *> contents;

private: // event channel
  atomic<bool> watcherRunning = false;
  TrOneShotServer<TrEventMessage> *eventChanServer = nullptr;
  thread *eventChanWatcher = nullptr;

private: // command buffer channel
  atomic<bool> commandBuffersWorkerRunning = false;
  thread *commandBuffersRecvWorker = nullptr;
  mutex commandBuffersMutex;

  friend class TrContentRuntime;
  friend class TrConstellation;
  friend class TrRenderer;
};
