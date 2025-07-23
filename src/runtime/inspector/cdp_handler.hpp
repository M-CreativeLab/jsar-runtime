#pragma once

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <rapidjson/document.h>

class TrInspectorClient;

// CDP (Chrome DevTools Protocol) Message Structure
struct CdpMessage
{
  int id = -1;
  std::string method;
  rapidjson::Value params;

  static std::unique_ptr<CdpMessage> parse(const std::string &json);
};

// CDP Response Builder
class CdpResponse
{
public:
  static std::string success(int id, const rapidjson::Value &result);
  static std::string error(int id, int code, const std::string &message);
  static std::string event(const std::string &method, const rapidjson::Value &params);
};

// CDP Command Definition
struct CdpCommand
{
  std::string name;
  std::string description;
  std::function<std::string(const CdpMessage &)> handler;
};

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

// CDP Handler - Main coordinator for CDP message processing
class CdpHandler
{
public:
  CdpHandler();
  ~CdpHandler();

  // Process incoming CDP message and return response
  std::string processMessage(const std::string &message, const std::string &clientId = "");

  // Register a domain factory function - called to create domain instances per client
  void registerDomainFactory(const std::string &domain, std::function<std::unique_ptr<CdpDomainHandler>(const std::string &clientId)> factory);

  // Get protocol definitions from a sample instance of each registered domain
  void addProtocolDefinitions(rapidjson::Value &domains, rapidjson::Document::AllocatorType &allocator);

  // Client lifecycle management
  void onClientConnected(const std::string &clientId);
  void onClientDisconnected(const std::string &clientId);

  // Get a specific domain instance for a client
  template<typename T>
  T* getDomainInstance(const std::string &clientId, const std::string &domainName)
  {
    auto clientIt = clientDomains_.find(clientId);
    if (clientIt == clientDomains_.end())
    {
      return nullptr;
    }
    
    auto domainIt = clientIt->second.find(domainName);
    if (domainIt == clientIt->second.end())
    {
      return nullptr;
    }
    
    return dynamic_cast<T*>(domainIt->second.get());
  }

private:
  std::unordered_map<std::string, std::function<std::unique_ptr<CdpDomainHandler>(const std::string &clientId)>> domainFactories_;
  std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<CdpDomainHandler>>> clientDomains_; // clientId -> domainName -> handler

  std::string extractDomain(const std::string &method);
  std::string extractMethodName(const std::string &method);
};
