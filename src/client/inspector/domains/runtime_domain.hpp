#pragma once

#include <client/inspector/content_cdp_handler.hpp>

namespace client_inspector::domains
{
  // Content-side Runtime domain implementation
  class CdpRuntimeDomain : public ContentCdpDomainHandler
  {
  public:
    CdpRuntimeDomain();
    ~CdpRuntimeDomain() override = default;

    // ContentCdpDomainHandler interface
    std::string handleMethod(const std::string &method, const CdpMessage &message) override;
    std::string getDomainName() const override
    {
      return "Runtime";
    }
    std::string getDomainDescription() const override
    {
      return "Content-side Runtime domain";
    }

  private:
    // Method handlers
    std::string enable(const CdpMessage &message);
    std::string disable(const CdpMessage &message);
    std::string getVersion(const CdpMessage &message);

    bool enabled_ = false;
  };
}