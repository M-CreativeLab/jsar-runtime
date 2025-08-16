#pragma once

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>
#include <rapidjson/document.h>
#include <common/inspector/cdp_message.hpp>

// Forward declarations
class TrConstellation;
class TrInspectorClient;
class ContentDomainProxy;

// Base CDP Domain Handler
class CdpDomainHandler
{
public:
  virtual ~CdpDomainHandler() = default;

  // Handle a method call for this domain
  virtual std::string handleMethod(const std::string &method, const CdpMessage &message, const std::string &clientId) = 0;

  // Get domain metadata
  virtual std::string getDomainName() const = 0;
  virtual std::string getDomainDescription() const = 0;
  virtual std::vector<CdpCommand> getCommands() const = 0;

  // Legacy method - kept for backward compatibility but will be removed
  virtual void addProtocolDefinition(rapidjson::Value &domains, rapidjson::Document::AllocatorType &allocator)
  {
  }
};

// CDP Handler - Per-client coordinator for CDP message processing
class CdpHandler
{
public:
  CdpHandler(TrConstellation *constellation, const std::string &clientId, TrInspectorClient *inspectorClient);
  ~CdpHandler();

  // Process incoming CDP message and return response
  std::string processMessage(const std::string &message);

  // Process incoming CDP message asynchronously (for WebSocket clients)
  void processMessageAsync(const std::string &message, TrInspectorClient *inspectorClient);

  // Get protocol definitions from all registered domains
  void addProtocolDefinitions(rapidjson::Value &domains, rapidjson::Document::AllocatorType &allocator);

  // Get the content domain proxy for handling content process responses
  ContentDomainProxy *getContentProxy() const;

private:
  std::unordered_map<std::string, std::unique_ptr<CdpDomainHandler>> domains_;
  std::unique_ptr<ContentDomainProxy> contentProxy_;
  std::string clientId_;
  TrInspectorClient *inspectorClient_; // For async responses

  std::string extractDomain(const std::string &method);
  std::string extractMethodName(const std::string &method);
};
