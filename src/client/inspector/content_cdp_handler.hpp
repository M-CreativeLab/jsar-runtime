#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <rapidjson/document.h>
#include <common/inspector/cdp_message.hpp>

// Forward declarations
class ContentCdpLogDomain;

// Function type for sending CDP events from content process to host
using CdpEventSender = std::function<bool(const std::string &eventJson)>;

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

  // Set the event sender callback (called by ContentCdpHandler)
  virtual void setEventSender(const CdpEventSender &eventSender)
  {
    eventSender_ = eventSender;
  }

protected:
  // Send a CDP event to the host process
  bool sendEvent(const std::string &eventJson)
  {
    if (eventSender_)
    {
      return eventSender_(eventJson);
    }
    return false;
  }

private:
  CdpEventSender eventSender_;
};

// Content-side CDP Handler - manages domain implementations in content processes
class ContentCdpHandler
{
public:
  ContentCdpHandler();
  ContentCdpHandler(const CdpEventSender &eventSender);
  ~ContentCdpHandler();

  // Process incoming CDP message from the proxy and return response
  std::string processMessage(const std::string &message);

  // Register a domain handler
  void registerDomain(const std::string &domainName, std::unique_ptr<ContentCdpDomainHandler> handler);

  // Set event sender for all registered domains
  void setEventSender(const CdpEventSender &eventSender);

  // Get specific domain handler
  template <typename T>
  T *getDomain(const std::string &domainName)
  {
    auto it = domains_.find(domainName);
    if (it != domains_.end())
    {
      return dynamic_cast<T *>(it->second.get());
    }
    return nullptr;
  }

  // Convenience method to get Log domain
  ContentCdpLogDomain *getLogDomain();

private:
  std::unordered_map<std::string, std::unique_ptr<ContentCdpDomainHandler>> domains_;
  CdpEventSender eventSender_;

  std::string extractDomain(const std::string &method);
  std::string extractMethodName(const std::string &method);
};
