#include <stdlib.h>
#include <unistd.h>

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
  isClient = true;
  contentManager = nullptr;

  vector<string> scriptArgs = {
      "--url",
      requestInit.url,
      "--id",
      to_string(requestInit.id),
      "--disable-cache",
      requestInit.disableCache ? "true" : "false",
      "--is-preview",
      requestInit.isPreview ? "true" : "false",
      "--run-scripts",
      requestInit.runScripts};

  clientContext = TrClientContextPerProcess::Create();  // create a new client context globally in child process.
  clientContext->id = requestInit.id;
  clientContext->url = requestInit.url;
  clientContext->applicationCacheDirectory = constellationOptions.applicationCacheDirectory;
  clientContext->httpsProxyServer = constellationOptions.httpsProxyServer;

  scriptRuntime = new TrScriptRuntimePerProcess();
  scriptRuntime->start(scriptArgs);
  // bootstrap XR or layout?
}

TrContentManager::TrContentManager(TrConstellation* constellation) : constellation(constellation)
{
}

TrContentManager::~TrContentManager()
{
  for (auto it = contentRuntimes.begin(); it != contentRuntimes.end(); ++it)
    delete *it;
  contentRuntimes.clear();
}

bool TrContentManager::initialize()
{
  auto nativeEventTarget = native_event::TrNativeEventTarget::GetOrCreateInstance();
  nativeEventTarget->addEventListener(native_event::TrEventType::TrXSMLRequest, [this](native_event::TrEventType type, native_event::TrNativeEvent &event)
                                      { this->onRequestEvent(event); });
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
