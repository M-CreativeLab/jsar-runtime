#include <rapidjson/document.h>
#include "constellation.hpp"
#include "idgen.hpp"
#include "./content.hpp"
#include "./media_manager.hpp"
#include "./embedder.hpp"

void __tr_empty()
{
  /**
   * Nothing to do here, this function is used to get the library path.
   */
}

TrConstellationInit::TrConstellationInit()
{
}

TrConstellation::TrConstellation(TrEmbedder *embedder) : embedder(embedder)
{
  srand(static_cast<unsigned int>(time(nullptr)));

  nativeEventTarget = std::make_shared<events_comm::TrNativeEventTarget>();
  contentManager = std::make_shared<TrContentManager>(this);
  mediaManager = std::make_shared<TrMediaManager>(this);
  renderer = std::make_shared<TrRenderer>(this);
  xrDevice = std::make_shared<xr::Device>(this);
}

TrConstellation::~TrConstellation()
{
}

bool TrConstellation::configure(TrConstellationInit& init)
{
  options = init;
  Dl_info dlinfo;
  if (dladdr((void *)__tr_empty, &dlinfo))
    options.runtimeDirectory = path(dlinfo.dli_fname).parent_path().c_str();
  else
    DEBUG(LOG_TAG_CONSTELLATION, "Failed to get the runtime path from current host");

  // Fix the environment such as creating the cache dir.
  options.fixEnvIfNeeded();
  return true;
}

bool TrConstellation::initialize()
{
  contentManager->initialize();
  mediaManager->initialize();
  renderer->initialize();
  xrDevice->initialize();
  perfFs = std::make_shared<TrHostPerformanceFileSystem>(options);
  initialized = true;

  // Start the hived when all the components are ready.
  contentManager->startHived();
  return true;
}

void TrConstellation::shutdown()
{
  contentManager->shutdown();
  mediaManager->shutdown();
  renderer->shutdown();
  xrDevice->shutdown();
  initialized = false;
}

void TrConstellation::tick(analytics::PerformanceCounter &perfCounter)
{
  if (initialized == false)
    return;
  contentManager->tickOnFrame();
  perfCounter.record("finishContentManager");
  renderer->tick(perfCounter);
  perfCounter.record("finishRenderer");

  if (xrDevice->enabled())
  {
    xrDevice->syncInputSourcesToZone();
    perfCounter.record("finishInputSourcesSync");
  }
}

TrEmbedder *TrConstellation::getEmbedder()
{
  return embedder;
}

uint32_t TrConstellation::open(string url, optional<TrDocumentRequestInit> init)
{
  auto content = contentManager->makeContent();
  if (content == nullptr)
  {
    DEBUG(LOG_TAG_CONSTELLATION, "Failed to create a new content");
    return 0;
  }

  TrDocumentRequestInit requestInit;
  if (init.has_value())
    requestInit = init.value();

  requestInit.id = content->id;
  requestInit.url = url;
  content->start(requestInit);
  return content->id;
}

bool TrConstellation::dispatchNativeEvent(events_comm::TrNativeEvent &event, TrContentRuntime *content)
{
  assert(embedder != nullptr);
  return embedder->onEvent(event, content);
}

TrConstellationInit &TrConstellation::getOptions()
{
  return options;
}

bool TrConstellation::isInitialized()
{
  return initialized;
}

bool TrConstellation::isRuntimeReady()
{
  if (!initialized)
    return false;

  // just returns if hived is ready.
  return contentManager->hived->daemonReady;
}
