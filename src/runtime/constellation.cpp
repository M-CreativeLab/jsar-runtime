#include <rapidjson/document.h>
#include "constellation.hpp"
#include "content.hpp"
#include "media_manager.hpp"
#include "embedder.hpp"

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

  nativeEventTarget = std::make_unique<TrEventTarget>();
  contentManager = std::make_unique<TrContentManager>(this);
  mediaManager = std::make_unique<TrMediaManager>();
  renderer = std::make_unique<TrRenderer>(this);
  xrDevice = std::make_unique<xr::Device>(this);
}

TrConstellation::~TrConstellation()
{
}

bool TrConstellation::initialize(string initJson)
{
  rapidjson::Document initDoc;
  initDoc.Parse(initJson.c_str());
  if (initDoc.HasParseError())
  {
    DEBUG(LOG_TAG_CONSTELLATION, "Failed to parse init json: %s", initJson.c_str());
    return false;
  }

  if (initDoc.HasMember("applicationCacheDirectory"))
    options.applicationCacheDirectory = initDoc["applicationCacheDirectory"].GetString();
  if (initDoc.HasMember("httpsProxyServer"))
    options.httpsProxyServer = initDoc["httpsProxyServer"].GetString();
  if (initDoc.HasMember("enableV8Profiling") && initDoc["enableV8Profiling"].IsBool())
    options.enableV8Profiling = initDoc["enableV8Profiling"].GetBool();
  if (initDoc.HasMember("isXRSupported") && initDoc["isXRSupported"].IsBool())
    options.isXRSupported = initDoc["isXRSupported"].GetBool();

  Dl_info dlinfo;
  if (dladdr((void *)__tr_empty, &dlinfo))
    options.runtimeDirectory = path(dlinfo.dli_fname).parent_path().c_str();
  else
    DEBUG(LOG_TAG_CONSTELLATION, "Failed to get the runtime path from current host");

  // Fix the environment such as creating the cache dir.
  options.fixEnvIfNeeded();

  contentManager->initialize();
  mediaManager->initialize();
  renderer->initialize();
  initialized = true;
  return true;
}

void TrConstellation::shutdown()
{
  contentManager->shutdown();
  renderer->shutdown();
  xrDevice->shutdown();
  mediaManager->shutdown();
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

TrEventTarget *TrConstellation::getNativeEventTarget()
{
  return nativeEventTarget.get();
}

TrContentManager *TrConstellation::getContentManager()
{
  return contentManager.get();
}

renderer::TrRenderer *TrConstellation::getRenderer()
{
  return renderer.get();
}

xr::Device *TrConstellation::getXrDevice()
{
  return xrDevice.get();
}

TrEmbedder *TrConstellation::getEmbedder()
{
  return embedder;
}

bool TrConstellation::onEvent(TrEvent &event, TrContentRuntime *content)
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
