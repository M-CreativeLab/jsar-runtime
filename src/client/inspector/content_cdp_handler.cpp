#include "./content_cdp_handler.hpp"
#include "./content_runtime_domain.hpp"
#include "./content_example_domain.hpp"
#include "./content_log_domain.hpp"
#include "runtime/inspector/cdp_handler.hpp" // For CdpMessage and CdpResponse
#include "common/debug.hpp"

using namespace std;

ContentCdpHandler::ContentCdpHandler()
{
  DEBUG(LOG_TAG_INSPECTOR, "Content CDP: Handler initialized");

  // Register content-side domain implementations
  registerDomain("Runtime", make_unique<ContentCdpRuntimeDomain>());
  registerDomain("Example", make_unique<ContentCdpExampleDomain>());
  registerDomain("Log", make_unique<ContentCdpLogDomain>());
}

ContentCdpHandler::~ContentCdpHandler()
{
  DEBUG(LOG_TAG_INSPECTOR, "Content CDP: Handler destroyed");
}

string ContentCdpHandler::processMessage(const string &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "Content CDP: Processing message: %s", message.c_str());

  auto cdpMessage = CdpMessage::parse(message);
  if (!cdpMessage)
  {
    DEBUG(LOG_TAG_INSPECTOR, "Content CDP: Failed to parse message");
    return CdpResponse::error(-1, -32700, "Parse error");
  }

  string domain = extractDomain(cdpMessage->method);
  string methodName = extractMethodName(cdpMessage->method);

  DEBUG(LOG_TAG_INSPECTOR, "Content CDP: Domain=%s, Method=%s, ID=%lld", domain.c_str(), methodName.c_str(), (long long)cdpMessage->id);

  // Find domain handler
  auto domainIt = domains_.find(domain);
  if (domainIt == domains_.end())
  {
    DEBUG(LOG_TAG_INSPECTOR, "Content CDP: Unknown domain: %s", domain.c_str());
    return CdpResponse::error(cdpMessage->id, -32601, "Method not found");
  }

  try
  {
    return domainIt->second->handleMethod(methodName, *cdpMessage);
  }
  catch (const exception &e)
  {
    DEBUG(LOG_TAG_INSPECTOR, "Content CDP: Domain handler error: %s", e.what());
    return CdpResponse::error(cdpMessage->id, -32603, "Internal error");
  }
}

void ContentCdpHandler::registerDomain(const string &domainName, unique_ptr<ContentCdpDomainHandler> handler)
{
  DEBUG(LOG_TAG_INSPECTOR, "Content CDP: Registering domain: %s", domainName.c_str());
  domains_[domainName] = move(handler);
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
