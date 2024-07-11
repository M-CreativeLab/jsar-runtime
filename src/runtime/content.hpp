#pragma once

#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <filesystem>

#include "common/classes.hpp"
#include "common/ipc.hpp"
#include "common/command_buffers/shared.hpp"
#include "common/command_buffers/sender.hpp"
#include "common/command_buffers/receiver.hpp"
#include "common/command_buffers/command_buffers.hpp"

#include "common/events/event_type.hpp"
#include "common/events/event_target.hpp"
#include "common/events/message.hpp"
#include "common/events/sender.hpp"
#include "common/events/receiver.hpp"
#include "common/xr/message.hpp"
#include "common/xr/sender.hpp"
#include "common/xr/receiver.hpp"
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
   * When the content's command buffers is about to be executed.
   */
  void onCommandBuffersExecuting();
  /**
   * When the content's command buffers are executed. Internally this method will clear the command buffer requests.
   */
  void onCommandBuffersExecuted();

public: // reference methods
  TrConstellation *getConstellation();
  xr::Device *getXrDevice();

public: // command buffer methods
  void setCommandBufferRequestHandler(function<void(TrCommandBufferBase *)> handler);
  void resetCommandBufferRequestHandler();
  void setupWithCommandBufferClient(TrOneShotClient<TrCommandBufferMessage> *client);
  bool sendCommandBufferResponse(TrCommandBufferResponse &res);

public: // event methods
  bool sendEventResponse(TrEvent &event);

public: // XR-related methods
  /**
   * Setup and add the ipc client for XR command messages.
   *
   * @param client The client pointer to receive or send XR command messages.
   */
  void setupWithXRCommandBufferClient(TrOneShotClient<xr::TrXRCommandMessage> *client);
  /**
   * Send a XR command response to the content's client.
   *
   * @param resp The XR command response to send.
   */
  template <typename CommandType>
  bool sendXRCommandResponse(xr::TrXRCommandBase<CommandType> &resp)
  {
    if (xrCommandChanSender != nullptr)
      return xrCommandChanSender->sendCommand(resp);
    return false;
  }

private:
  void onClientProcess();
  bool testClientProcessExitOnFrame(); // true if the client process has exited
  void recvCommandBuffers(uint32_t timeout);
  void recvEvent();
  bool recvClientOutput();
  bool tickOnFrame();

public:
  pid_t pid = -1;
  int id = -1;

private:
  int eventChanPort;
  int frameChanPort;
  int commandBufferChanPort;
  TrXSMLRequestInit requestInit;
  TrConstellationInit constellationOptions;
  TrContentManager *contentManager;
  atomic<bool> shouldDestroy = false;

private:
  TrEventReceiver *eventChanReceiver = nullptr;
  TrEventSender *eventChanSender = nullptr;
  TrOneShotClient<TrCommandBufferMessage> *commandBufferChanClient = nullptr;
  TrCommandBufferReceiver *commandBufferChanReceiver = nullptr;
  TrCommandBufferSender *commandBufferChanSender = nullptr;
  std::unique_ptr<thread> commandBuffersRecvWorker;
  atomic<bool> commandBuffersWorkerRunning = false;

  function<void(TrCommandBufferBase *)> onCommandBufferRequestReceived;
  TrOneShotClient<xr::TrXRCommandMessage> *xrCommandChanClient = nullptr;
  xr::TrXRCommandReceiver *xrCommandChanReceiver = nullptr;
  xr::TrXRCommandSender *xrCommandChanSender = nullptr;

private:
  int childPipes[2];
  string lastClientOutput;

private:
  mutex recvCommandBuffersMutex;
  mutex commandBufferRequestsMutex;
  atomic<bool> isCommandBufferRequestsExecuting = false;

  friend class TrContentManager;
  friend class renderer::TrRenderer;
  friend class renderer::TrContentRenderer;
};

/**
 * A `TrContentManager` is to manage the lifecycle of content instances, that is, to create, run, and dispose of JavaScript runtime
 * environments.
 */
class TrContentManager
{
public:
  TrContentManager(TrConstellation *constellation);
  ~TrContentManager();

public:
  bool initialize();
  bool shutdown();
  bool tickOnFrame();
  TrContentRuntime *makeContent();
  TrContentRuntime *findContent(pid_t pid);
  void disposeContent(TrContentRuntime *content);

private:
  void onRequestEvent(TrEvent &event);
  void onRecvXrCommands(int timeout = 100);

private:
  TrConstellation *constellation = nullptr;
  shared_mutex contentsMutex;
  vector<TrContentRuntime *> contents;
  std::unique_ptr<thread> contentsDestroyingWorker;
  atomic<bool> contentsDestroyingWorkerRunning = false;

private: // event channel
  TrOneShotServer<TrEventMessage> *eventChanServer = nullptr;
  atomic<bool> watcherRunning = false;
  std::unique_ptr<thread> eventChanWatcher;

private: // XR command channel
  atomic<bool> xrCommandsWorkerRunning = false;
  std::unique_ptr<thread> xrCommandsRecvWorker;

  friend class TrContentRuntime;
  friend class TrConstellation;
  friend class TrRenderer;
};
