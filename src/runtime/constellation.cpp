#include <rapidjson/document.h>
#include "constellation.hpp"
#include "content.hpp"

TrConstellationInit::TrConstellationInit()
{
}

TrConstellation *TrConstellation::s_Instance = nullptr;
TrConstellation *TrConstellation::Create()
{
  if (s_Instance == nullptr)
    s_Instance = new TrConstellation();
  return s_Instance;
}

TrConstellation *TrConstellation::Get()
{
  return s_Instance;
}

TrConstellation::TrConstellation()
{
  srand(static_cast<unsigned int>(time(nullptr)));
}

TrConstellation::~TrConstellation()
{
  if (contentManager != nullptr)
    delete contentManager;
  initialized = false;
}

void TrConstellation::initialize(const char *initJson)
{
  rapidjson::Document initDoc;
  initDoc.Parse(initJson);
  if (initDoc.HasParseError())
  {
    DEBUG(LOG_TAG_CONSTELLATION, "Failed to parse init json: %s", initJson);
    return;
  }

  if (initDoc.HasMember("applicationCacheDirectory"))
    options.applicationCacheDirectory = initDoc["applicationCacheDirectory"].GetString();
  if (initDoc.HasMember("httpsProxyServer"))
    options.httpsProxyServer = initDoc["httpsProxyServer"].GetString();
  if (initDoc.HasMember("isXRSupported") && initDoc["isXRSupported"].IsBool())
    options.isXRSupported = initDoc["isXRSupported"].GetBool();

  Dl_info dlinfo;
  if (dladdr((void *)TrConstellation::Create, &dlinfo))
    options.runtimeDirectory = path(dlinfo.dli_fname).parent_path().c_str();
  else
    DEBUG(LOG_TAG_CONSTELLATION, "Failed to get the runtime path from current host");

  contentManager = new TrContentManager(this);
  renderer = new renderer::TrRenderer();

  contentManager->initialize();
  renderer->initialize();
  initialized = true;
}

void TrConstellation::tick()
{
  if (initialized == false)
    return;
  if (contentManager != nullptr)
    contentManager->tickOnFrame();
  if (renderer != nullptr)
    renderer->tickOnAnimationFrame();
}

TrContentManager *TrConstellation::getContentManager()
{
  assert(initialized == true);
  return contentManager;
}

renderer::TrRenderer *TrConstellation::getRenderer()
{
  assert(initialized == true);
  return renderer;
}

TrConstellationInit &TrConstellation::getOptions()
{
  return options;
}

bool TrConstellation::isInitialized()
{
  return initialized;
}
