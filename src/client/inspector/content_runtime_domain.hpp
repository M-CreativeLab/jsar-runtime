#pragma once

#include "./content_cdp_handler.hpp"

// Content-side Runtime domain implementation
class ContentCdpRuntimeDomain : public ContentCdpDomainHandler
{
public:
  ContentCdpRuntimeDomain();
  ~ContentCdpRuntimeDomain() override = default;

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