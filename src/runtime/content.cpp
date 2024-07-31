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

#include "debug.hpp"
#include "embedder.hpp"
#include "content.hpp"
#include "media_manager.hpp"
#include "crates/jsar_jsbundle.h"

TrContentRuntime::TrContentRuntime(TrContentManager *contentMgr) : contentManager(contentMgr)
{
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

  // Flush the client's output.
  while (true)
  {
    if (!recvClientOutput())
      break;
  }
}

void TrContentRuntime::start(TrDocumentRequestInit &init)
{
  if (pipe(childPipes) == -1)
  {
    DEBUG(LOG_TAG_ERROR, "Failed to create child pipes for content runtime.");
  }
  else
  {
    int flags = fcntl(childPipes[0], F_GETFL, 0);
    if (flags != -1)
    {
      flags |= O_NONBLOCK;
      fcntl(childPipes[0], F_SETFL, flags);
    }
  }

  id = init.id;
  requestInit = init;
  constellationOptions = contentManager->constellation->getOptions();
  eventChanPort = contentManager->eventChanServer->getPort();
  frameChanPort = contentManager->constellation->renderer->getAnimationFrameChanPort();
  commandBufferChanPort = contentManager->constellation->renderer->getCommandBufferChanPort();
  DEBUG(LOG_TAG_CONTENT, "Start a new client process for %s", init.url.c_str());

  // start a new process for client.
  pid = fork();
  if (pid == -1)
  {
    DEBUG(LOG_TAG_CONTENT, "Failed to fork a new process.");
    return;
  }
  else if (pid == 0)
  {
    /**
     * Configure pipes for child process.
     */
    close(childPipes[0]); // close read pipe in child
    dup2(childPipes[1], STDOUT_FILENO);
    dup2(childPipes[1], STDERR_FILENO); // TODO: split stderr into another channel?
    close(childPipes[1]);

    onClientProcess();
  }
  else
  {
    /** Configure pipes for parent process  */
    close(childPipes[1]);

    commandBuffersRecvWorker = std::make_unique<WorkerThread>("TrCommandBuffersWorker", [this](WorkerThread &worker)
                                                              { recvCommandBuffers(worker, 100); });
    auto renderer = contentManager->constellation->renderer;
    renderer->addContentRenderer(this);
    reportDocumentEvent(TrDocumentEventType::SpawnProcess);
    DEBUG(LOG_TAG_CONTENT, "The client process(%d) is started.", pid);
  }
}

void TrContentRuntime::pause()
{
  // TODO
}

void TrContentRuntime::resume()
{
  // TODO
}

void TrContentRuntime::terminate()
{
  DEBUG(LOG_TAG_CONTENT, "Terminating the client(%d).", pid);
  kill(pid, SIGKILL);
}

void TrContentRuntime::dispose()
{
  terminate();
  while (true)
  {
    if (testClientProcessExitOnFrame()) // Return util the child is exit.
    {
      shouldDestroy = true;
      break;
    }
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

void TrContentRuntime::onClientProcess()
{
  path basePath = path(constellationOptions.runtimeDirectory);
  path clientPath = basePath / "TransmuteClient";
  auto embedder = getConstellation()->getEmbedder();
  /**
   * NOTE: Even though the `libTransmuteClient.{so|dylib}` is a shared library name, the file is actually an executable, and the reason
   * to do this trick is to make Unity copy this file to the apk.
   */
  if (embedder->isEmbeddingWith(TrHostEngine::Unity))
  {
#ifdef __ANDROID__
    clientPath = basePath / "libTransmuteClient.so";
#elif __APPLE__
    clientPath = basePath / "libTransmuteClient.dylib";
#endif
  }
  fprintf(stdout, "Start a new client with: %s\n", clientPath.c_str());

  rapidjson::Document scriptContext;
  scriptContext.SetObject();

  auto constellation = getConstellation();
  auto &allocator = scriptContext.GetAllocator();
  scriptContext.AddMember("id", requestInit.id, allocator);
  scriptContext.AddMember("disableCache", requestInit.disableCache, allocator);
  scriptContext.AddMember("isPreview", requestInit.isPreview, allocator);
  scriptContext.AddMember("runScripts", rapidjson::Value(requestInit.getRunScriptsName().c_str(), allocator), allocator);
  scriptContext.AddMember("eventChanPort", eventChanPort, allocator);
  scriptContext.AddMember("frameChanPort", frameChanPort, allocator);
  scriptContext.AddMember("mediaChanPort", constellation->mediaManager->chanPort(), allocator);
  scriptContext.AddMember("commandBufferChanPort", commandBufferChanPort, allocator);

  // Add constellation options
  scriptContext.AddMember("applicationCacheDirectory",
                          rapidjson::Value(constellationOptions.applicationCacheDirectory.c_str(), allocator), allocator);
  scriptContext.AddMember("httpsProxyServer",
                          rapidjson::Value(constellationOptions.httpsProxyServer.c_str(), allocator), allocator);
  scriptContext.AddMember("enableV8Profiling", constellationOptions.enableV8Profiling, allocator);

  auto xrDevice = constellation->xrDevice;
  if (xrDevice != nullptr)
  {
    rapidjson::Value xrDeviceObject(rapidjson::kObjectType);
    xrDeviceObject.AddMember("enabled", xrDevice->enabled(), allocator);
    xrDeviceObject.AddMember("active", true, allocator);
    xrDeviceObject.AddMember("stereoRenderingMode", static_cast<int>(xrDevice->getStereoRenderingMode()), allocator);
    xrDeviceObject.AddMember("commandChanPort", xrDevice->getCommandChanPort(), allocator);
    xrDeviceObject.AddMember("inputSourcesZonePath",
                             rapidjson::Value(xrDevice->getInputSourcesZonePath().c_str(), allocator),
                             allocator);
    scriptContext.AddMember("xrDevice", xrDeviceObject, allocator);
  }

  rapidjson::StringBuffer scriptContextBuffer;
  rapidjson::Writer<rapidjson::StringBuffer> scriptContextWriter(scriptContextBuffer);
  scriptContext.Accept(scriptContextWriter);

  char *scriptArgs[] = {
      const_cast<char *>(clientPath.c_str()),
      const_cast<char *>(requestInit.url.c_str()),
      const_cast<char *>(scriptContextBuffer.GetString()),
      nullptr};

  if (execvp(clientPath.c_str(), scriptArgs) == -1)
  {
    fprintf(stderr, "Failed to execute the client process on %s, and the error: %s\n",
            clientPath.c_str(), strerror(errno));
  }

  /**
   * NOTE: The following code will not be executed if `execvp()` is successful, thus the `exit()` does guarantee the child process
   * to be terminated specifically.
   */
  exit(0);
}

bool TrContentRuntime::testClientProcessExitOnFrame()
{
  int status;
  pid_t child = waitpid(pid, &status, WNOHANG);
  if (child == -1)
  {
    DEBUG(LOG_TAG_CONTENT, "Failed to wait for the client process(%d): %s", pid, strerror(errno));
    return false;
  }
  else if (child > 0)
  {
    if (WIFEXITED(status) || WIFSTOPPED(status))
    {
      if (WIFEXITED(status)) // Exit
        DEBUG(LOG_TAG_CONTENT, "The client process(%d) exits with code(%d)", pid, WEXITSTATUS(status));
      else // Stopped
        DEBUG(LOG_TAG_CONTENT, "The client process(%d) is stopped with a signal: %d", pid, WSTOPSIG(status));
      pid = -1;
      return true;
    }
    else if (WIFSIGNALED(status))
    {
      // Process is terminated
      DEBUG(LOG_TAG_CONTENT, "The client process(%d) is terminated by a signal: %d, and core dumped: %d",
            pid, WTERMSIG(status), WCOREDUMP(status));
      pid = -1;
      return true;
    }
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

bool TrContentRuntime::recvClientOutput()
{
  struct pollfd fds[1];
  fds[0].fd = childPipes[0];
  fds[0].events = POLLIN;

  int events = poll(fds, 1, 0);
  if (events <= 0)
    return false;

  char buf[2048];
  if (fds[0].revents & POLLIN)
  {
    bool r = true;
    ssize_t bytesRead = 0;
    do
    {
      bytesRead = read(childPipes[0], buf, sizeof(buf));
      if (bytesRead <= 0)
      {
        r = false;
        if (bytesRead == -1 && (errno != EAGAIN && errno != EWOULDBLOCK))
          DEBUG(LOG_TAG_ERROR, "Failed to read pipe from client(%d): %s", pid, strerror(errno));
        break;
      }
      for (int pos = 0; pos < bytesRead; pos++)
      {
        if (buf[pos] == '\n')
        {
          DEBUG(LOG_TAG_CLIENT_ENTRY, "client(%d): %s", pid, lastClientOutput.c_str());
          lastClientOutput.clear();
        }
        else
        {
          lastClientOutput += buf[pos];
        }
      }
    } while (bytesRead > 0);
    return r;
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
  recvClientOutput();
  return true;
}

TrContentManager::TrContentManager(TrConstellation *constellation) : constellation(constellation)
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
                                                    { onNewEventChan(); });
  xrCommandsRecvWorker = std::make_unique<WorkerThread>("TrXRCommandsWorker", [this](WorkerThread &)
                                                        { onRecvXrCommands(); });
  contentsDestroyingWorker = std::make_unique<WorkerThread>("TrContentsMgrWorker", [this](WorkerThread &)
                                                            {
    this_thread::sleep_for(chrono::milliseconds(1000));
    {
      bool needDestroy = false;
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
            needDestroy = true;
            break;
          }
        }
      }

      if (needDestroy)
      {
        /**
         * Only if there is a content that should be destroyed, then we need to use a unique lock to destroy it.
         */
        unique_lock<shared_mutex> lock(contentsMutex);
        for (auto it = contents.begin(); it != contents.end();)
        {
          auto content = *it;
          if (content->shouldDestroy)
          {
            delete content;
            it = contents.erase(it);
          }
          else
          {
            ++it;
          }
        }
      }
    } });
  return true;
}

bool TrContentManager::shutdown()
{
  contentsDestroyingWorker->stop();
  {
    unique_lock<shared_mutex> lock(contentsMutex);
    auto size = contents.size();
    for (auto content : contents)
    {
      content->dispose();
      delete content;
    }
    contents.clear();
    DEBUG(LOG_TAG_CONTENT, "All contents(%zu) has been disposed and removed", size);
  }

  eventChanWatcher->stop();
  xrCommandsRecvWorker->stop();
  return true;
}

bool TrContentManager::tickOnFrame()
{
  // Check the status of each content runtime.
  shared_lock<shared_mutex> lock(contentsMutex);
  for (auto content : contents)
  {
    if (content->pid > 0 && content->testClientProcessExitOnFrame())
      content->shouldDestroy = true;
    else
      content->tickOnFrame();
  }
  return true;
}

TrContentRuntime *TrContentManager::makeContent()
{
  TrContentRuntime *content = new TrContentRuntime(this);
  {
    unique_lock<shared_mutex> lock(contentsMutex);
    contents.push_back(content);
  }
  return content;
}

TrContentRuntime *TrContentManager::findContent(pid_t pid)
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

void TrContentManager::disposeContent(TrContentRuntime *content)
{
  unique_lock<shared_mutex> lock(contentsMutex);
  auto it = std::find(contents.begin(), contents.end(), content);
  if (it != contents.end())
  {
    contents.erase(it);
    delete content;
  }
}

void TrContentManager::onRecvXrCommands(int timeout)
{
  if (contents.empty() || constellation->xrDevice == nullptr)
  {
    this_thread::sleep_for(chrono::milliseconds(timeout));
  }
  else
  {
    shared_lock<shared_mutex> lock(contentsMutex);
    for (auto it = contents.begin(); it != contents.end(); ++it)
    {
      auto content = *it;
      if (content->xrCommandChanReceiver == nullptr)
        continue;
      auto xrCommandMessage = content->xrCommandChanReceiver->recvCommandMessage(timeout);
      if (xrCommandMessage != nullptr)
      {
        constellation->xrDevice->handleCommandMessage(*xrCommandMessage, content);
        delete xrCommandMessage;
      }
    }
  }
}

void TrContentManager::onNewEventChan()
{
  eventChanServer->tryAccept([this](TrOneShotClient<events_comm::TrNativeEventMessage> &newClient)
                             {
        shared_lock<shared_mutex> lock(contentsMutex);

        bool foundNewClient = false;
        for (auto it = contents.begin(); it != contents.end(); ++it)
        {
          auto content = *it;
          if (content->pid == newClient.getPid())
          {
            foundNewClient = true;
            content->eventChanReceiver = std::make_unique<events_comm::TrNativeEventReceiver>(&newClient);
            content->eventChanSender = std::make_unique<events_comm::TrNativeEventSender>(&newClient);
            break;
          }
        }

        if (foundNewClient)
          DEBUG(LOG_TAG_CONTENT, "New client(#%d) connected to the event channel.", newClient.getPid());
        else
        {
          DEBUG(LOG_TAG_CONTENT, "Failed to accept a new client(#%d) on the event channel: pid is not found",
                newClient.getPid());
          eventChanServer->removeClient(&newClient); // remove the client if it is not found by pid.
        } }, 1000);
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
