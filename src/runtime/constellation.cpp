#include <rapidjson/document.h>
#include "constellation.hpp"
#include "idgen.hpp"
#include "./content.hpp"
#include "./media_manager.hpp"
#include "./embedder.hpp"

using namespace std;

void __tr_empty()
{
  /**
   * Nothing to do here, this function is used to get the library path.
   */
}

TrConstellationInit::TrConstellationInit()
{
}

TrConstellation::TrConstellation(TrEmbedder *embedder)
    : embedder(embedder)
{
  srand(static_cast<unsigned int>(time(nullptr)));

  nativeEventTarget = std::make_shared<events_comm::TrNativeEventTarget>();
  contentManager = std::make_shared<TrContentManager>(this);
  mediaManager = std::make_shared<TrMediaManager>(this);
  renderer = TrRenderer::Make(this);
  xrDevice = std::make_shared<xr::Device>(this);
}

TrConstellation::~TrConstellation()
{
  DEBUG(LOG_TAG_CONSTELLATION, "Constellation(%p) is destroyed.", this);
}

bool TrConstellation::configure(TrConstellationInit &init)
{
  options = init;
  Dl_info dlinfo;
  if (dladdr((void *)__tr_empty, &dlinfo))
  {
    options.runDirectory = path(dlinfo.dli_fname).parent_path().c_str();
    DEBUG(LOG_TAG_CONSTELLATION, "The run path is %s", options.runDirectory.c_str());
  }
  else
    DEBUG(LOG_TAG_CONSTELLATION, "Failed to get the runtime path from current host");

  // Fix the environment such as creating the cache dir.
  options.fixEnvIfNeeded();
  return true;
}

bool TrConstellation::initialize()
{
  disableTicking = false;
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
  disableTicking = true;
  mediaManager->shutdown(); // Shutdown the media manager first to release the audio resources.
  contentManager->shutdown();
  renderer->shutdown();
  xrDevice->shutdown();
  initialized = false;
}

void TrConstellation::tick(analytics::PerformanceCounter &perfCounter)
{
  if (TR_UNLIKELY(initialized == false || disableTicking))
    return;

  contentManager->tickOnFrame();
  perfCounter.record("finishContentManager");
  renderer->tick(perfCounter);
  perfCounter.record("finishRenderer");

  if (xrDevice->enabled())
  {
    xrDevice->tick();
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

bool TrConstellation::dispatchNativeEvent(events_comm::TrNativeEvent &event, shared_ptr<TrContentRuntime> content)
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

  if (!contentManager->hived->daemonReady)
    return false;

  return true;
}
