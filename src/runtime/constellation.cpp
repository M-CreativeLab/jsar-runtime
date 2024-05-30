#include <rapidjson/document.h>
#include "constellation.hpp"
#include "content.hpp"

void __tr_empty()
{
  /**
   * Nothing to do here, this function is used to get the library path.
   */
}

TrConstellationInit::TrConstellationInit()
{
}

TrConstellation::TrConstellation()
{
  srand(static_cast<unsigned int>(time(nullptr)));

  nativeEventTarget = new TrEventTarget();
  contentManager = new TrContentManager(this);
  renderer = new TrRenderer(this);
  xrDevice = new xr::Device();
}

TrConstellation::~TrConstellation()
{
  if (nativeEventTarget != nullptr)
  {
    delete nativeEventTarget;
    nativeEventTarget = nullptr;
  }
  if (contentManager != nullptr)
  {
    delete contentManager;
    contentManager = nullptr;
  }
  if (renderer != nullptr)
  {
    delete renderer;
    renderer = nullptr;
  }
  if (xrDevice != nullptr)
  {
    delete xrDevice;
    xrDevice = nullptr;
  }
  initialized = false;
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
  if (initDoc.HasMember("isXRSupported") && initDoc["isXRSupported"].IsBool())
    options.isXRSupported = initDoc["isXRSupported"].GetBool();

  Dl_info dlinfo;
  if (dladdr((void *)__tr_empty, &dlinfo))
    options.runtimeDirectory = path(dlinfo.dli_fname).parent_path().c_str();
  else
    DEBUG(LOG_TAG_CONSTELLATION, "Failed to get the runtime path from current host");

  contentManager->initialize();
  renderer->initialize();
  initialized = true;
  return true;
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

TrEventTarget *TrConstellation::getNativeEventTarget()
{
  return nativeEventTarget;
}

TrContentManager *TrConstellation::getContentManager()
{
  return contentManager;
}

renderer::TrRenderer *TrConstellation::getRenderer()
{
  return renderer;
}

xr::Device *TrConstellation::getXrDevice()
{
  return xrDevice;
}

TrConstellationInit &TrConstellation::getOptions()
{
  return options;
}

bool TrConstellation::isInitialized()
{
  return initialized;
}
