#include <memory>
#include <string>
#include <map>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>

#include <rapidjson/document.h>
#include <renderer/renderer.hpp>

#include "./inspector.hpp"
#include "./constellation.hpp"
#include "./content_manager.hpp"
#include "./embedder.hpp"
#include "./inspector/cdp_handler.hpp"

using namespace std;
using namespace std::placeholders;

void TrInspector::initialize()
{
  server_ = make_unique<TrInspectorServer>(shared_from_this());
  inspectorCommandChanServer_ = make_unique<ipc::TrOneShotServer<inspector_comm::TrInspectorCommandMessage>>("InspectorCommandChan");
  DEBUG(LOG_TAG_INSPECTOR, "Inspector initialized with command channel port: %d", getInspectorCommandChanPort());
}

void TrInspector::tick()
{
  server_->tryAccept();
  server_->tick();
}

bool TrInspector::canAcceptWebSocketConnection()
{
  const int maxConnections = 5;
  return server_->countWebSocketClients() < maxConnections;
}

void TrInspector::onRequest(TrInspectorClient &requestClient)
{
  string requestUrl = requestClient.url();
  // Remove the ending slash
  if (requestUrl.size() > 1 && requestUrl.back() == '/')
    requestUrl.pop_back();

  map<string, string> params;
  if (requestUrl == "/json/version")
  {
    handleRequest(bind(&TrInspector::getVersion, this, _1), requestClient);
  }
  else if (requestUrl == "/contents" ||
           requestUrl == "/json" ||
           requestUrl == "/json/list")
  {
    handleRequest(bind(&TrInspector::getContents, this, _1), requestClient);
  }
  else if (requestUrl == "/json/protocol")
  {
    handleRequest(bind(&TrInspector::getProtocol, this, _1), requestClient);
  }
  else if (requestUrl == "/json/statistics")
  {
    handleRequest(bind(&TrInspector::getStatistics, this, _1), requestClient);
  }
  else if (matchRoute(requestUrl, "/:id/logs/stdout", params))
  {
    handleRequest(bind(&TrInspector::printContentLog, this, params["id"], "out"), requestClient);
  }
  else if (matchRoute(requestUrl, "/:id/logs/stderr", params))
  {
    handleRequest(bind(&TrInspector::printContentLog, this, params["id"], "err"), requestClient);
  }
  else
  {
    requestClient.respond(404, "Not Found");
  }
}

bool TrInspector::matchRoute(const string &url, const string &pattern, map<string, string> &params)
{
  params.clear();

  // Split URL and pattern into segments
  vector<string> urlSegments = splitPath(url);
  vector<string> patternSegments = splitPath(pattern);

  if (urlSegments.size() != patternSegments.size())
  {
    return false;
  }

  for (size_t i = 0; i < urlSegments.size(); ++i)
  {
    const string &urlSeg = urlSegments[i];
    const string &patternSeg = patternSegments[i];

    if (patternSeg.starts_with(":"))
    {
      // Parameter segment
      string paramName = patternSeg.substr(1);
      params[paramName] = urlSeg;
    }
    else if (urlSeg != patternSeg)
    {
      // Literal segment doesn't match
      return false;
    }
  }

  return true;
}

vector<string> TrInspector::splitPath(const string &path)
{
  vector<string> segments;
  if (path.empty() || path == "/")
  {
    return segments;
  }

  size_t start = 1; // Skip leading slash
  size_t pos = path.find('/', start);

  while (pos != string::npos)
  {
    if (pos > start)
    {
      segments.push_back(path.substr(start, pos - start));
    }
    start = pos + 1;
    pos = path.find('/', start);
  }

  // Add the last segment
  if (start < path.length())
  {
    segments.push_back(path.substr(start));
  }

  return segments;
}

void TrInspector::handleRequest(function<string()> handler, TrInspectorClient &requestClient)
{
  try
  {
    string responseText = handler();
    requestClient.respond(200, responseText);
  }
  catch (const exception &e)
  {
    requestClient.respond(500, "Internal Server Error: " + string(e.what()));
  }
  catch (...)
  {
    requestClient.respond(500, "Internal Server Error");
  }
}

void TrInspector::handleRequest(function<bool(rapidjson::Document &)> handler, TrInspectorClient &requestClient)
{
  rapidjson::Document json;
  try
  {
    if (handler(json))
      requestClient.respond(200, json);
    else
      throw runtime_error("Failed to handle the request");
  }
  catch (const exception &e)
  {
    requestClient.respond(500, "Internal Server Error: " + string(e.what()));
  }
  catch (...)
  {
    requestClient.respond(500, "Internal Server Error");
  }
}

bool TrInspector::getVersion(rapidjson::Document &json)
{
  auto embedder = constellation->getEmbedder();
  string browserTitle = "JSAR v" + embedder->getVersion();

  json.SetObject();
  auto &allocator = json.GetAllocator();

  // CDP fields
  json.AddMember("Browser", rapidjson::Value().SetString(browserTitle.c_str(), allocator), allocator);
  json.AddMember("Protocol-Version", rapidjson::Value().SetString("1.3", allocator), allocator);

  // JSAR extended fields
  json.AddMember("Uptime", rapidjson::Value().SetInt64(embedder->getUptime()), allocator);
  return true;
}

bool TrInspector::getContents(rapidjson::Document &json)
{
  json.SetArray();
  auto &allocator = json.GetAllocator();

  for (const auto &content : constellation->contentManager->contents)
  {
    auto &requestInit = content->requestInit;
    string id = to_string(content->id);
    string title = "jsar[" + id + "]";
    string url = requestInit.url;
    string debuggerUrl = "ws://localhost:9423/devtools/inspector/" + id;
    string devtoolsFrontendUrl = "devtools://devtools/inspector/devtools.html?ws=" + debuggerUrl;

    // Make sure the URL is a valid file URL if it's an absolute path
    if (url.starts_with("/"))
      url = "file://" + url;

    rapidjson::Value contentJson;
    contentJson.SetObject();

    // CDP fields
    contentJson.AddMember("description",
                          rapidjson::Value().SetString("JSAR page", allocator),
                          allocator);
    contentJson.AddMember("devtoolsFrontendUrl",
                          rapidjson::Value().SetString(devtoolsFrontendUrl.c_str(), allocator),
                          allocator);
    contentJson.AddMember("devtoolsFrontendUrlCompat",
                          rapidjson::Value().SetString(devtoolsFrontendUrl.c_str(), allocator),
                          allocator);
    contentJson.AddMember("faviconUrl",
                          rapidjson::Value()
                            .SetString("https://nodejs.org/static/images/favicons/favicon.ico", allocator),
                          allocator);
    contentJson.AddMember("id", rapidjson::Value().SetString(id.c_str(), allocator), allocator);
    contentJson.AddMember("title", rapidjson::Value().SetString(title.c_str(), allocator), allocator);
    contentJson.AddMember("type", rapidjson::Value().SetString("page", allocator), allocator);
    contentJson.AddMember("url", rapidjson::Value().SetString(url.c_str(), allocator), allocator);
    contentJson.AddMember("webSocketDebuggerUrl",
                          rapidjson::Value().SetString(debuggerUrl.c_str(), allocator),
                          allocator);

    // JSAR extended fields
    contentJson.AddMember("pid", content->pid.load(), allocator);
    contentJson.AddMember("used", content->used.load(), allocator);
    {
      // RequestInit fields
      rapidjson::Value requestInitJson;
      requestInitJson.SetObject();
      requestInitJson.AddMember("url",
                                rapidjson::Value().SetString(requestInit.url.c_str(), allocator),
                                allocator);
      requestInitJson.AddMember("disableCache", requestInit.disableCache, allocator);
      contentJson.AddMember("requestInit", requestInitJson, allocator);
    }
    {
      // Logs fields
      rapidjson::Value logsJson;
      logsJson.SetObject();

      // TODO(yorkie): Reading the Host from the request header instead of using "localhost".
      static const string inspectorHost = "localhost:" + to_string(server_->port);
      string outPath = "http://" + inspectorHost + "/" + id + "/logs/stdout";
      string errPath = "http://" + inspectorHost + "/" + id + "/logs/stderr";

      logsJson.AddMember("stdout",
                         rapidjson::Value().SetString(outPath.c_str(), allocator),
                         allocator);
      logsJson.AddMember("stderr",
                         rapidjson::Value().SetString(errPath.c_str(), allocator),
                         allocator);
      contentJson.AddMember("logs", logsJson, allocator);
    }
    json.PushBack(contentJson, allocator);
  }

  return true;
}

bool TrInspector::getProtocol(rapidjson::Document &json)
{
  json.SetObject();
  auto &allocator = json.GetAllocator();

  rapidjson::Value domains;
  domains.SetArray();

  // Create a temporary CDP handler to get protocol definitions
  auto tempHandler = make_unique<CdpHandler>(constellation, "temp", nullptr);
  tempHandler->addProtocolDefinitions(domains, allocator);

  json.AddMember("version", rapidjson::Value().SetString("1.3", allocator), allocator);
  json.AddMember("domains", domains, allocator);
  return true;
}

bool TrInspector::getStatistics(rapidjson::Document &json)
{
  auto embedder = constellation->getEmbedder();

  json.SetObject();
  auto &allocator = json.GetAllocator();

  json.AddMember("uptime", rapidjson::Value().SetInt64(embedder->getUptime()), allocator);
  json.AddMember("fps", rapidjson::Value().SetInt(embedder->getFps()), allocator);

  // add renderers
  rapidjson::Value renderers_list;
  renderers_list.SetArray();
  auto add_content_renderer_fields = [&renderers_list, &allocator](const renderer::TrContentRenderer &content_renderer)
  {
    rapidjson::Value descriptor;
    descriptor.SetObject();
    descriptor.AddMember("contentId", rapidjson::Value().SetInt(content_renderer.contentId), allocator);
    descriptor.AddMember("contextId", rapidjson::Value().SetInt(content_renderer.contextId), allocator);
    descriptor.AddMember("drawCallsPerFrame",
                         rapidjson::Value().SetInt(content_renderer.drawCallsPerFrame),
                         allocator);
    descriptor.AddMember("drawCallsCountPerFrame",
                         rapidjson::Value().SetInt(content_renderer.drawCallsCountPerFrame),
                         allocator);
    descriptor.AddMember("frameDuration",
                         rapidjson::Value().SetInt64(content_renderer.frameDuration.count()),
                         allocator);
    descriptor.AddMember("maxFrameDuration",
                         rapidjson::Value().SetInt64(content_renderer.maxFrameDuration.count()),
                         allocator);
    renderers_list.PushBack(descriptor, allocator);
  };
  constellation->renderer->iterateContentRenderers(add_content_renderer_fields);
  json.AddMember("renderers", renderers_list, allocator);

  return true;
}

string TrInspector::printContentLog(const string &contentId, const string &logType)
{
  static filesystem::path logsDir = filesystem::path(constellation->options.applicationCacheDirectory) / "logs";

  const auto &content = constellation->contentManager->getContent(stoi(contentId), true);
  if (content == nullptr)
    return "No such content";

  filesystem::path logFile = logsDir / (to_string(content->pid.load()) + "." + logType + ".log");
  if (!filesystem::exists(logFile))
    return "No log file found for content(" + contentId + ")";

  ifstream file(logFile);
  if (!file.is_open())
    return "Failed to open log file for content(" + contentId + ")";

  stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

void TrInspector::onMessage(TrInspectorClient &client, const string &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "Received WebSocket message: %s", message.c_str());

  auto cdpHandler = client.getCdpHandler();
  if (!cdpHandler)
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP handler not initialized for client, falling back to echo");
    client.sendWebSocketMessage("Echo: " + message);
    return;
  }

  try
  {
    // Use async processing for WebSocket clients to avoid blocking
    if (client.isWebSocket())
    {
      cdpHandler->processMessageAsync(message, &client);
    }
    else
    {
      // Fall back to synchronous processing for non-WebSocket clients
      string response = cdpHandler->processMessage(message);
      client.sendWebSocketMessage(response);
    }
  }
  catch (const exception &e)
  {
    DEBUG(LOG_TAG_INSPECTOR, "Error processing CDP message: %s", e.what());
    client.sendWebSocketMessage("{\"id\":-1,\"error\":{\"code\":-32603,\"message\":\"Internal error\"}}");
  }
}

void TrInspector::onClientConnected(TrInspectorClient &client)
{
  DEBUG(LOG_TAG_INSPECTOR, "Client connected: %s", client.clientId().c_str());
}

void TrInspector::onClientDisconnected(TrInspectorClient &client)
{
  DEBUG(LOG_TAG_INSPECTOR, "Client disconnected: %s", client.clientId().c_str());
}

int TrInspector::getInspectorCommandChanPort() const
{
  if (inspectorCommandChanServer_ == nullptr)
    return 0;
  return inspectorCommandChanServer_->getPort();
}

void TrInspector::broadcastEventToClients(const std::string &eventJson)
{
  if (!server_)
  {
    DEBUG(LOG_TAG_INSPECTOR, "Inspector server not initialized, cannot broadcast event");
    return;
  }

  for (const auto &client : server_->getClients())
  {
    if (client && client->isWebSocket())
      client->sendWebSocketMessage(eventJson);
  }
}
