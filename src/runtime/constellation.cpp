#include <rapidjson/document.h>
#include <idgen.hpp>

#include "./constellation.hpp"
#include "./content_manager.hpp"
#include "./media_manager.hpp"
#include "./embedder.hpp"

#ifdef TR_ENABLE_INSPECTOR
#include "./inspector.hpp"
#endif

using namespace std;
using namespace std::filesystem;
using namespace renderer;

void __tr_empty()
{
  /**
   * Nothing to do here, this function is used to get the library path.
   */
}

TrConstellation::TrConstellation(TrEmbedder *embedder)
    : embedder(embedder)
{
  srand(static_cast<unsigned int>(time(nullptr)));

  nativeEventTarget = std::make_shared<events_comm::TrNativeEventTarget>();
  contentManager = std::make_shared<TrContentManager>(this);
  mediaManager = std::make_shared<TrMediaManager>(this);
  renderer = TrRenderer::Make(this);
  xrDevice = xr::Device::Make(this);

#ifdef TR_ENABLE_INSPECTOR
  inspector = std::make_shared<TrInspector>(this);
#endif
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
  {
    contentManager->initialize();
    mediaManager->initialize();
    renderer->initialize();
    xrDevice->initialize();
    perfFs = std::make_shared<TrHostPerformanceFileSystem>(options);

#ifdef TR_ENABLE_INSPECTOR
    inspector->initialize();
#endif
  }
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

void TrConstellation::onBeforeRendering(analytics::PerformanceCounter &perfCounter)
{
  if (initialized == false || disableTicking) [[unlikely]]
    return;

  contentManager->tickOnFrame();
  perfCounter.record("finishContentManager");

  renderer->onBeforeRendering();
  perfCounter.record("finishRenderer");
}

void TrConstellation::onOpaquesRenderPass(analytics::PerformanceCounter &perfCounter)
{
  if (initialized == false || disableTicking) [[unlikely]]
    return;

  renderer->onOpaquesRenderPass(perfCounter);
  perfCounter.record("finishRenderer");

  // TODO(yorkie): move the input sources sync to the metrices update?
  if (xrDevice->enabled()) [[likely]]
  {
    xrDevice->tick();
    perfCounter.record("finishInputSourcesSync");
  }
}

void TrConstellation::onTransparentsRenderPass(analytics::PerformanceCounter &perfCounter)
{
  if (initialized == false || disableTicking) [[unlikely]]
    return;

  renderer->onTransparentsRenderPass(perfCounter);
  perfCounter.record("finishRenderer");
}

void TrConstellation::onAfterRendering()
{
  if (initialized == false || disableTicking) [[unlikely]]
    return;

  renderer->onAfterRendering();
#ifdef TR_ENABLE_INSPECTOR
  inspector->tick();
#endif
}

uint32_t TrConstellation::open(string url, optional<TrDocumentRequestInit> init)
{
  auto content = contentManager->makeContent();
  if (content == nullptr)
  {
    DEBUG(LOG_TAG_ERROR, "Failed to select or create a new content process: %s", url.c_str());
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

bool TrConstellation::close(uint32_t id)
{
  auto content = contentManager->getContent(id, false);
  if (content == nullptr)
  {
    DEBUG(LOG_TAG_UNITY, "Could not find the content with id: %d", id);
    return false;
  }
  content->dispose(false);
  return true;
}

void TrConstellation::resetContents()
{
  contentManager->disposeAll();
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
