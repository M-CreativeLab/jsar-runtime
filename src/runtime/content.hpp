#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <filesystem>

#include "common/classes.hpp"
#include "common/debug.hpp"
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

#define INVALID_PID -1

// Forward declarations
class TrContentManager;

/**
 * The content runtime is a class that manages the corresponding content process, it's responsible for the lifecycle of the content
 * process, such as starting, pausing, resuming, and disposing.
 */
class TrContentRuntime final : public std::enable_shared_from_this<TrContentRuntime>
{
  friend class TrContentManager;
  friend class TrHiveDaemon;
  friend class TrInspector;
  friend class renderer::TrRenderer;
  friend class renderer::TrContentRenderer;

public:
  /**
   * Create a new content runtime instance.
   *
   * @param manager The content manager instance.
   * @returns The shared pointer of the content runtime.
   */
  static std::shared_ptr<TrContentRuntime> Make(TrContentManager *manager)
  {
    return std::shared_ptr<TrContentRuntime>(new TrContentRuntime(manager), [](TrContentRuntime *content)
                                             { content->release();
                                               delete content; });
  }

public:
  /**
   * Construct a new content runtime with the given content manager.
   *
   * @param manager The content manager instance.
   */
  TrContentRuntime(TrContentManager *manager);
  ~TrContentRuntime() = default;

public:
  /**
   * Pre-start the content, it will start the content and initialize the environment which is not related to the request
   * to boost the responding speed.
   */
  void preStart();
  /**
   * Start a content process with the given initialization options, and starts a command buffer receiver worker.
   *
   * @param init The initialization options for the content.
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
  /**
   * @returns the constellation instance.
   */
  TrConstellation *getConstellation();
  /**
   * @returns the WebXR device instance.
   */
  xr::Device *getXrDevice();

public: // command buffer methods
  void setupWithCommandBufferClient(ipc::TrOneShotClient<TrCommandBufferMessage> *client);
  bool sendCommandBufferResponse(TrCommandBufferResponse &res);

public: // event methods
  /**
   * When the content's client is connected to the server side event channel.
   *
   * @param client The event channel client.
   */
  void onEventChanConnected(ipc::TrOneShotClient<events_comm::TrNativeEventMessage> &client);
  /**
   * It dispatches the content's event from native side.
   *
   * @param event The native event to dispatch.
   * @returns true if the event is dispatched successfully.
   */
  bool dispatchEvent(std::shared_ptr<events_comm::TrNativeEvent> event);
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

public: // WebXR methods
  /**
   * When the content's client is connected to the server-side WebXR command channel.
   *
   * @param client The channel client for the WebXR command.
   */
  void onXRCommandChanConnected(TrOneShotClient<xr::TrXRCommandMessage> &client);
  /**
   * It responds a WebXR command response.
   *
   * @tparam CommandType The XR command type.
   * @param resp The XR command response to send.
   * @returns true if the response is sent successfully.
   */
  template <typename CommandType>
  inline bool sendXRCommandResponse(xr::TrXRCommandBase<CommandType> &resp)
  {
    return (xrCommandChanSender != nullptr)
             ? xrCommandChanSender->sendCommand(resp)
             : false;
  }
  /**
   * @returns a `std::vector` of the `XRSession` instances, which includes all the WebXR sessions by the content.
   */
  inline std::vector<xr::TrXRSession *> &getXRSessions()
  {
    return xrSessionsStack;
  }
  /**
   * Get the current active XRSession, it means the top of the sessions stack.
   *
   * @returns a pointer to the active XRSession, or nullptr if there is no active session.
   */
  xr::TrXRSession *getActiveXRSession();
  /**
   * Append a WebXR session to the content's XR sessions stack.
   *
   * @param session The `xr::XRSession` pointer to append.
   */
  void appendXRSession(xr::TrXRSession *session);
  /**
   * Remove the WebXR session from the content's XR sessions stack.
   *
   * @param session The `xr::XRSession` pointer to remove.
   * @returns true if the session is removed successfully.
   */
  bool removeXRSession(xr::TrXRSession *session);

private:
  void recvCommandBuffers(WorkerThread &worker, uint32_t timeout);
  void recvEvent();
  void recvMediaRequest();
  bool recvXRCommand(int timeout = 0);
  bool tryDispatchRequest();
  bool tickOnFrame();
  void release();

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
  std::atomic<int> pid = INVALID_PID;

private:
  /**
   * The content manager.
   */
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
  /**
   * The flag `used` is to indicate the content is used, each pre-started content would be marked as not used (`false`), only the call
   * to `start()` will set this flag to `true`.
   *
   * This flag will be used to filter the content that is used or not used.
   */
  std::atomic<bool> used = false;
  /**
   * The flag `started` is to indicate the content is started, it's set to true when the client process is started.
   */
  std::atomic<bool> started = false;
  /**
   * The flag `available` is to indicate the content is available for the client process, it's set to true when the client process is
   * started or pre-started.
   */
  std::atomic<bool> available = false;
  /**
   * The flag `shouldDestroy` is to indicate the content should not be rendered, it's a quick set to skip the rendering of the content.
   */
  std::atomic<bool> disableRendering = false;
  /**
   * The flag `shouldDestroy` is to indicate the content is going to be destroyed, the content cleanup thread will check this flag to
   * remove the content related resources.
   */
  std::atomic<bool> shouldDestroy = false;
  std::mutex exitingMutex;
  std::condition_variable exitedCv;

private:
  std::unique_ptr<events_comm::TrNativeEventReceiver> eventChanReceiver = nullptr;
  std::unique_ptr<events_comm::TrNativeEventSender> eventChanSender = nullptr;
  std::unique_ptr<media_comm::TrMediaCommandReceiver> mediaChanReceiver = nullptr;
  std::unique_ptr<media_comm::TrMediaCommandSender> mediaChanSender = nullptr;
  std::unique_ptr<TrCommandBufferReceiver> commandBufferChanReceiver = nullptr;
  std::unique_ptr<TrCommandBufferSender> commandBufferChanSender = nullptr;
  ipc::TrOneShotClient<TrCommandBufferMessage> *commandBufferChanClient = nullptr;
  std::unique_ptr<WorkerThread> commandBuffersRecvWorker;

private: // XR fields
  ipc::TrOneShotClient<xr::TrXRCommandMessage> *xrCommandChanClient = nullptr;
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
  std::vector<xr::TrXRSession *> xrSessionsStack;

private:
  std::mutex commandBufferRequestsMutex;
  std::mutex commandBufferExecutingMutex;
  std::condition_variable commandBufferExecutingCv;
  std::atomic<bool> isCommandBufferRequestsExecuting = false;
};
