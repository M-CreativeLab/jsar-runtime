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
#include "embedder.hpp"
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
}

void TrContentRuntime::start(TrXSMLRequestInit init)
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

    auto renderer = contentManager->constellation->getRenderer();
    renderer->addContentRenderer(this);
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
  /**
   * Disposing the content runtime will wait for the command buffer is executed, also when a content is disposing, it should
   * avoid the command buffer receiver to receive any new command buffer requests.
   */
  lock_guard<mutex> lock(recvCommandBuffersMutex);
  auto renderer = contentManager->constellation->getRenderer();
  assert(renderer != nullptr);
  renderer->removeCommandBufferChanClient(commandBufferChanClient);
  DEBUG(LOG_TAG_CONTENT, "Terminating the client(%d).", pid);

  kill(pid, SIGKILL);
}

void TrContentRuntime::dispose()
{
  terminate();
  while (true)
  {
    if (testClientProcessExitOnFrame()) // Return util the child is exit.
      break;
  }
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

void TrContentRuntime::setupWithXRCommandBufferClient(TrOneShotClient<xr::TrXRCommandMessage> *client)
{
  assert(client != nullptr);
  xrCommandChanReceiver = new xr::TrXRCommandReceiver(client);
  xrCommandChanSender = new xr::TrXRCommandSender(client);
  xrCommandChanClient = client;
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

  // Add constellation options
  scriptContext.AddMember("applicationCacheDirectory",
                          rapidjson::Value(constellationOptions.applicationCacheDirectory.c_str(), allocator), allocator);
  scriptContext.AddMember("httpsProxyServer",
                          rapidjson::Value(constellationOptions.httpsProxyServer.c_str(), allocator), allocator);

  auto xrDevice = getConstellation()->getXrDevice();
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
      if (WIFEXITED(status)) // Exit
        DEBUG(LOG_TAG_CONTENT, "The client process(%d) exits, and code: %d, core dump: %s", pid, WEXITSTATUS(status));
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
      if (onCommandBufferRequestReceived)
        onCommandBufferRequestReceived(commandBuffer);
    }
    else
    {
      break;
    }
  }
}

void TrContentRuntime::recvEvent()
{
  if (eventChanReceiver == nullptr)
    return;

  auto eventMessage = eventChanReceiver->recvEvent(0);
  if (eventMessage != nullptr)
  {
    auto eventTarget = contentManager->constellation->getNativeEventTarget();
    if (eventTarget != nullptr)
      eventTarget->dispatchEvent(eventMessage->type, eventMessage->detail());
    delete eventMessage;
  }
}

void TrContentRuntime::recvClientOutput()
{
  struct pollfd fds[1];
  fds[0].fd = childPipes[0];
  fds[0].events = POLLIN;

  int events = poll(fds, 1, 0);
  if (events <= -1)
    return;

  char buf[2048];
  if (fds[0].revents & POLLIN)
  {
    ssize_t bytesRead = 0;
    do
    {
      bytesRead = read(childPipes[0], buf, sizeof(buf));
      if (bytesRead == -1)
      {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
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
  }
}

bool TrContentRuntime::tickOnFrame()
{
  recvEvent();
  recvClientOutput();
  return true;
}

TrContentManager::TrContentManager(TrConstellation *constellation) : constellation(constellation)
{
  eventChanServer = new TrOneShotServer<TrEventMessage>("eventChan");
}

TrContentManager::~TrContentManager()
{
  if (eventChanServer != nullptr)
  {
    delete eventChanServer;
    eventChanServer = nullptr;
  }
  for (auto content : contents)
    delete content;
  contents.clear();
}

bool TrContentManager::initialize()
{
  auto eventTarget = constellation->getNativeEventTarget();
  eventTarget->addEventListener(TrEventType::TR_EVENT_XSML_REQUEST, [this](TrEventType type, TrEvent &event)
                                { this->onRequestEvent(event); });

  watcherRunning = true;
  eventChanWatcher = std::make_unique<thread>([this]()
                                              {
    SET_THREAD_NAME("TrEventChanWatcher");
    while (watcherRunning)
    {
      auto newClient = eventChanServer->tryAccept(1000);
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
          DEBUG(LOG_TAG_CONTENT, "New client(#%d) connected to the event channel.", newClient->getPid());
        else
          eventChanServer->removeClient(newClient); // remove the client if it is not found by pid.
      }
    } });

  commandBuffersWorkerRunning = true;
  commandBuffersRecvWorker = std::make_unique<thread>([this]()
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
        for (auto content : contents)
          content->recvCommandBuffers(timeout);
      }
    } });

  xrCommandsWorkerRunning = true;
  xrCommandsRecvWorker = std::make_unique<thread>([this]()
                                                  {
    SET_THREAD_NAME("TrXRCommandsWorker");

    uint32_t timeout = 100;
    while (xrCommandsWorkerRunning)
    {
      if (contents.empty() || constellation->getXrDevice() == nullptr)
      {
        this_thread::sleep_for(chrono::milliseconds(timeout));
        continue;
      }
      else
      {
        auto xrDevice = constellation->getXrDevice();
        for (auto it = contents.begin(); it != contents.end(); ++it)
        {
          auto content = *it;
          if (content->xrCommandChanReceiver == nullptr)
            continue;
          auto xrCommandMessage = content->xrCommandChanReceiver->recvCommandMessage(timeout);
          if (xrCommandMessage != nullptr)
          {
            xrDevice->handleCommandMessage(*xrCommandMessage, content);
            delete xrCommandMessage;
          }
        }
      }
    } });
  return true;
}

bool TrContentManager::shutdown()
{
  if (commandBuffersRecvWorker != nullptr)
  {
    commandBuffersWorkerRunning = false;
    commandBuffersRecvWorker->join();
    DEBUG(LOG_TAG_CONTENT, "CommandBuffers Receiver is stopped.");
  }
  for (auto content : contents)
    content->dispose();
  DEBUG(LOG_TAG_CONTENT, "All contents(%zu) has been disposed", contents.size());

  if (eventChanWatcher != nullptr)
  {
    watcherRunning = false;
    eventChanWatcher->join();
    DEBUG(LOG_TAG_CONTENT, "Native Events Watcher is stopped.");
  }
  if (xrCommandsRecvWorker != nullptr)
  {
    xrCommandsWorkerRunning = false;
    xrCommandsRecvWorker->join();
    DEBUG(LOG_TAG_CONTENT, "XR Commands Receiver is stopped.");
  }
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
      auto renderer = constellation->getRenderer();
      renderer->removeContentRenderer(content);
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
