#pragma once

#include <atomic>
#include <mutex>
#include <memory>
#include <shared_mutex>
#include <filesystem>

#include "common/classes.hpp"
#include "common/scoped_thread.hpp"
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
#include "common/media/message.hpp"
#include "common/media/sender.hpp"
#include "common/media/receiver.hpp"
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
  /**
   * Start a content process with the given initialization options, and starts a command buffer receiver worker.
   */
  void start(TrXSMLRequestInit init);
  /**
   * Send a `pause` event to the content process.
   */
  void pause();
  /**
   * Send a `resume` event to the content process.
   */
  void resume();
  /**
   * Terminate the content, it will remove the content renderer and send a kill signal to the content process.
   */
  void terminate();
  /**
   * Dispose the content, it firstly call `terminate()` and wait for the content process to exit.
   */
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
  /**
   * It dispatches the content's event from native side.
   */
  bool dispatchEvent(TrEvent &event);
  /**
   * It dispatches a XSMLEvent with the given event type as the content's event.
   */
  inline bool dispatchXSMLEvent(TrXSMLEventType eventType)
  {
    auto event = TrEvent::MakeXSMLEvent(TrXSMLEvent(id, eventType));
    return dispatchEvent(event);
  }
  /**
   * It sends the event response to client side.
   */
  bool sendEventResponse(TrEvent &event);

public: // media methods
  /**
   * When the content's client is connected to the server side media channel.
   * 
   * @param client The media channel client.
   */
  void onMediaChanConnected(TrOneShotClient<media_comm::TrMediaCommandMessage> &client);
  /**
   * Dispatch the media event to the content.
   * 
   * @param event The media event to dispatch.
   * @returns true if the event is dispatched successfully.
   */
  bool dispatchMediaEvent(media_comm::TrMediaCommandBase &event);

public: // xr methods
  /**
   * When the content's client is connected to the server side XR command channel.
   */
  void onXRCommandChanConnected(TrOneShotClient<xr::TrXRCommandMessage> &client);
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
  /**
   * Get the current active XRSession, it means the top of the sessions stack.
   * 
   * @returns a pointer to the active XRSession, or nullptr if there is no active session.
   */
  xr::TrXRSession *getActiveXRSession();
  /**
   * Append a XRSession to the content's XR sessions stack.
   */
  void appendXRSession(xr::TrXRSession *session);
  /**
   * Remove the XRSession from the content's XR sessions stack.
   */
  bool removeXRSession(xr::TrXRSession *session);

private:
  void onClientProcess();
  bool testClientProcessExitOnFrame(); // true if the client process has exited
  void recvCommandBuffers(WorkerThread &worker, uint32_t timeout);
  void recvEvent();
  void recvMediaRequest();
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
  unique_ptr<TrEventReceiver> eventChanReceiver = nullptr;
  unique_ptr<TrEventSender> eventChanSender = nullptr;
  unique_ptr<media_comm::TrMediaCommandReceiver> mediaChanReceiver = nullptr;
  unique_ptr<media_comm::TrMediaCommandSender> mediaChanSender = nullptr;
  unique_ptr<TrCommandBufferReceiver> commandBufferChanReceiver = nullptr;
  unique_ptr<TrCommandBufferSender> commandBufferChanSender = nullptr;
  TrOneShotClient<TrCommandBufferMessage> *commandBufferChanClient = nullptr;
  unique_ptr<WorkerThread> commandBuffersRecvWorker;
  function<void(TrCommandBufferBase *)> onCommandBufferRequestReceived;

private: // XR fields
  TrOneShotClient<xr::TrXRCommandMessage> *xrCommandChanClient = nullptr;
  xr::TrXRCommandReceiver *xrCommandChanReceiver = nullptr;
  xr::TrXRCommandSender *xrCommandChanSender = nullptr;
  /**
   * **Why we need to store the XRSession instances?**
   *
   * In WebXR, the `XRSession` is requested by the `XRSystem` and it decides if a new session should be created and to be
   * activated. At traditional Web browsers, a page is corresponding to a window, it means that immersive-vr content is only
   * available for a XRSession at a time, however multiple immersive-ar contents could be available at the same time.
   *
   * But in the JSAR environment, a page(XSML/HTML) is not just a window, but a volumetric space, it means that we could restrict
   * the XRSession with `immersive-ar` to be one-per-page, such that we could make use of spatialized information for other resources
   * which is not created with WebXR Device APIs, for example, we could treat a common <audio> element or `new Audio(url)` as a
   * spatialized sound source.
   *
   * See https://developer.mozilla.org/en-US/docs/Web/API/XRSystem/requestSession for more detailed background information.
   *
   * **Design**
   *
   * Each content would have a stack for storing the XRSession instances, and the top of this stack is the active XRSession.
   * When a new `XRSession` is requested, it will be pushed to the stack, and use the new session to render the visual content,
   * and when this session is ended via (e.g. `end()` method), it will be popped from the stack, and the previous session will be
   * activated.
   */
  vector<xr::TrXRSession *> xrSessionsStack;

private:
  int childPipes[2];
  string lastClientOutput;

private:
  mutex commandBufferRequestsMutex;
  mutex commandBufferExecutingMutex;
  condition_variable commandBufferExecutingCv;
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
  bool hasContents() { return !contents.empty(); }
  TrContentRuntime *makeContent();
  TrContentRuntime *findContent(pid_t pid);
  void disposeContent(TrContentRuntime *content);

private:
  void onRequestEvent(TrEvent &event);
  void onRecvXrCommands(int timeout = 100);
  void onNewEventChan();

private:
  void installScripts();

private:
  TrConstellation *constellation = nullptr;
  shared_mutex contentsMutex;
  vector<TrContentRuntime *> contents;

private: // channels & workers
  TrOneShotServer<TrEventMessage> *eventChanServer = nullptr;
  std::unique_ptr<WorkerThread> contentsDestroyingWorker;
  std::unique_ptr<WorkerThread> eventChanWatcher;
  std::unique_ptr<WorkerThread> xrCommandsRecvWorker;

  friend class TrContentRuntime;
  friend class TrConstellation;
  friend class TrRenderer;
};
