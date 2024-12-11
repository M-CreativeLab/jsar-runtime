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
#include "res/client.bin.h"
#include "crates/jsar_jsbundle.h"

TrContentRuntime::TrContentRuntime(TrContentManager *contentMgr) : contentManager(contentMgr)
{
  static TrIdGenerator idGen(0x100);
  id = idGen.get();
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
  DEBUG(LOG_TAG_CONTENT, "The content runtime(%d) has been destroyed", id);
}

void TrContentRuntime::preStart()
{
  available = true;
  commandBuffersRecvWorker = std::make_unique<WorkerThread>("TrCBWorker", [this](WorkerThread &worker)
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
  contentManager->constellation->mediaManager->removeSoundSourcesByContent(this); // Remove the sound sources.
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
  if (!used)
  {
    /**
     * FIXME: This means the pre-content is failed to boot up, we must let this abort the process to expose related problems.
     */
    DEBUG(LOG_TAG_ERROR, "Occurred a fatal error: the content(%d) is not used but the process is exited.", id);
    assert(false);
  }
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

bool TrContentRuntime::dispatchEvent(events_comm::TrNativeEvent &event)
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
  if (TR_UNLIKELY(!available || shouldDestroy || mediaChanReceiver == nullptr))
    return;

  media_comm::TrMediaCommandMessage mediaMessage;
  if (mediaChanReceiver->recvCommand(mediaMessage, 0))
  {
    auto mediaManager = getConstellation()->mediaManager;
    mediaManager->onContentRequest(this, mediaMessage);
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
    xrDevice->handleCommandMessage(*message, this);
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

TrContentManager::TrContentManager(TrConstellation *constellation)
    : constellation(constellation),
      hived(make_unique<TrHiveDaemon>(constellation))
{
  eventChanServer = new TrOneShotServer<events_comm::TrNativeEventMessage>("eventChan");

  auto eventTarget = constellation->nativeEventTarget;
  rpcRequestListener = eventTarget->addEventListener(events_comm::TrNativeEventType::RpcRequest, [this](auto type, auto &event)
                                                     { onRpcRequest(event); });
  documentEventListener = eventTarget->addEventListener(events_comm::TrNativeEventType::DocumentEvent, [this](auto type, auto &event)
                                                        { onDocumentEvent(event); });
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
  installExecutable();
  installScripts();

  eventChanWatcher = std::make_unique<WorkerThread>("TrEventChanWatcher", [this](WorkerThread &)
                                                    { acceptEventChanClients(); });
  contentsDestroyingWorker = std::make_unique<WorkerThread>("TrContentsMgr", [this](WorkerThread &worker)
                                                            { onTryDestroyingContents(); worker.sleep(); }, 1000);
  return true;
}

bool TrContentManager::shutdown()
{
  enablePreContent = false;
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

  auto eventTarget = constellation->nativeEventTarget;
  if (eventTarget != nullptr)
  {
    eventTarget->removeEventListener(events_comm::TrNativeEventType::RpcRequest, rpcRequestListener);
    eventTarget->removeEventListener(events_comm::TrNativeEventType::DocumentEvent, documentEventListener);
  }

  eventChanWatcher->stop();
  hived->shutdown();
  DEBUG(LOG_TAG_CONTENT, "TrContentManager::shutdown() done.");
  return true;
}

bool TrContentManager::tickOnFrame()
{
  hived->tick();

  // When the hive daemon is ready, we need to make sure the pre-content is always ready.
  if (enablePreContent && hived->daemonReady)
    preparePreContent();

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
  shared_ptr<TrContentRuntime> contentToUse;
  {
    unique_lock<shared_mutex> lock(contentsMutex);
    for (auto content : contents)
    {
      if (!content->used)
      {
        content->used = true;
        contentToUse = content;
        break;
      }
    }
  }
  if (contentToUse == nullptr)
  {
    // Create a new content runtime when there is no available content.
    contentToUse = make_shared<TrContentRuntime>(this);
    {
      unique_lock<shared_mutex> lock(contentsMutex);
      contentToUse->used = true;
      contents.push_back(contentToUse);
    }
  }
  return contentToUse;
}

shared_ptr<TrContentRuntime> TrContentManager::getContent(uint32_t id, bool includePreContent)
{
  shared_lock<shared_mutex> lock(contentsMutex);
  for (auto it = contents.begin(); it != contents.end(); ++it)
  {
    auto content = *it;
    if (content->id == id)
    {
      if (includePreContent || content->started)
        return content;
      else
        break;
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

void TrContentManager::onRpcRequest(events_comm::TrNativeEvent &event)
{
  if (TR_UNLIKELY(event.type != events_comm::TrNativeEventType::RpcRequest))
    return;

  /**
   * TODO: support by content?
   */
  constellation->dispatchNativeEvent(event, nullptr);
}

void TrContentManager::onDocumentEvent(events_comm::TrNativeEvent &event)
{
  if (TR_UNLIKELY(event.type != events_comm::TrNativeEventType::DocumentEvent))
    return;

  auto detail = event.detail<events_comm::TrDocumentEvent>();
  auto content = getContent(detail.documentId, true);
  if (TR_UNLIKELY(content == nullptr))
  {
    DEBUG(LOG_TAG_ERROR, "Failed to find the content(%d) for the DocumentEvent", detail.documentId);
    return;
  }
  content->logDocumentEvent(detail);
  constellation->dispatchNativeEvent(event, content.get());
}

/**
 * Install the executable to the runtime directory.
 *
 * @param runtimeDir The runtime directory to install the executable.
 * @param executableName The name of the executable.
 * @param writeContent The function to write the content to the file.
 */
static void InstallExecutable(string runtimeDir, string executableName, string executableMd5,
                              std::function<void(FILE *)> writeContent)
{
  path execPath = path(runtimeDir) / executableName;
  path execMd5Path = execPath.string() + ".md5";
  bool shouldInstall = false;

  /**
   * Check if the executable file exists, if not, we should always install it.
   */
  if (filesystem::exists(execPath))
  {
    /**
     * If the MD5 file doesn't exist, we should install the executable.
     */
    if (!filesystem::exists(execMd5Path))
    {
      shouldInstall = true;
    }
    else
    {
      /**
       * Otherwise, we need to compare the MD5 hash of the executable content.
       */
      FILE *md5fp = fopen(execMd5Path.c_str(), "rb");
      if (md5fp != nullptr)
      {
        char md5[33];
        fread(md5, 1, 32, md5fp);
        md5[32] = '\0';
        fclose(md5fp);

        /**
         * If the MD5 hash is different, we should install the executable.
         */
        if (string(md5) != executableMd5)
        {
          shouldInstall = true;
          DEBUG(LOG_TAG_CONTENT, "The MD5 hash of the executable is different, re-install it.");
        }
      }
      else
      {
        /**
         * If the MD5 file is not readable, we should install the executable.
         */
        shouldInstall = true;
      }
    }

    /**
     * When the executable is already installed and the md5 hash is also verified, we need to check if the file's permission.
     */
    if (!shouldInstall)
    {
      struct stat st;
      if (stat(execPath.c_str(), &st) != 0)
      {
        auto msg = "Failed to stat() on the executable file";
        DEBUG(LOG_TAG_ERROR, "%s: %s", msg, strerror(errno));
        throw runtime_error(msg);
      }

      /**
       * Skip the installation when both the file are verified and executable.
       */
      if ((st.st_mode & S_IXUSR) != 0)
        return;
    }
  }
  else
  {
    shouldInstall = true;
  }

  /**
   * Install the executable file and the MD5 hash.
   */
  if (shouldInstall)
  {
    // Write the file content.
    FILE *fp = fopen(execPath.c_str(), "wb");
    if (fp != nullptr)
    {
      writeContent(fp);
      fclose(fp);
    }

    // Write the MD5 hash of the content.
    FILE *md5fp = fopen(execMd5Path.c_str(), "wb");
    if (md5fp != nullptr)
    {
      fwrite(executableMd5.c_str(), 1, executableMd5.size(), md5fp);
      fclose(md5fp);
    }
  }

  // Make the library executable.
  if (chmod(execPath.c_str(), 0755) == -1)
  {
    auto msg = "Failed to chmod for the executable file";
    DEBUG(LOG_TAG_ERROR, "%s: %s", msg, strerror(errno));
    throw runtime_error(msg);
  }
}

/**
 * Install the Node.js library to the target directory.
 *
 * @param runtimeDir The runtime directory to install the library.
 */
static void InstallNodejsLibrary(string runtimeDir)
{
  string name;
#ifdef __APPLE__
  name = "libnode.108.dylib";
#elif defined(__ANDROID__)
  name = "libnode.so";
#else
  throw runtime_error("Unsupported platform to install the Node.js library");
#endif

  string md5 = string(reinterpret_cast<const char *>(get_libnode_md5_ptr()), get_libnode_md5_size());
  return InstallExecutable(runtimeDir, name, md5, [](FILE *fp)
                           { fwrite(get_libnode_ptr(), 1, get_libnode_size(), fp); });
}

void TrContentManager::installExecutable()
{
  auto executableTargetDir = constellation->getOptions().runtimeDirectory();
  if (!filesystem::exists(executableTargetDir))
    filesystem::create_directory(executableTargetDir);

  // Install the Node.js library.
  InstallNodejsLibrary(executableTargetDir);

  // Install the TransmuteClient executable.
  InstallExecutable(executableTargetDir,
                    "TransmuteClient",
                    string(transmute_client_binary_md5),
                    [](FILE *fp)
                    { fwrite(transmute_client_binary, 1, transmute_client_binary_len, fp); });
}

void TrContentManager::installScripts()
{
  auto scriptsTargetDir = constellation->getOptions().scriptsDirectory();
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
  if (preContentScheduled)
  {
    if (chrono::system_clock::now() < preContentScheduledTimepoint)
      return;

    auto preContent = make_shared<TrContentRuntime>(this);
    {
      unique_lock<shared_mutex> lock(contentsMutex);
      contents.push_back(preContent);
    }
    preContent->preStart();
    preContentScheduled = false;
    return;
  }

  bool hasPreContent = false;
  bool hasContents = false;
  {
    shared_lock<shared_mutex> lock(contentsMutex);
    hasContents = !contents.empty();
    if (hasContents)
    {
      for (auto content : contents)
      {
        if (!content->used)
        {
          hasPreContent = true;
          break;
        }
      }
    }
  }

  if (!hasPreContent)
  {
    // TODO: support the pre-content configuration?
    int delayTime = hasContents ? 3000 : 0;
    preContentScheduledTimepoint = chrono::system_clock::now() + chrono::milliseconds(delayTime);
    preContentScheduled = true;
  }
}

void TrContentManager::acceptEventChanClients(int timeout)
{
  eventChanServer->tryAccept([this](TrOneShotClient<events_comm::TrNativeEventMessage> &newClient)
                             { onNewClientOnEventChan(newClient); }, timeout);
}
