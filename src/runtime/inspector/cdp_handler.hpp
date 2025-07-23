#pragma once

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <rapidjson/document.h>

// Forward declarations
class TrConstellation;
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

// Forward declarations
class TrConstellation;
class TrInspectorClient;

// CDP Handler - Per-client coordinator for CDP message processing
class CdpHandler
{
public:
  CdpHandler(TrConstellation* constellation, const std::string& clientId, TrInspectorClient* inspectorClient);
  ~CdpHandler();

  // Process incoming CDP message and return response
  std::string processMessage(const std::string &message);

  // Get protocol definitions from all registered domains
  void addProtocolDefinitions(rapidjson::Value &domains, rapidjson::Document::AllocatorType &allocator);

  // Get a specific domain instance
  template<typename T>
  T* getDomainInstance(const std::string &domainName) const
  {
    auto it = domains_.find(domainName);
    if (it == domains_.end())
    {
      return nullptr;
    }
    
    return dynamic_cast<T*>(it->second.get());
  }

private:
  std::unordered_map<std::string, std::unique_ptr<CdpDomainHandler>> domains_;
  
  std::string extractDomain(const std::string &method);
  std::string extractMethodName(const std::string &method);
};
