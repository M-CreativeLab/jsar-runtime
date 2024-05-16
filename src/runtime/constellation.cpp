#include <rapidjson/document.h>
#include "constellation.hpp"
#include "content.hpp"

TrConstellationInit::TrConstellationInit()
{
}

TrConstellation* TrConstellation::s_Instance = nullptr;
TrConstellation* TrConstellation::Create()
{
  if (s_Instance == nullptr)
    s_Instance = new TrConstellation();
  return s_Instance;
}

TrConstellation* TrConstellation::Get()
{
  return s_Instance;
}

TrConstellation::TrConstellation()
{
}

TrConstellation::~TrConstellation()
{
  if (contentManager != nullptr)
    delete contentManager;
}

void TrConstellation::initialize(const char* initJson)
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

  contentManager = new TrContentManager(this);
  contentManager->initialize();
  initialized = true;
}

TrContentManager* TrConstellation::getContentManager()
{
  if (initialized == false)
  {
    DEBUG(LOG_TAG_CONSTELLATION, "Please initialize constellation first");
    return nullptr;
  }
  return contentManager;
}

TrConstellationInit& TrConstellation::getOptions()
{
  return options;
}

bool TrConstellation::isInitialized()
{
  return initialized;
}
