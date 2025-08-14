#pragma once

#include "./content_cdp_handler.hpp"

// Content-side Example domain implementation
class ContentCdpExampleDomain : public ContentCdpDomainHandler
{
public:
  ContentCdpExampleDomain();
  ~ContentCdpExampleDomain() override = default;

  // ContentCdpDomainHandler interface
  std::string handleMethod(const std::string &method, const CdpMessage &message) override;
  std::string getDomainName() const override
  {
    return "Example";
  }
  std::string getDomainDescription() const override
  {
    return "Content-side Example domain for testing";
  }

private:
  // Method handlers
  std::string ping(const CdpMessage &message);
  std::string echo(const CdpMessage &message);
  std::string getInfo(const CdpMessage &message);
};