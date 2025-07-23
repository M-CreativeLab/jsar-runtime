#pragma once

#include "./cdp_handler.hpp"

class TrConstellation;

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

private:
  TrConstellation *constellation_;
  renderer::TrRenderer *getRenderer() const;

  // Tracing control methods
  std::string enableTracing(const CdpMessage &message);
  std::string disableTracing(const CdpMessage &message);

  // Frame rate control methods
  std::string setClientFrameRate(const CdpMessage &message);

  // Renderer inspection methods
  std::string getRendererInfo(const CdpMessage &message);
  std::string getContentRenderers(const CdpMessage &message);

  // Command buffer debugging methods
  std::string getCommandBuffers(const CdpMessage &message);
};