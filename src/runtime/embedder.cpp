#include "embedder.hpp"
#include "common/analytics/perf_counter.hpp"

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
  analytics::PerformanceCounter perfCounter("HostTick");
  constellation->tick(perfCounter);
  perfCounter.end();

#ifdef TR_ENABLE_PERF_COUNTER
  if (perfCounter.duration() > 2.0)
    DEBUG(LOG_TAG_ERROR, "Detected a long tick(>=2ms) in host frame: \n%s", perfCounter.toString().c_str());
#endif
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
