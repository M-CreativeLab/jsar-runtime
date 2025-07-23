#pragma once

#include "./cdp_handler.hpp"
#include <string>
#include <set>
#include <map>

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
  CdpJsarUniversalRenderingServerDomain(TrConstellation *constellation);

  std::string handleMethod(const std::string &method, const CdpMessage &message, const std::string &clientId) override;

  // New declarative interface
  std::string getDomainName() const override;
  std::string getDomainDescription() const override;
  std::vector<CdpCommand> getCommands() const override;

  // Public methods for inspector integration
  void setInspectorClient(const std::string &clientId, TrInspectorClient *client);
  void removeInspectorClient(const std::string &clientId);
  void onCommandBufferExecuted(const std::vector<commandbuffers::TrCommandBufferBase*> &commandBuffers, const renderer::TrContentRenderer *contentRenderer);

private:
  TrConstellation *constellation_;
  renderer::TrRenderer *getRenderer() const;

  // Tracing control methods (also controls command buffer dispatching)
  std::string enableTracing(const CdpMessage &message, const std::string &clientId);
  std::string disableTracing(const CdpMessage &message, const std::string &clientId);

  // Frame rate control methods
  std::string setClientFrameRate(const CdpMessage &message);

  // Renderer inspection methods
  std::string getRendererInfo(const CdpMessage &message);
  std::string getContentRenderers(const CdpMessage &message);

  // Command buffer dispatching
  void sendCommandBufferEvent(const std::vector<commandbuffers::TrCommandBufferBase*> &commandBuffers, const renderer::TrContentRenderer *contentRenderer);
  std::string serializeCommandBuffers(const std::vector<commandbuffers::TrCommandBufferBase*> &commandBuffers, const renderer::TrContentRenderer *contentRenderer);

private:
  std::set<std::string> commandBufferClients_; // Clients subscribed to command buffer events
  std::map<std::string, TrInspectorClient*> inspectorClients_; // Client ID to inspector client mapping
};