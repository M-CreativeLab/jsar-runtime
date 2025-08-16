#include <chrono>
#include <rapidjson/document.h>

#include "./content_cdp_handler.hpp"
#include "./content_runtime_domain.hpp"
#include "./content_example_domain.hpp"
#include "./content_log_domain.hpp"
#include "common/inspector/cdp_domains.hpp"

using namespace std;

ContentCdpHandler::ContentCdpHandler(const CdpEventSender &eventSender)
    : eventSender_(eventSender)
{
  // Register content-side domain implementations
  registerDomain("Runtime", make_unique<ContentCdpRuntimeDomain>());
  registerDomain("Example", make_unique<ContentCdpExampleDomain>());
  registerDomain("Log", make_unique<ContentCdpLogDomain>());

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
    return CdpResponse::error(cdpMessage->id, -32601, "Method not found");
  }

  try
  {
    return domainIt->second->handleMethod(methodName, *cdpMessage);
  }
  catch (const exception &e)
  {
    return CdpResponse::error(cdpMessage->id, -32603, "Internal error");
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

ContentCdpLogDomain *ContentCdpHandler::getLogDomain()
{
  return getDomain<ContentCdpLogDomain>("Log");
}
