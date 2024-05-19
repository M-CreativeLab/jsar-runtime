#include "embedder.hpp"

TrEmbedder::TrEmbedder()
{
  constellation = new TrConstellation();
}

TrEmbedder::~TrEmbedder()
{
  shutdown();
}

bool TrEmbedder::configureXrDevice(bool enabled)
{
  constellation->getXrDevice()->initialize(enabled);
  return true;
}

void TrEmbedder::shutdown()
{
  if (constellation != nullptr)
  {
    delete constellation;
    constellation = nullptr;
  }
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
  return constellation;
}

TrNativeEventTarget *TrEmbedder::getNativeEventTarget()
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
