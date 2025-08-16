#include <chrono>
#include <rapidjson/document.h>
#include <common/inspector/cdp_domains.hpp>
#include <common/inspector/message.hpp>

#include "./content_cdp_handler.hpp"
#include "./domains/log_domain.hpp"
#include "./domains/runtime_domain.hpp"

namespace client_inspector
{
  using namespace std;

  ContentCdpHandler::ContentCdpHandler(const CdpEventSender &eventSender)
      : eventSender_(eventSender)
  {
    // Register content-side domain implementations
    registerDomain("Runtime", make_unique<domains::CdpRuntimeDomain>());
    registerDomain("Log", make_unique<domains::CdpLogDomain>());

    // Set event sender for all domains if provided
    if (eventSender_)
    {
      setEventSender(eventSender_);
    }
  }

  ContentCdpHandler::~ContentCdpHandler()
  {
  }

  string ContentCdpHandler::processMessage(const string &message)
  {
    auto cdpMessage = CdpMessage::parse(message);
    if (!cdpMessage)
    {
      return CdpResponse::error(-1, -32700, "Parse error");
    }

    string domain = extractDomain(cdpMessage->method);
    string methodName = extractMethodName(cdpMessage->method);

    // Find domain handler
    auto domainIt = domains_.find(domain);
    if (domainIt == domains_.end())
    {
      return CdpResponse::error(cdpMessage->id, inspector_comm::TR_CDP_METHOD_NOT_FOUND_CODE, "Method not found");
    }

    try
    {
      return domainIt->second->handleMethod(methodName, *cdpMessage);
    }
    catch (const exception &e)
    {
      return CdpResponse::error(cdpMessage->id, inspector_comm::TR_CDP_INTERNAL_ERROR_CODE, "Internal error");
    }
  }

  void ContentCdpHandler::registerDomain(const string &domainName, unique_ptr<ContentCdpDomainHandler> handler)
  {
    // Set event sender if we have one
    if (eventSender_)
    {
      handler->setEventSender(eventSender_);
    }

    domains_[domainName] = move(handler);
  }

  void ContentCdpHandler::setEventSender(const CdpEventSender &eventSender)
  {
    eventSender_ = eventSender;

    // Set event sender for all existing domains
    for (auto &domain : domains_)
    {
      domain.second->setEventSender(eventSender);
    }
  }

  string ContentCdpHandler::extractDomain(const string &method)
  {
    size_t dotPos = method.find('.');
    if (dotPos == string::npos)
      return "";
    return method.substr(0, dotPos);
  }

  string ContentCdpHandler::extractMethodName(const string &method)
  {
    size_t dotPos = method.find('.');
    if (dotPos == string::npos)
      return method;
    return method.substr(dotPos + 1);
  }

  domains::CdpLogDomain *ContentCdpHandler::getLogDomain()
  {
    return getDomain<domains::CdpLogDomain>("Log");
  }

  domains::CdpRuntimeDomain *ContentCdpHandler::getRuntimeDomain()
  {
    return getDomain<domains::CdpRuntimeDomain>("Runtime");
  }
}
