#include "embedder.hpp"

TrEmbedder::TrEmbedder() : constellation(std::make_unique<TrConstellation>(this))
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
  constellation->tick();
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
