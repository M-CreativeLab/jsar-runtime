#include "embedder.hpp"

TrEmbedder::TrEmbedder()
{
}

TrEmbedder::~TrEmbedder()
{
}

bool TrEmbedder::initialize()
{
  return true;
}

bool TrEmbedder::configureXrDevice(bool enabled)
{
  return true;
}

void TrEmbedder::shutdown()
{
}

bool TrEmbedder::onStart(string argJson)
{
  return true;
}

bool TrEmbedder::onFrame()
{
  return true;
}

TrConstellation *TrEmbedder::getConstellation()
{
  return nullptr;
}

TrNativeEventTarget *TrEmbedder::getNativeEventTarget()
{
  return nullptr;
}

TrRenderer *TrEmbedder::getRenderer()
{
  return nullptr;
}

xr::Device *TrEmbedder::getXrDevice()
{
  return nullptr;
}
