#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <rapidjson/document.h>

// Forward declarations
struct CdpMessage;

// Content-side base CDP Domain Handler - runs in content processes
class ContentCdpDomainHandler
{
public:
  virtual ~ContentCdpDomainHandler() = default;

  // Handle a method call for this domain in the content process
  virtual std::string handleMethod(const std::string &method, const CdpMessage &message) = 0;

  // Get domain metadata
  virtual std::string getDomainName() const = 0;
  virtual std::string getDomainDescription() const = 0;
};

// Content-side CDP Handler - manages domain implementations in content processes
class ContentCdpHandler
{
public:
  ContentCdpHandler();
  ~ContentCdpHandler();

  // Process incoming CDP message from the proxy and return response
  std::string processMessage(const std::string &message);

  // Register a domain handler
  void registerDomain(const std::string &domainName, std::unique_ptr<ContentCdpDomainHandler> handler);

private:
  std::unordered_map<std::string, std::unique_ptr<ContentCdpDomainHandler>> domains_;

  std::string extractDomain(const std::string &method);
  std::string extractMethodName(const std::string &method);
};