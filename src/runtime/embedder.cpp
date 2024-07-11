#include "embedder.hpp"

TrEmbedder::TrEmbedder(TrHostEngine hostEngine)
    : constellation(std::make_unique<TrConstellation>(this)),
      hostEngine(hostEngine)
{
}

bool TrEmbedder::configureXrDevice(bool xrEnabled, xr::TrDeviceInit &init)
{
  constellation->getXrDevice()->initialize(xrEnabled, init);
  return true;
}

void TrEmbedder::shutdown()
{
  constellation->shutdown();
}

uint32_t TrEmbedder::getFps()
{
  return constellation->getRenderer()->getFps();
}

uint32_t TrEmbedder::getUptime()
{
  return constellation->getRenderer()->getUptime();
}

bool TrEmbedder::onStart(string argJson)
{
  return constellation->initialize(argJson);
}

bool TrEmbedder::onFrame()
{
  auto now = chrono::steady_clock::now();
  constellation->tick();
  auto duration = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - now).count();
  if (duration > 2000)
    DEBUG(LOG_TAG_ERROR, "Detected a long tick(%dus) in host frame", duration);
  return true;
}

TrConstellation *TrEmbedder::getConstellation()
{
  return constellation.get();
}

TrEventTarget *TrEmbedder::getNativeEventTarget()
{
  return constellation->getNativeEventTarget();
}

TrRenderer *TrEmbedder::getRenderer()
{
  return constellation->getRenderer();
}

xr::Device *TrEmbedder::getXrDevice()
{
  return constellation->getXrDevice();
}

bool TrEmbedder::isEmbeddingWith(TrHostEngine engine)
{
  return hostEngine == engine;
}
