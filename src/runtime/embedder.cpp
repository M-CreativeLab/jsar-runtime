#include "embedder.hpp"
#include "common/analytics/perf_counter.hpp"

using namespace std;
using namespace renderer;

TrEmbedder::TrEmbedder(TrHostEngine hostEngine)
    : constellation(make_shared<TrConstellation>(this)),
      hostEngine(hostEngine)
{
  struct sigaction sa;
  if (sigaction(SIGPIPE, nullptr, &sa) == -1)
    return;

  if (sa.sa_handler == SIG_DFL)
  {
    signal(SIGPIPE, SIG_IGN);
    DEBUG(LOG_TAG_ERROR, "Note: SIGPIPE has been ignored because the host process has no handler for it.");
  }
}

bool TrEmbedder::configure(string storageDirectory, string httpsProxyServer, bool enableXR)
{
  TrConstellationInit init;
  init.applicationCacheDirectory = storageDirectory;
  init.httpsProxyServer = httpsProxyServer;
  init.isXRSupported = enableXR;
  init.enableV8Profiling = false;
  return constellation->configure(init);
}

bool TrEmbedder::configureXrDevice(xr::TrDeviceInit &init)
{
  if (!constellation->options.isXRSupported)
  {
    DEBUG(LOG_TAG_ERROR, "configureXRDevice() requires `enableXR` to be true in configure() firstly.");
    return false;
  }
  constellation->xrDevice->configure(init);
  return true;
}

bool TrEmbedder::onFrame()
{
  analytics::PerformanceCounter perfCounter("HostTick");
  constellation->tick(perfCounter);
  perfCounter.end();

#ifdef TR_ENABLE_PERF_COUNTER
  auto frameDuration = perfCounter.duration();
  if (frameDuration > 2.0)
    DEBUG(LOG_TAG_ERROR, "Detected a long tick(>=2ms) in host frame: \n%s", perfCounter.toString().c_str());
  constellation->getPerfFs()->setFrameDuration(frameDuration);
#endif
  return true;
}
