#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <idgen.hpp>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "./content_manager.hpp"
#include "./embedder.hpp"
#include "./content.hpp"
#include "./media_manager.hpp"

using namespace std;

TrContentRuntime::TrContentRuntime(TrContentManager *contentMgr) : contentManager(contentMgr)
{
  static TrIdGenerator idGen(0x100);
  id = idGen.get();
}

void TrContentRuntime::preStart()
{
  available = true;
  commandBuffersRecvWorker = make_unique<WorkerThread>("TrCBWorker", [this](WorkerThread &worker)
                                                       { recvCommandBuffers(worker, 100); });
  auto renderer = contentManager->constellation->renderer;
  renderer->addContentRenderer(shared_from_this());

  // Send the create process request to the hive daemon.
  TrDocumentRequestInit init;
  init.id = id;
  contentManager->hived->createClient(init, [this](pid_t pid)
                                      { this->pid = pid; });
  reportDocumentEvent(TrDocumentEventType::SpawnProcess);
}

void TrContentRuntime::start(TrDocumentRequestInit &init)
{
  if (!available) // PreStart if the process is not available.
    preStart();

  started = true;
  requestInit = init;
  requestInit.id = id;
  isRequestDispatched = false;
  reportDocumentEvent(TrDocumentEventType::DispatchRequest);
  tryDispatchRequest();
}

void TrContentRuntime::pause()
{
  // TODO
}

void TrContentRuntime::resume()
{
  // TODO
}

void TrContentRuntime::dispose(bool waitsForExit)
{
  assert(pid != 0);
  if (pid == INVALID_PID) // The process is not started or exited.
    return;

  available = false;
  disableRendering = true;
  contentManager->constellation->mediaManager->removeSoundSourcesByContent(id); // Remove the sound sources.
  contentManager->hived->terminateClient(id);
  if (waitsForExit)
  {
    DEBUG(LOG_TAG_CONTENT, "Waiting for the content(%d) to exit...", id);
    unique_lock<mutex> lock(exitingMutex);
    exitedCv.wait(lock, [this]
                  { return pid == INVALID_PID; });
  }
}

void TrContentRuntime::onCommandBuffersExecuting()
{
  isCommandBufferRequestsExecuting.store(true);
}

void TrContentRuntime::onCommandBuffersExecuted()
{
  isCommandBufferRequestsExecuting.store(false);
  commandBufferExecutingCv.notify_all();
}

void TrContentRuntime::onClientProcessExited(int exitCode)
{
  available = false; // make sure this state is still false.
  pid = INVALID_PID;
  shouldDestroy = true;
  exitedCv.notify_all(); // No need to use the mutex because the states are atomic.
}

TrConstellation *TrContentRuntime::getConstellation()
{
  return contentManager->constellation;
}

xr::Device *TrContentRuntime::getXrDevice()
{
  return contentManager->constellation->xrDevice.get();
}

void TrContentRuntime::setCommandBufferRequestHandler(function<void(TrCommandBufferBase *)> handler)
{
  onCommandBufferRequestReceived = handler;
}

void TrContentRuntime::resetCommandBufferRequestHandler()
{
  onCommandBufferRequestReceived = nullptr;
}

void TrContentRuntime::setupWithCommandBufferClient(TrOneShotClient<TrCommandBufferMessage> *client)
{
  assert(client != nullptr);
  commandBufferChanReceiver = std::make_unique<TrCommandBufferReceiver>(client);
  commandBufferChanSender = std::make_unique<TrCommandBufferSender>(client);
  commandBufferChanClient = client;
  DEBUG(LOG_TAG_CONTENT, "Setup the command buffer channel with client(%d, %d)", client->getPid(), id);
}

bool TrContentRuntime::sendCommandBufferResponse(TrCommandBufferResponse &res)
{
  if (TR_UNLIKELY(!available || shouldDestroy || commandBufferChanSender == nullptr))
    return false;
  else
    return commandBufferChanSender->sendCommandBufferResponse(res);
}

void TrContentRuntime::onEventChanConnected(TrOneShotClient<events_comm::TrNativeEventMessage> &client)
{
  eventChanReceiver = make_unique<events_comm::TrNativeEventReceiver>(&client);
  eventChanSender = make_unique<events_comm::TrNativeEventSender>(&client);
}

bool TrContentRuntime::dispatchEvent(std::shared_ptr<events_comm::TrNativeEvent> event)
{
  return getConstellation()->nativeEventTarget->dispatchEvent(event);
}

bool TrContentRuntime::respondRpcRequest(events_comm::TrRpcResponse &respDetail, uint32_t requestId)
{
  if (TR_UNLIKELY(!available || shouldDestroy || eventChanSender == nullptr))
    return false;
  auto eventToDispatch = events_comm::TrNativeEvent::MakeEvent(events_comm::TrNativeEventType::RpcResponse, &respDetail);
  return eventChanSender->dispatchEvent(eventToDispatch, requestId);
}

void TrContentRuntime::logDocumentEvent(events_comm::TrDocumentEvent &docEvent)
{
  int duration = 0;
  if (prevDocumentEventTime != 0)
    duration = docEvent.timestamp - prevDocumentEventTime;
  prevDocumentEventTime = docEvent.timestamp;
  DEBUG(LOG_TAG_METRICS, "content#%d received DocumentEvent(\"%s\") at %zu (+%dms)",
        docEvent.documentId,
        docEvent.toString().c_str(),
        docEvent.timestamp,
        duration);
}

void TrContentRuntime::onMediaChanConnected(TrOneShotClient<media_comm::TrMediaCommandMessage> &client)
{
  if (mediaChanReceiver != nullptr || mediaChanSender != nullptr)
  {
    DEBUG(LOG_TAG_CONTENT, "The media channel connection is already established.");
    return;
  }
  mediaChanReceiver = make_unique<media_comm::TrMediaCommandReceiver>(&client);
  mediaChanSender = make_unique<media_comm::TrMediaCommandSender>(&client);
  tryDispatchRequest();
}

bool TrContentRuntime::dispatchMediaEvent(media_comm::TrMediaCommandBase &event)
{
  if (TR_UNLIKELY(!available || shouldDestroy || mediaChanSender == nullptr))
    return false;
  return mediaChanSender->sendCommand(event);
}

void TrContentRuntime::onXRCommandChanConnected(TrOneShotClient<xr::TrXRCommandMessage> &client)
{
  xrCommandChanReceiver = new xr::TrXRCommandReceiver(&client);
  xrCommandChanSender = new xr::TrXRCommandSender(&client);
  xrCommandChanClient = &client;
}

xr::TrXRSession *TrContentRuntime::getActiveXRSession()
{
  if (xrSessionsStack.empty())
    return nullptr;
  return xrSessionsStack.back();
}

void TrContentRuntime::appendXRSession(xr::TrXRSession *session)
{
  if (session != nullptr)
    xrSessionsStack.push_back(session);
}

bool TrContentRuntime::removeXRSession(xr::TrXRSession *session)
{
  if (session == nullptr)
    return false;
  auto it = std::find(xrSessionsStack.begin(), xrSessionsStack.end(), session);
  if (it != xrSessionsStack.end())
  {
    xrSessionsStack.erase(it);
    return true;
  }
  return false;
}

void TrContentRuntime::recvCommandBuffers(WorkerThread &worker, uint32_t timeout)
{
  /**
   * When the following conditions are met, the worker thread will sleep and wait for the next frame tick:
   *
   * 1. The command buffer channel receiver is not ready.
   * 2. The command buffer requests are executing.
   */
  if (commandBufferChanReceiver == nullptr)
  {
    worker.sleep();
    return;
  }
  if (isCommandBufferRequestsExecuting.load())
  {
    /**
     * When the executing is true, just wait for the execution is finished via the condition variable.
     */
    unique_lock<mutex> lock(commandBufferExecutingMutex);
    commandBufferExecutingCv.wait(lock, [this]
                                  { return !isCommandBufferRequestsExecuting.load(); });
  }

  // Do the command buffer receving work.
  auto commandBuffer = commandBufferChanReceiver->recvCommandBufferRequest(timeout);
  if (commandBuffer != nullptr)
  {
    lock_guard<mutex> lock(commandBufferRequestsMutex);
    if (onCommandBufferRequestReceived)
      onCommandBufferRequestReceived(commandBuffer);
    else
      DEBUG(LOG_TAG_CONTENT, "No command buffer request handler for the content(%d)", id);
  }
}

void TrContentRuntime::recvEvent()
{
  if (TR_UNLIKELY(!available || shouldDestroy || eventChanReceiver == nullptr))
    return;

  auto eventTarget = contentManager->constellation->nativeEventTarget;
  assert(eventTarget != nullptr);

  events_comm::TrNativeEventMessage eventMessage;
  if (eventChanReceiver->recvEventOn(eventMessage, 0))
  {
    switch (eventMessage.getType())
    {
#define CASE(eventType)                                                                                                       \
  case events_comm::TrNativeEventType::eventType:                                                                             \
  {                                                                                                                           \
    auto sharedEvent = events_comm::TrSharedNativeEventBase::FromMessage<events_comm::Tr##eventType##Remote>(eventMessage);   \
    auto eventToDispatch = events_comm::TrNativeEvent::MakeEventWithString(sharedEvent.type, sharedEvent.detailJson.c_str()); \
    eventToDispatch->id = sharedEvent.eventId;                                                                                \
    eventTarget->dispatchEvent(eventToDispatch);                                                                              \
    break;                                                                                                                    \
  }
      CASE(RpcRequest)
      CASE(RpcResponse)
      CASE(DocumentEvent)
#undef CASE
    default:
      break;
    }
  }
}

void TrContentRuntime::recvMediaRequest()
{
  if (TR_UNLIKELY(!available || shouldDestroy || mediaChanReceiver == nullptr))
    return;

  media_comm::TrMediaCommandMessage mediaMessage;
  if (mediaChanReceiver->recvCommand(mediaMessage, 0))
  {
    auto mediaManager = getConstellation()->mediaManager;
    mediaManager->onContentRequest(shared_from_this(), mediaMessage);
  }
}

bool TrContentRuntime::recvXRCommand(int timeout)
{
  auto xrDevice = getConstellation()->xrDevice;
  if (TR_UNLIKELY(xrCommandChanReceiver == nullptr || xrDevice == nullptr || !xrDevice->enabled()))
    return false;

  auto message = xrCommandChanReceiver->recvCommandMessage(timeout);
  if (message != nullptr)
  {
    // NOTE: Don't expose the content reference to the XR handler.
    xrDevice->handleCommandMessage(*message, shared_from_this());
    delete message;
    return true;
  }
  else
  {
    return false;
  }
}

bool TrContentRuntime::tryDispatchRequest()
{
  if (isRequestDispatched || eventChanSender == nullptr)
    return false;

  events_comm::TrDocumentRequest request(requestInit);
  auto requestEvent = events_comm::TrNativeEvent::MakeEvent(events_comm::TrNativeEventType::DocumentRequest, &request);
  if (eventChanSender->dispatchEvent(requestEvent))
  {
    isRequestDispatched = true;
    return true;
  }
  else
  {
    return false;
  }
}

bool TrContentRuntime::tickOnFrame()
{
  recvEvent();
  recvXRCommand();
  recvMediaRequest();
  return true;
}

void TrContentRuntime::release()
{
  std::cout << "Releasing the content runtime(" << id << ")" << std::endl;
  auto constellation = getConstellation();

  // Stopping the receiver worker.
  commandBuffersRecvWorker->stop();

  // Removing the content renderer and command buffer client.
  auto renderer = constellation->renderer;
  if (renderer != nullptr)
  {
    renderer->removeContentRenderer(id);
    renderer->removeCommandBufferChanClient(commandBufferChanClient);
  }

  // Removing the related XR sessions.
  auto xrDevice = constellation->xrDevice;
  for (auto session : xrSessionsStack)
  {
    if (session != nullptr)
      xrDevice->endAndRemoveSession(session);
  }
  xrSessionsStack.clear();
  DEBUG(LOG_TAG_CONTENT, "The content runtime(%d) has been destroyed", id);
}
