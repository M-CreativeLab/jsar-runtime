#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <filesystem>

#include "common/classes.hpp"
#include "common/options.hpp"
#include "common/scoped_thread.hpp"
#include "common/ipc.hpp"
#include "common/command_buffers/shared.hpp"
#include "common/command_buffers/sender.hpp"
#include "common/command_buffers/receiver.hpp"
#include "common/command_buffers/command_buffers.hpp"

#include "common/events_v2/event_target.hpp"
#include "common/events_v2/native_event.hpp"
#include "common/events_v2/native_message.hpp"
#include "common/events_v2/native_sender.hpp"
#include "common/events_v2/native_receiver.hpp"
#include "common/media/message.hpp"
#include "common/media/sender.hpp"
#include "common/media/receiver.hpp"
#include "common/xr/message.hpp"
#include "common/xr/sender.hpp"
#include "common/xr/receiver.hpp"

#include "./constellation.hpp"
#include "./hive_daemon.hpp"

using namespace std;
using namespace ipc;

#define INVALID_PID -1

class TrContentManager;
class TrContentRuntime
{
public:
  TrContentRuntime(TrContentManager *manager);
  ~TrContentRuntime();

public:
  /**
   * Pre-start the content, it will start the content and initialize the environment which is not related to the request
   * to boost the responding speed.
   */
  void preStart();
  /**
   * Start a content process with the given initialization options, and starts a command buffer receiver worker.
   */
  void start(TrDocumentRequestInit &init);
  /**
   * Send a `pause` event to the content process.
   */
  void pause();
  /**
   * Send a `resume` event to the content process.
   */
  void resume();
  /**
   * Dispose the content instance.
   *
   * @param waitsForExit If true, it will wait for the client process to exit.
   * @returns null, but it will be blocked util the process is exit when `waitsForExit` is true.
   */
  void dispose(bool waitsForExit = false);

public: // lifecycle which is called by other classes
  /**
   * When the content's command buffers is about to be executed.
   */
  void onCommandBuffersExecuting();
  /**
   * When the content's command buffers are executed. Internally this method will clear the command buffer requests.
   */
  void onCommandBuffersExecuted();
  /**
   * When the content's client process is exited.
   *
   * @param exitCode The exit code of the client process, 0 means normal exit.
   */
  void onClientProcessExited(int exitCode);

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
   * When the content's client is connected to the server side event channel.
   *
   * @param client The event channel client.
   */
  void onEventChanConnected(TrOneShotClient<events_comm::TrNativeEventMessage> &client);
  /**
   * It dispatches the content's event from native side.
   *
   * @param event The native event to dispatch.
   * @returns true if the event is dispatched successfully.
   */
  bool dispatchEvent(events_comm::TrNativeEvent &event);
  /**
   * Report a document event to the host native EventTarget, this is useful for reporting the events from the host process.
   *
   * @param documentEventType The document event type, such as `TrDocumentEventType::SpawnProcess`.
   * @returns true if the event is dispatched successfully.
   */
  inline bool reportDocumentEvent(TrDocumentEventType documentEventType)
  {
    events_comm::TrDocumentEvent detail(id, documentEventType);
    auto event = events_comm::TrNativeEvent::MakeEvent(events_comm::TrNativeEventType::DocumentEvent, &detail);
    return dispatchEvent(event);
  }
  /**
   * It responds to the RPC request for the content, internally it sends a `TrRpcResponse` event to the peer process, namely the
   * content.
   *
   * @param respDetail The RPC response detail.
   * @param requestId The request ID of the RPC request.
   * @returns true if the response is sent successfully.
   */
  bool respondRpcRequest(events_comm::TrRpcResponse &respDetail, uint32_t requestId);
  /**
   * Log the document event for metrics.
   * 
   * @param docEvent The `TrDocumentEvent` reference to log.
   */
  void logDocumentEvent(events_comm::TrDocumentEvent &docEvent);

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
  void recvCommandBuffers(WorkerThread &worker, uint32_t timeout);
  void recvEvent();
  void recvMediaRequest();
  bool recvXRCommand(int timeout = 0);
  bool tryDispatchRequest();
  bool tickOnFrame();

public:
  /**
   * The content id.
   *
   * A content corresponds to documents, user could use a content to browse multiple documents, but the initial document is
   * unique, thus at JSAR, we use the initial document id as the content id and client id.
   *
   * __What's the difference between content and client?__
   *
   * A content is an instance in the host process, it manages the lifecycle of the client process, and the client is the instance
   * at another process that is created via hived, our client process incubator.
   *
   * We use the same id for the content and client both to identify the content/client in the host process and the client process.
   */
  int id = -1;
  /**
   * The OS process id of this content's client process, it will be set asynchronously when the client process is created, and respond
   * via the hived.
   *
   * NOTE: Don't use pid to identify the content related component such as `ContentRenderer`, `Media`, etc, this is because we establish
   * connections asynchronously to speed up the content creation, that causes the pid is unavailable when a content's component such as
   * `ContentRenderer` is going to be created.
   */
  atomic<int> pid = INVALID_PID;

private:
  TrContentManager *contentManager;
  /**
   * The content's initialization options.
   */
  TrDocumentRequestInit requestInit;
  /**
   * The previous received `DocumentEvent` timestamp in milliseconds, this is used to calculate the duration between two events.
   */
  long long prevDocumentEventTime = 0;
  /**
   * The flag `isRequestDispatched` is to control the request dispatching.
   * 
   * Call to start() will reset this flag to false and update the request to dispatch, then the tick function will check this flag to 
   * dispatch the request.
   */
  bool isRequestDispatched = true;
  atomic<bool> used = false;
  atomic<bool> started = false;
  /**
   * The flag `available` is to indicate the content is available for the client process, it's set to true when the client process is
   * started or pre-started.
   */
  atomic<bool> available = false;
  /**
   * The flag `shouldDestroy` is to indicate the content should not be rendered, it's a quick set to skip the rendering of the content.
   */
  atomic<bool> disableRendering = false;
  /**
   * The flag `shouldDestroy` is to indicate the content is going to be destroyed, the content cleanup thread will check this flag to
   * remove the content related resources.
   */
  atomic<bool> shouldDestroy = false;
  mutex exitingMutex;
  condition_variable exitedCv;

private:
  unique_ptr<events_comm::TrNativeEventReceiver> eventChanReceiver = nullptr;
  unique_ptr<events_comm::TrNativeEventSender> eventChanSender = nullptr;
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
  mutex commandBufferRequestsMutex;
  mutex commandBufferExecutingMutex;
  condition_variable commandBufferExecutingCv;
  atomic<bool> isCommandBufferRequestsExecuting = false;

  friend class TrContentManager;
  friend class TrHiveDaemon;
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
  /**
   * Make a new content instance, this doesn't start the content process, just created a `TrContentRuntime` instance and added it
   * to the managed list.
   */
  shared_ptr<TrContentRuntime> makeContent();
  /**
   * Get the content instance by its id.
   * 
   * @param id The content id.
   * @param includePreContent If true, it will return the pre-content instance if the id is matched.
   */
  shared_ptr<TrContentRuntime> getContent(uint32_t id, bool includePreContent = false);
  /**
   * Find the content instance by its client process id.
   * 
   * @param pid The client process id.
   */
  shared_ptr<TrContentRuntime> findContentByPid(pid_t pid);
  /**
   * Dispose the content instance, it will release the related resources and terminate the client process.
   *
   * @param content The content instance to dispose.
   */
  void disposeContent(shared_ptr<TrContentRuntime> content);

private:
  void onNewClientOnEventChan(TrOneShotClient<events_comm::TrNativeEventMessage> &client);
  void onTryDestroyingContents();
  void onRpcRequest(events_comm::TrNativeEvent &event);
  void onDocumentEvent(events_comm::TrNativeEvent &event);

private:
  void installScripts();
  void startHived();
  void preparePreContent();
  void acceptEventChanClients(int timeout = 100);

private:
  TrConstellation *constellation = nullptr;
  shared_mutex contentsMutex;
  vector<shared_ptr<TrContentRuntime>> contents;
  unique_ptr<TrHiveDaemon> hived;

private:  // content listeners
  shared_ptr<events_comm::TrNativeEventListener> rpcRequestListener = nullptr;
  shared_ptr<events_comm::TrNativeEventListener> documentEventListener = nullptr;

private: // pre-content
  bool enablePreContent = true;
  atomic<bool> preContentScheduled = false;
  chrono::time_point<chrono::system_clock> preContentScheduledTimepoint;

private: // channels & workers
  TrOneShotServer<events_comm::TrNativeEventMessage> *eventChanServer = nullptr;
  unique_ptr<WorkerThread> eventChanWatcher;

  friend class TrContentRuntime;
  friend class TrConstellation;
  friend class TrRenderer;
};
