#pragma once

#include <string>
#include <set>
#include <map>
#include <chrono>
#include <common/command_buffers/base.hpp>

#include "./cdp_handler.hpp"

class TrConstellation;
class TrInspectorClient;

namespace renderer
{
  class TrRenderer;
  class TrContentRenderer;
}

// JSAR.UniversalRenderingServer domain handler for CDP
class CdpJsarUniversalRenderingServerDomain : public CdpDomainHandler
{
public:
  CdpJsarUniversalRenderingServerDomain(TrConstellation *constellation, const std::string &clientId);
  ~CdpJsarUniversalRenderingServerDomain();

  std::string handleMethod(const std::string &method, const CdpMessage &message, const std::string &clientId) override;

  // New declarative interface
  std::string getDomainName() const override;
  std::string getDomainDescription() const override;
  std::vector<CdpCommand> getCommands() const override;

  // Public methods for command buffer callbacks
  void onCommandBufferExecuted(const std::vector<commandbuffers::TrCommandBufferBase *> &commandBuffers,
                               const renderer::TrContentRenderer *contentRenderer);

  // Set the inspector client for this domain instance
  void setInspectorClient(TrInspectorClient *client);

private:
  TrConstellation *constellation_;
  std::string clientId_;
  int callbackId_ = -1; // Callback ID for unregistering
  bool tracingEnabled_ = false;
  TrInspectorClient *inspectorClient_ = nullptr;

  // Event throttling for performance
  std::chrono::steady_clock::time_point lastEventTime_;
  uint32_t eventThrottleMs_ = 100; // Default: max 10 events per second
  uint64_t totalEventsReceived_ = 0;
  uint64_t totalEventsSent_ = 0;

  renderer::TrRenderer *getRenderer() const;

  // Tracing control methods (also controls command buffer dispatching)
  std::string enableTracing(const CdpMessage &message, const std::string &clientId);
  std::string disableTracing(const CdpMessage &message, const std::string &clientId);

  // Event throttling configuration
  std::string setEventThrottle(const CdpMessage &message);

  // Frame rate control methods
  std::string setClientFrameRate(const CdpMessage &message);

  // Renderer inspection methods
  std::string getRendererInfo(const CdpMessage &message);
  std::string getContentRenderers(const CdpMessage &message);

  // Command buffer dispatching
  void sendCommandBufferEvent(const std::vector<commandbuffers::TrCommandBufferBase *> &commandBuffers,
                              const renderer::TrContentRenderer *contentRenderer);
  std::string serializeCommandBuffers(const std::vector<commandbuffers::TrCommandBufferBase *> &commandBuffers,
                                      const renderer::TrContentRenderer *contentRenderer);
};
