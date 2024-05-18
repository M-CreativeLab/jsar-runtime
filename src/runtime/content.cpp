#include <stdlib.h>
#include <unistd.h>
#include <iostream>

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
                                                                   requestInit(native_event::TrXSMLRequestInit("", 0))
{
}

TrContentRuntime::~TrContentRuntime()
{
  // scriptRuntime.terminate();
  contentManager->disposeContent(this);
}

void TrContentRuntime::start(native_event::TrXSMLRequestInit init)
{
  requestInit = init;
  constellationOptions = contentManager->constellation->getOptions();
  channelServerPort = contentManager->eventChanServer->getPort();
  DEBUG(LOG_TAG_CONTENT, "Start a new client process for %s", init.url.c_str());

  // start a new process for client.
  pid = fork();
  if (pid == -1)
  {
    DEBUG(LOG_TAG_SCRIPT, "Failed to fork a new process.");
    return;
  }
  else if (pid == 0)
  {
    onClientProcess();
  }
}

void TrContentRuntime::pause()
{
}

void TrContentRuntime::resume()
{
}

void TrContentRuntime::terminate()
{
  // scriptRuntime.terminate();
}

void TrContentRuntime::dispose()
{
  delete this;
}

void TrContentRuntime::onClientProcess()
{
  path basePath = path(constellationOptions.runtimeDirectory);
  path clientPath = basePath / "libTransmuteClient.so";
  DEBUG(LOG_TAG_CONTENT, "Start a new client with: %s", clientPath.c_str());

  rapidjson::Document scriptContext;
  scriptContext.SetObject();

  auto &allocator = scriptContext.GetAllocator();
  scriptContext.AddMember("id", requestInit.id, allocator);
  scriptContext.AddMember("disableCache", requestInit.disableCache, allocator);
  scriptContext.AddMember("isPreview", requestInit.isPreview, allocator);
  scriptContext.AddMember("runScripts", rapidjson::Value(requestInit.runScripts.c_str(), allocator), allocator);
  scriptContext.AddMember("channelServerPort", channelServerPort, allocator);

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

TrContentManager::TrContentManager(TrConstellation *constellation) : constellation(constellation)
{
  eventChanServer = new TrOneShotServer<CustomEvent>();
}

TrContentManager::~TrContentManager()
{
  if (eventChanServer != nullptr)
  {
    delete eventChanServer;
    eventChanServer = nullptr;
  }
  for (auto it = contentRuntimes.begin(); it != contentRuntimes.end(); ++it)
    delete *it;
  contentRuntimes.clear();
}

bool TrContentManager::initialize()
{
  auto nativeEventTarget = native_event::TrNativeEventTarget::GetOrCreateInstance();
  nativeEventTarget->addEventListener(native_event::TrEventType::TrXSMLRequest, [this](native_event::TrEventType type, native_event::TrNativeEvent &event)
                                      { this->onRequestEvent(event); });

  eventChanWorker = new thread([this]()
                               {
    while (true)
    {
      eventChanServer->accept();
      for (auto receiver : eventChanServer->getReceivers())
      {
        auto data = receiver->tryRecv();
        if (data != nullptr)
        {
          DEBUG(LOG_TAG_SCRIPT, "Received event: %d", data->foobar);
          delete data;
        }
      }
    } });
  return true;
}

TrContentRuntime *TrContentManager::makeContent()
{
  TrContentRuntime *content = new TrContentRuntime(this);
  contentRuntimes.push_back(content);
  return content;
}

void TrContentManager::disposeContent(TrContentRuntime *content)
{
  auto it = std::find(contentRuntimes.begin(), contentRuntimes.end(), content);
  if (it != contentRuntimes.end())
  {
    contentRuntimes.erase(it);
    delete content;
  }
}

void TrContentManager::onRequestEvent(native_event::TrNativeEvent &event)
{
  if (event.type != native_event::TrEventType::TrXSMLRequest)
    return;

  auto init = event.detail.get<native_event::TrXSMLRequestInit>();
  auto content = makeContent();
  if (content != nullptr)
    content->start(init);
}
