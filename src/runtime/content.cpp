#include <iostream>
#include <filesystem>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "idgen.hpp"
#include "debug.hpp"
#include "embedder.hpp"
#include "content.hpp"
#include "media_manager.hpp"
#include "crates/jsar_jsbundle.h"

TrContentRuntime::TrContentRuntime(TrContentManager *contentMgr) : contentManager(contentMgr)
{
  static TrIdGenerator idGen(0x100);
  id = idGen.get();

  auto eventTarget = contentManager->constellation->nativeEventTarget;
  assert(eventTarget != nullptr);

  eventTarget->addEventListener(events_comm::TrNativeEventType::RpcRequest, [this](auto type, auto &event)
                                { this->getConstellation()->dispatchNativeEvent(event, this); });
  eventTarget->addEventListener(events_comm::TrNativeEventType::DocumentEvent, [this](auto type, auto &event)
                                { this->getConstellation()->dispatchNativeEvent(event, this); });
}

TrContentRuntime::~TrContentRuntime()
{
  auto constellation = getConstellation();

  // Stopping the receiver worker.
  commandBuffersRecvWorker->stop();

  // Removing the content renderer and command buffer client.
  auto renderer = constellation->renderer;
  if (renderer != nullptr)
  {
    renderer->removeContentRenderer(this);
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
}

void TrContentRuntime::preStart()
{
  available = true;
  commandBuffersRecvWorker = std::make_unique<WorkerThread>("TrCommandBuffersWorker", [this](WorkerThread &worker)
                                                            { recvCommandBuffers(worker, 100); });
  auto renderer = contentManager->constellation->renderer;
  renderer->addContentRenderer(this);

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
  contentManager->hived->terminateClient(id);
  if (waitsForExit)
  {
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
  if (!shouldDestroy && commandBufferChanSender != nullptr)
    return commandBufferChanSender->sendCommandBufferResponse(res);
  else
    return false;
}

void TrContentRuntime::onEventChanConnected(TrOneShotClient<events_comm::TrNativeEventMessage> &client)
{
  eventChanReceiver = make_unique<events_comm::TrNativeEventReceiver>(&client);
  eventChanSender = make_unique<events_comm::TrNativeEventSender>(&client);
}

bool TrContentRuntime::dispatchEvent(events_comm::TrNativeEvent &event)
{
  return getConstellation()->nativeEventTarget->dispatchEvent(event);
}

bool TrContentRuntime::respondRpcRequest(events_comm::TrRpcResponse &respDetail, uint32_t requestId)
{
  if (shouldDestroy || eventChanSender == nullptr)
    return false;

  auto eventToDispatch = events_comm::TrNativeEvent::MakeEvent(events_comm::TrNativeEventType::RpcResponse, &respDetail);
  return eventChanSender->dispatchEvent(eventToDispatch, requestId);
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
  if (shouldDestroy || mediaChanSender == nullptr)
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
  if (shouldDestroy || eventChanReceiver == nullptr)
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
    eventToDispatch.id = sharedEvent.eventId;                                                                                 \
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
  if (TR_UNLIKELY(shouldDestroy || mediaChanReceiver == nullptr))
    return;

  media_comm::TrMediaCommandMessage mediaMessage;
  if (mediaChanReceiver->recvCommand(mediaMessage, 0))
  {
    auto mediaManager = getConstellation()->mediaManager;
    mediaManager->onContentRequest(this, mediaMessage);
  }
}

void TrContentRuntime::recvXRCommand(int timeout)
{
  auto xrDevice = getConstellation()->xrDevice;
  if (xrCommandChanReceiver == nullptr || xrDevice == nullptr || !xrDevice->enabled())
    return;

  auto message = xrCommandChanReceiver->recvCommandMessage(timeout);
  if (message != nullptr)
  {
    // NOTE: Don't expose the content reference to the XR handler.
    xrDevice->handleCommandMessage(*message, this);
    delete message;
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
  recvMediaRequest();
  return true;
}

TrContentManager::TrContentManager(TrConstellation *constellation)
    : constellation(constellation),
      hived(make_unique<TrHiveDaemon>(constellation))
{
  eventChanServer = new TrOneShotServer<events_comm::TrNativeEventMessage>("eventChan");
}

TrContentManager::~TrContentManager()
{
  if (eventChanServer != nullptr)
  {
    delete eventChanServer;
    eventChanServer = nullptr;
  }
}

bool TrContentManager::initialize()
{
  installScripts();

  eventChanWatcher = std::make_unique<WorkerThread>("TrEventChanWatcher", [this](WorkerThread &)
                                                    { acceptEventChanClients(); });
  xrCommandsRecvWorker = std::make_unique<WorkerThread>("TrXRCommandsWorker", [this](WorkerThread &worker)
                                                        { recvXRCommands(); worker.sleep(); }, 100);
  contentsDestroyingWorker = std::make_unique<WorkerThread>("TrContentsMgr", [this](WorkerThread &worker)
                                                            { onTryDestroyingContents(); worker.sleep(); }, 1000);
  return true;
}

bool TrContentManager::shutdown()
{
  auto contentsCount = contents.size();
  {
    shared_lock<shared_mutex> lock(contentsMutex);
    for (auto content : contents)
      content->dispose();
  }
  while (true)
  {
    this_thread::sleep_for(chrono::milliseconds(100));
    tickOnFrame();
    if (contents.empty())
      break;
  }
  contentsDestroyingWorker->stop();
  DEBUG(LOG_TAG_CONTENT, "All contents(%zu) has been disposed and removed", contentsCount);

  eventChanWatcher->stop();
  xrCommandsRecvWorker->stop();
  hived->shutdown();
  DEBUG(LOG_TAG_CONTENT, "TrContentManager::shutdown() done.");
  return true;
}

bool TrContentManager::tickOnFrame()
{
  hived->tick();

  // When the hive daemon is ready, we need to make sure the pre-content is always ready.
  if (hived->daemonReady)
    preparePreContent();

  if (isPreContentSet)
  {
    shared_lock<shared_mutex> lock(preContentMutex);
    // Tick the pre-content runtime.
    assert(preContent != nullptr);
    preContent->tickOnFrame();
  }
  {
    // Check the status of each content runtime.
    shared_lock<shared_mutex> lock(contentsMutex);
    for (auto content : contents)
    {
      if (content->available)
        content->tickOnFrame();
    }
  }
  return true;
}

shared_ptr<TrContentRuntime> TrContentManager::makeContent()
{
  preparePreContent();

  shared_ptr<TrContentRuntime> content = nullptr;
  {
    unique_lock<shared_mutex> lock(preContentMutex);
    content = preContent;
    preContent.reset();
    isPreContentSet = false;
  }
  {
    unique_lock<shared_mutex> lock(contentsMutex);
    contents.push_back(content);
  }
  return content;
}

shared_ptr<TrContentRuntime> TrContentManager::getContent(uint32_t id, bool includePreContent)
{
  if (includePreContent)
  {
    shared_lock<shared_mutex> lock(preContentMutex);
    if (preContent != nullptr && preContent->id == id)
      return preContent;
  }
  {
    shared_lock<shared_mutex> lock(contentsMutex);
    for (auto it = contents.begin(); it != contents.end(); ++it)
    {
      auto content = *it;
      if (content->id == id)
        return content;
    }
  }
  return nullptr;
}

shared_ptr<TrContentRuntime> TrContentManager::findContentByPid(pid_t pid)
{
  shared_lock<shared_mutex> lock(contentsMutex);
  for (auto it = contents.begin(); it != contents.end(); ++it)
  {
    auto content = *it;
    if (content->pid == pid)
      return content;
  }
  return nullptr;
}

void TrContentManager::disposeContent(shared_ptr<TrContentRuntime> content)
{
  unique_lock<shared_mutex> lock(contentsMutex);
  auto it = std::find(contents.begin(), contents.end(), content);
  if (it != contents.end())
    contents.erase(it);
}

void TrContentManager::onNewClientOnEventChan(TrOneShotClient<events_comm::TrNativeEventMessage> &client)
{
  auto peerId = client.getCustomId();
  auto content = getContent(peerId, true);
  if (content == nullptr)
  {
    eventChanServer->removeClient(&client);
    DEBUG(LOG_TAG_CONTENT, "Failed to accept a new event chan client: could not find #%d from contents", peerId);
  }
  else
  {
    content->onEventChanConnected(client);
  }
}

void TrContentManager::onTryDestroyingContents()
{
  bool hasContentsToRemove = false;
  {
    /**
     * Use a shared lock to check if there is a content that should be destroyed, this avoids the lock contention
     * at frame tick.
     *
     * TODO: Use a lock-free queue to simplify?
     */
    shared_lock<shared_mutex> lock(contentsMutex);
    for (auto content : contents)
    {
      if (content->shouldDestroy)
      {
        hasContentsToRemove = true;
        break;
      }
    }
  }

  if (hasContentsToRemove)
  {
    /**
     * Only if there is a content that should be removed, then we need a unique lock to destroy it.
     */
    unique_lock<shared_mutex> lock(contentsMutex);
    for (auto it = contents.begin(); it != contents.end();)
    {
      auto content = *it;
      if (content->shouldDestroy)
        it = contents.erase(it);
      else
        ++it;
    }
  }
}

void TrContentManager::installScripts()
{
  auto scriptsTargetDir = constellation->getOptions().applicationCacheDirectory + "/scripts";
  if (!filesystem::exists(scriptsTargetDir))
    filesystem::create_directory(scriptsTargetDir);

  auto jsBootstrapSrc = get_jsbootstrap_ptr();
  path bootstrapPath = path(scriptsTargetDir) / "jsar-bootstrap.js";
  {
    FILE *fp = fopen(bootstrapPath.c_str(), "wb");
    if (fp != nullptr)
    {
      fwrite(jsBootstrapSrc, 1, get_jsbootstrap_size(), fp);
      fclose(fp);
    }
  }

  auto jsBundleSrc = get_jsbundle_ptr();
  path bundlePath = path(scriptsTargetDir) / "jsar-bundle.js";
  {
    FILE *fp = fopen(bundlePath.c_str(), "wb");
    if (fp != nullptr)
    {
      fwrite(jsBundleSrc, 1, get_jsbundle_size(), fp);
      fclose(fp);
    }
  }
}

void TrContentManager::startHived()
{
  {
    // Configure the hived
    hived->eventChanPort = eventChanServer->getPort();
    hived->frameChanPort = constellation->renderer->getAnimationFrameChanPort();
    hived->mediaChanPort = constellation->mediaManager->chanPort();
    hived->commandBufferChanPort = constellation->renderer->getCommandBufferChanPort();
  }
  hived->start();
}

void TrContentManager::preparePreContent()
{
  if (isPreContentSet == true)
  {
    shared_lock<shared_mutex> lock(preContentMutex);
    assert(preContent != nullptr);
    return;
  }
  else
  {
    unique_lock<shared_mutex> lock(preContentMutex);
    preContent = make_shared<TrContentRuntime>(this);
    isPreContentSet = true;
    preContent->preStart();
  }
}

void TrContentManager::acceptEventChanClients(int timeout)
{
  eventChanServer->tryAccept([this](TrOneShotClient<events_comm::TrNativeEventMessage> &newClient)
                             { onNewClientOnEventChan(newClient); }, timeout);
}

void TrContentManager::recvXRCommands(int timeout)
{
  if (constellation->xrDevice == nullptr)
    return;

  {
    // Recv the XR commands for the pre-content.
    shared_lock<shared_mutex> lock(preContentMutex);
    if (preContent != nullptr)
      preContent->recvXRCommand(timeout);
  }

  {
    // Recv the XR commands for the contents.
    shared_lock<shared_mutex> lock(contentsMutex);
    if (contents.empty())
      return;
    for (auto content : contents)
      content->recvXRCommand(timeout);
  }
}
