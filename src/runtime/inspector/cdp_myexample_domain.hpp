#pragma once

#include "./cdp_handler.hpp"

// MyExample domain handler for CDP - demonstrating extensibility
class CdpMyExampleDomain : public CdpDomainHandler
{
public:
  CdpMyExampleDomain();

  std::string handleMethod(const std::string &method, const CdpMessage &message) override;
  void addProtocolDefinition(rapidjson::Value &domains, rapidjson::Document::AllocatorType &allocator) override;

private:
  // Example domain methods
  std::string ping(const CdpMessage &message);
  std::string echo(const CdpMessage &message);
  std::string getInfo(const CdpMessage &message);
};