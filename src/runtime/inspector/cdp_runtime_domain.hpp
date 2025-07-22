#pragma once

#include "./cdp_handler.hpp"

class TrConstellation;

// Runtime domain handler for CDP
class CdpRuntimeDomain : public CdpDomainHandler
{
public:
  CdpRuntimeDomain(TrConstellation *constellation);

  std::string handleMethod(const std::string &method, const CdpMessage &message) override;
  void addProtocolDefinition(rapidjson::Value &domains, rapidjson::Document::AllocatorType &allocator) override;

private:
  TrConstellation *constellation_;

  // Runtime domain methods
  std::string enable(const CdpMessage &message);
  std::string disable(const CdpMessage &message);
  std::string getVersion(const CdpMessage &message);
  std::string evaluate(const CdpMessage &message);
};