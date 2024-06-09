#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "debug.hpp"
#include "content.hpp"
#include "crates/jsar_jsbindings.h"

using namespace v8;
using namespace node;

TrContentRuntime::TrContentRuntime(TrContentManager *contentMgr) : contentManager(contentMgr),
                                                                   requestInit(TrXSMLRequestInit("", 0))
{
  auto eventTarget = contentManager->constellation->getNativeEventTarget();
  assert(eventTarget != nullptr);

  eventTarget->addEventListener(TrEventType::TR_EVENT_RPC_REQUEST, [this](TrEventType type, TrEvent &event)
                                { this->getConstellation()->onEvent(event, this); });
}

TrContentRuntime::~TrContentRuntime()
{
  /**
   * Destroy the content runtime will wait for the command buffer is executed, also when a content is disposing, it should
   * avoid the command buffer receiver to receive any new command buffer requests.
   */
  lock_guard<mutex> lock(recvCommandBuffersMutex);
  if (eventChanReceiver != nullptr)
  {
    delete eventChanReceiver;
    eventChanReceiver = nullptr;
  }
  if (eventChanSender != nullptr)
  {
    delete eventChanSender;
    eventChanSender = nullptr;
  }
  if (commandBufferChanReceiver != nullptr)
  {
    delete commandBufferChanReceiver;
    commandBufferChanReceiver = nullptr;
  }
  if (commandBufferChanSender != nullptr)
  {
    delete commandBufferChanSender;
    commandBufferChanSender = nullptr;
  }

  auto renderer = contentManager->constellation->getRenderer();
  renderer->removeCommandBufferChanClient(commandBufferChanClient);
}

void TrContentRuntime::start(TrXSMLRequestInit init)
{
  requestInit = init;
  constellationOptions = contentManager->constellation->getOptions();
  eventChanPort = contentManager->eventChanServer->getPort();
  frameChanPort = contentManager->constellation->getRenderer()->getAnimationFrameChanPort();
  commandBufferChanPort = contentManager->constellation->getRenderer()->getCommandBufferChanPort();
  DEBUG(LOG_TAG_CONTENT, "Start a new client process for %s", init.url.c_str());

  // start a new process for client.
  pid = fork();
  if (pid == -1)
  {
    DEBUG(LOG_TAG_CONTENT, "Failed to fork a new process.");
    return;
  }
  else if (pid == 0)
    onClientProcess();
  else
    DEBUG(LOG_TAG_CONTENT, "The client process(%d) is started.", pid);
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
  kill(pid, SIGKILL);
}

void TrContentRuntime::dispose()
{
  delete this;
}

void TrContentRuntime::onCommandBuffersExecuting()
{
  isCommandBufferRequestsExecuting = true;
}

void TrContentRuntime::onCommandBuffersExecuted()
{
  isCommandBufferRequestsExecuting = false;
}

TrConstellation *TrContentRuntime::getConstellation()
{
  return contentManager->constellation;
}

xr::Device *TrContentRuntime::getXrDevice()
{
  return contentManager->constellation->getXrDevice();
}

void TrContentRuntime::setupWithCommandBufferClient(TrOneShotClient<TrCommandBufferMessage> *client)
{
  commandBufferChanReceiver = new TrCommandBufferReceiver(client);
  commandBufferChanSender = new TrCommandBufferSender(client);
  commandBufferChanClient = client;
}

bool TrContentRuntime::sendCommandBufferResponse(TrCommandBufferResponse &res)
{
  if (commandBufferChanSender != nullptr)
    return commandBufferChanSender->sendCommandBufferResponse(res);
  else
    return false;
}

bool TrContentRuntime::sendEventResponse(TrEvent &event)
{
  if (eventChanSender == nullptr)
    return false;
  TrEventMessage eventMessage(event);
  return eventChanSender->sendEvent(eventMessage);
}

void TrContentRuntime::onClientProcess()
{
  path basePath = path(constellationOptions.runtimeDirectory);
#if defined(__ANDROID__)
  /**
   * NOTE: Even though the `libTransmuteClient.so` is a shared library name, the file is actually an executable, and the reason
   * to do this trick is to make Unity copy this file to the apk.
   */
  path clientPath = basePath / "libTransmuteClient.so";
#else
  path clientPath = basePath / "TransmuteClient";
#endif
  DEBUG(LOG_TAG_CONTENT, "Start a new client with: %s", clientPath.c_str());

  rapidjson::Document scriptContext;
  scriptContext.SetObject();

  auto &allocator = scriptContext.GetAllocator();
  scriptContext.AddMember("id", requestInit.id, allocator);
  scriptContext.AddMember("disableCache", requestInit.disableCache, allocator);
  scriptContext.AddMember("isPreview", requestInit.isPreview, allocator);
  scriptContext.AddMember("runScripts", rapidjson::Value(requestInit.runScripts.c_str(), allocator), allocator);
  scriptContext.AddMember("eventChanPort", eventChanPort, allocator);
  scriptContext.AddMember("frameChanPort", frameChanPort, allocator);
  scriptContext.AddMember("commandBufferChanPort", commandBufferChanPort, allocator);

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
    DEBUG(LOG_TAG_CONTENT, "Failed to execute the client process on %s, and the error: %s",
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
      DEBUG(LOG_TAG_CONTENT, "The client process(%d) is terminated or stopped.", pid);
      pid = -1;
      return true;
    }
    else if (WIFSIGNALED(status))
    {
      DEBUG(LOG_TAG_CONTENT, "The client process(%d) is terminated by a signal: %d, and core dumped: %d",
            pid, WTERMSIG(status), WCOREDUMP(status));
      pid = -1;
      return true;
    }
  }
  return false;
}

void TrContentRuntime::recvCommandBuffers(uint32_t timeout)
{
  lock_guard<mutex> lock(recvCommandBuffersMutex);
  if (commandBufferChanReceiver == nullptr)
    return; // Skip if the command buffer channel is not ready.
  if (isCommandBufferRequestsExecuting == true)
    return; // Skip if the command buffer requests are executing.

  while (true)
  {
    auto commandBuffer = commandBufferChanReceiver->recvCommandBufferRequest(timeout);
    if (commandBuffer != nullptr)
    {
      lock_guard<mutex> lock(commandBufferRequestsMutex);
      commandBufferRequests.push_back(commandBuffer);
    }
  }
}

bool TrContentRuntime::tickOnFrame()
{
  if (eventChanReceiver == nullptr)
    return false;

  auto eventMessage = eventChanReceiver->recvEvent(0);
  if (eventMessage != nullptr)
  {
    auto eventTarget = contentManager->constellation->getNativeEventTarget();
    if (eventTarget != nullptr)
      eventTarget->dispatchEvent(eventMessage->type, eventMessage->detail());
    delete eventMessage;
  }
  return true;
}

TrContentManager::TrContentManager(TrConstellation *constellation) : constellation(constellation)
{
  eventChanServer = new TrOneShotServer<TrEventMessage>("eventChan");
}

TrContentManager::~TrContentManager()
{
  // Stop command buffers worker
  if (commandBuffersRecvWorker != nullptr)
  {
    commandBuffersWorkerRunning = false;
    commandBuffersRecvWorker->join();
    delete commandBuffersRecvWorker;
    commandBuffersRecvWorker = nullptr;
  }
  DEBUG(LOG_TAG_CONTENT, "All command buffers worker is stopped.");

  // Stop event channel watcher
  if (eventChanWatcher != nullptr)
  {
    watcherRunning = false;
    eventChanWatcher->join();
    delete eventChanWatcher;
    eventChanWatcher = nullptr;
  }
  if (eventChanServer != nullptr)
  {
    delete eventChanServer;
    eventChanServer = nullptr;
  }
  DEBUG(LOG_TAG_CONTENT, "All event channel watcher is stopped.");

  // Dispose all contents
  for (auto it = contents.begin(); it != contents.end(); ++it)
    delete *it;
  contents.clear();
  DEBUG(LOG_TAG_CONTENT, "All contents are disposed.");
}

bool TrContentManager::initialize()
{
  auto eventTarget = constellation->getNativeEventTarget();
  eventTarget->addEventListener(TrEventType::TR_EVENT_XSML_REQUEST, [this](TrEventType type, TrEvent &event)
                                { this->onRequestEvent(event); });

  watcherRunning = true;
  eventChanWatcher = new thread([this]()
                                {
    SET_THREAD_NAME("TrEventChanWatcher");
    while (watcherRunning)
    {
      auto newClient = eventChanServer->tryAccept(-1);
      if (newClient != nullptr)
      {
        lock_guard<mutex> lock(contentsMutex);

        bool foundNewClient = false;
        for (auto it = contents.begin(); it != contents.end(); ++it)
        {
          auto content = *it;
          if (content->pid == newClient->getPid())
          {
            foundNewClient = true;
            content->eventChanReceiver = new TrEventReceiver(newClient);
            content->eventChanSender = new TrEventSender(newClient);
            break;
          }
        }

        if (foundNewClient)
          DEBUG(LOG_TAG_CONTENT, "New client connected to the event channel: %d", newClient->getPid());
        else
          eventChanServer->removeClient(newClient); // remove the client if it is not found by pid.
      }
    } });

  commandBuffersWorkerRunning = true;
  commandBuffersRecvWorker = new thread([this]()
                                        {
    SET_THREAD_NAME("TrCommandBuffersWorker");

    uint32_t timeout = 100;
    while (commandBuffersWorkerRunning)
    {
      /**
       * TODO: If there is no content, we should not start the worker.
       */
      if (contents.empty())
      {
        this_thread::sleep_for(chrono::milliseconds(timeout));
        continue;
      }
      else
      {
        for (auto it = contents.begin(); it != contents.end(); ++it)
          (*it)->recvCommandBuffers(timeout);
      }
    } });
  return true;
}

bool TrContentManager::tickOnFrame()
{
  // Check the status of each content runtime.
  lock_guard<mutex> lock(contentsMutex);
  for (auto it = contents.begin(); it != contents.end();)
  {
    auto content = *it;
    if (content->pid > 0 && content->testClientProcessExitOnFrame())
    {
      delete content;
      it = contents.erase(it);
    }
    else
    {
      content->tickOnFrame();
      ++it;
    }
  }
  return true;
}

TrContentRuntime *TrContentManager::makeContent()
{
  TrContentRuntime *content = new TrContentRuntime(this);
  {
    lock_guard<mutex> lock(contentsMutex);
    contents.push_back(content);
  }
  return content;
}

TrContentRuntime *TrContentManager::findContent(pid_t pid)
{
  lock_guard<mutex> lock(contentsMutex);
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
  lock_guard<mutex> lock(contentsMutex);
  auto it = std::find(contents.begin(), contents.end(), content);
  if (it != contents.end())
  {
    contents.erase(it);
    delete content;
  }
}

void TrContentManager::onRequestEvent(TrEvent &event)
{
  if (event.type != TrEventType::TR_EVENT_XSML_REQUEST)
    return;

  auto content = makeContent();
  if (content != nullptr)
    content->start(event.detail.get<TrXSMLRequestInit>());
}
