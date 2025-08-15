#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/error/en.h>
#include <common/debug.hpp>
#include <runtime/constellation.hpp>

#include "./cdp_handler.hpp"
#include "./content_domain_proxy.hpp"
#include "./cdp_runtime_domain.hpp"
#include "./cdp_myexample_domain.hpp"
#include "./cdp_jsar_universal_rendering_server_domain.hpp"

using namespace std;

// CdpMessage implementation
unique_ptr<CdpMessage> CdpMessage::parse(const string &json)
{
  auto message = make_unique<CdpMessage>();

  rapidjson::Document doc;
  doc.Parse(json.c_str());

  if (doc.HasParseError())
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP: Failed to parse JSON: %s", rapidjson::GetParseError_En(doc.GetParseError()));
    return nullptr;
  }

  // Extract id (optional for events)
  if (doc.HasMember("id"))
  {
    if (doc["id"].IsInt64())
      message->id = doc["id"].GetInt64();
    else if (doc["id"].IsInt())
      message->id = doc["id"].GetInt();
    else if (doc["id"].IsUint64())
      message->id = static_cast<int64_t>(doc["id"].GetUint64());
    else if (doc["id"].IsUint())
      message->id = static_cast<int64_t>(doc["id"].GetUint());
    else if (doc["id"].IsString())
      message->id = stoll(doc["id"].GetString());
  }

  // Extract method (required)
  if (!doc.HasMember("method") || !doc["method"].IsString())
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP: Message missing required 'method' field");
    return nullptr;
  }
  message->method = doc["method"].GetString();

  // Extract params (optional)
  if (doc.HasMember("params"))
  {
    message->params.CopyFrom(doc["params"], doc.GetAllocator());
  }
  else
  {
    message->params.SetObject();
  }

  return message;
}

// CdpResponse implementation
string CdpResponse::success(int64_t id, const rapidjson::Value &result)
{
  rapidjson::Document response;
  response.SetObject();
  auto &allocator = response.GetAllocator();

  response.AddMember("id", id, allocator);

  rapidjson::Value resultCopy;
  resultCopy.CopyFrom(result, allocator);
  response.AddMember("result", resultCopy, allocator);

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  response.Accept(writer);

  return buffer.GetString();
}

string CdpResponse::error(int64_t id, int code, const string &message)
{
  rapidjson::Document response;
  response.SetObject();
  auto &allocator = response.GetAllocator();

  response.AddMember("id", id, allocator);

  rapidjson::Value error;
  error.SetObject();
  error.AddMember("code", code, allocator);
  error.AddMember("message", rapidjson::Value().SetString(message.c_str(), allocator), allocator);

  response.AddMember("error", error, allocator);

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  response.Accept(writer);

  return buffer.GetString();
}

string CdpResponse::event(const string &method, const rapidjson::Value &params)
{
  rapidjson::Document response;
  response.SetObject();
  auto &allocator = response.GetAllocator();

  response.AddMember("method", rapidjson::Value().SetString(method.c_str(), allocator), allocator);

  rapidjson::Value paramsCopy;
  paramsCopy.CopyFrom(params, allocator);
  response.AddMember("params", paramsCopy, allocator);

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  response.Accept(writer);

  return buffer.GetString();
}

// CdpHandler implementation
CdpHandler::CdpHandler(TrConstellation *constellation, const string &clientId, TrInspectorClient *inspectorClient)
    : clientId_(clientId)
    , inspectorClient_(inspectorClient)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP: Handler initialized for client: %s", clientId.c_str());

  // Initialize content domain proxy
  contentProxy_ = make_unique<ContentDomainProxy>(constellation);

  // Create domain instances directly (host-side domains only)
  // Runtime and Example domains are now handled by content processes via proxy
  domains_["JSAR.UniversalRenderingServer"] = make_unique<CdpJsarUniversalRenderingServerDomain>(constellation,
                                                                                                 clientId);

  // Set inspector client reference for JSAR.UniversalRenderingServer domain
  auto *jsarDomain = dynamic_cast<CdpJsarUniversalRenderingServerDomain *>(domains_["JSAR.UniversalRenderingServer"].get());
  if (jsarDomain)
  {
    jsarDomain->setInspectorClient(inspectorClient);
  }
}

CdpHandler::~CdpHandler()
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP: Handler destroyed");
}

string CdpHandler::processMessage(const string &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP: Processing message: %s", message.c_str());

  auto cdpMessage = CdpMessage::parse(message);
  if (!cdpMessage)
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP: Failed to parse message");
    return CdpResponse::error(-1, -32700, "Parse error");
  }

  string domain = extractDomain(cdpMessage->method);
  string methodName = extractMethodName(cdpMessage->method);

  DEBUG(LOG_TAG_INSPECTOR, "CDP: Domain=%s, Method=%s, ID=%lld", domain.c_str(), methodName.c_str(), (long long)cdpMessage->id);

  // Check if this domain should be forwarded to content processes
  if (contentProxy_ && contentProxy_->shouldForwardDomain(domain))
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP: Forwarding domain %s to content process", domain.c_str());
    return contentProxy_->forwardRequest(cdpMessage->method, *cdpMessage, clientId_);
  }

  // Find local domain handler
  auto domainIt = domains_.find(domain);
  if (domainIt == domains_.end())
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP: Unknown domain: %s", domain.c_str());
    return CdpResponse::error(cdpMessage->id, -32601, "Method not found");
  }

  try
  {
    return domainIt->second->handleMethod(methodName, *cdpMessage, clientId_);
  }
  catch (const exception &e)
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP: Domain handler error: %s", e.what());
    return CdpResponse::error(cdpMessage->id, -32603, "Internal error");
  }
}

void CdpHandler::processMessageAsync(const string &message, TrInspectorClient *inspectorClient)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP: Processing message asynchronously: %s", message.c_str());

  auto cdpMessage = CdpMessage::parse(message);
  if (!cdpMessage)
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP: Failed to parse message");
    string errorResponse = CdpResponse::error(-1, -32700, "Parse error");
    if (inspectorClient && inspectorClient->isWebSocket())
    {
      inspectorClient->sendWebSocketMessage(errorResponse);
    }
    return;
  }

  string domain = extractDomain(cdpMessage->method);
  string methodName = extractMethodName(cdpMessage->method);

  DEBUG(LOG_TAG_INSPECTOR, "CDP: Async Domain=%s, Method=%s, ID=%lld", domain.c_str(), methodName.c_str(), (long long)cdpMessage->id);

  // Check if this domain should be forwarded to content processes
  if (contentProxy_ && contentProxy_->shouldForwardDomain(domain))
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP: Async forwarding domain %s to content process", domain.c_str());

    // Use async forwarding without blocking
    contentProxy_->forwardRequestAsync(cdpMessage->method, *cdpMessage, clientId_, [inspectorClient](const string &response)
                                       {
        if (inspectorClient && inspectorClient->isWebSocket())
        {
          inspectorClient->sendWebSocketMessage(response);
        } });
    return;
  }

  // Handle local domain synchronously and send response
  auto domainIt = domains_.find(domain);
  if (domainIt == domains_.end())
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP: Unknown domain: %s", domain.c_str());
    string errorResponse = CdpResponse::error(cdpMessage->id, -32601, "Method not found");
    if (inspectorClient && inspectorClient->isWebSocket())
    {
      inspectorClient->sendWebSocketMessage(errorResponse);
    }
    return;
  }

  try
  {
    string response = domainIt->second->handleMethod(methodName, *cdpMessage, clientId_);
    if (inspectorClient && inspectorClient->isWebSocket())
    {
      inspectorClient->sendWebSocketMessage(response);
    }
  }
  catch (const exception &e)
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP: Domain handler error: %s", e.what());
    string errorResponse = CdpResponse::error(cdpMessage->id, -32603, "Internal error");
    if (inspectorClient && inspectorClient->isWebSocket())
    {
      inspectorClient->sendWebSocketMessage(errorResponse);
    }
  }
}

void CdpHandler::addProtocolDefinitions(rapidjson::Value &domains, rapidjson::Document::AllocatorType &allocator)
{
  // Get protocol definitions from all registered domains
  for (const auto &[domainName, domainHandler] : domains_)
  {
    // Create domain object
    rapidjson::Value domainObj;
    domainObj.SetObject();

    // Add domain name and description
    domainObj.AddMember("domain",
                        rapidjson::Value().SetString(domainHandler->getDomainName().c_str(), allocator),
                        allocator);
    domainObj.AddMember("description",
                        rapidjson::Value().SetString(domainHandler->getDomainDescription().c_str(), allocator),
                        allocator);

    // Add commands
    rapidjson::Value commands;
    commands.SetArray();

    auto domainCommands = domainHandler->getCommands();
    for (const auto &cmd : domainCommands)
    {
      rapidjson::Value cmdObj;
      cmdObj.SetObject();
      cmdObj.AddMember("name",
                       rapidjson::Value().SetString(cmd.name.c_str(), allocator),
                       allocator);
      cmdObj.AddMember("description",
                       rapidjson::Value().SetString(cmd.description.c_str(), allocator),
                       allocator);
      commands.PushBack(cmdObj, allocator);
    }

    domainObj.AddMember("commands", commands, allocator);
    domains.PushBack(domainObj, allocator);
  }
}

string CdpHandler::extractDomain(const string &method)
{
  size_t dotPos = method.rfind('.');
  if (dotPos == string::npos)
  {
    return method; // No domain separator, treat whole string as domain
  }
  return method.substr(0, dotPos);
}

string CdpHandler::extractMethodName(const string &method)
{
  size_t dotPos = method.rfind('.');
  if (dotPos == string::npos)
  {
    return ""; // No method name
  }
  return method.substr(dotPos + 1);
}
