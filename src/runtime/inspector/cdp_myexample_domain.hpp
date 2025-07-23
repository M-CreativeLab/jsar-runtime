#pragma once

#include "./cdp_handler.hpp"

// MyExample domain handler for CDP - demonstrating extensibility
class CdpMyExampleDomain : public CdpDomainHandler
{
public:
  CdpMyExampleDomain();

  std::string handleMethod(const std::string &method, const CdpMessage &message) override;
  
  // New declarative interface
  std::string getDomainName() const override;
  std::string getDomainDescription() const override;
  std::vector<CdpCommand> getCommands() const override;

private:
  // Example domain methods
  std::string ping(const CdpMessage &message);
  std::string echo(const CdpMessage &message);
  std::string getInfo(const CdpMessage &message);
};