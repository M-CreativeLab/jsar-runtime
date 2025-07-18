#include <memory>
#include <rapidjson/document.h>
#include <renderer/renderer.hpp>

#include "./inspector.hpp"
#include "./constellation.hpp"
#include "./content_manager.hpp"
#include "./embedder.hpp"

using namespace std;
using namespace std::placeholders;

void TrInspector::initialize()
{
  server_ = make_unique<TrInspectorServer>(shared_from_this());
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

  if (requestUrl == "/json/version")
  {
    handleRequest(std::bind(&TrInspector::getVersion, this, _1), requestClient);
  }
  else if (requestUrl == "/contents" ||
           requestUrl == "/json" ||
           requestUrl == "/json/list")
  {
    handleRequest(std::bind(&TrInspector::getContents, this, _1), requestClient);
  }
  else if (requestUrl == "/json/protocol")
  {
    handleRequest(std::bind(&TrInspector::getProtocol, this, _1), requestClient);
  }
  else if (requestUrl == "/json/statistics")
  {
    handleRequest(std::bind(&TrInspector::getStatistics, this, _1), requestClient);
  }
  else
  {
    requestClient.respond(404, "Not Found");
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
  catch (const std::exception &e)
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
    string debuggerUrl = "ws://localhost:" + to_string(requestInit.inspectorPort()) + "/devtools/inspector/" + id;
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
      rapidjson::Value requestInitJson;
      requestInitJson.SetObject();
      requestInitJson.AddMember("url",
                                rapidjson::Value().SetString(requestInit.url.c_str(), allocator),
                                allocator);
      requestInitJson.AddMember("disableCache", requestInit.disableCache, allocator);
      contentJson.AddMember("requestInit", requestInitJson, allocator);
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
  {
    // TODO: Add the domains
  }

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

void TrInspector::onMessage(TrInspectorClient &client, const string &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "Received WebSocket message: %s", message.c_str());

  // For now, just echo the message back (placeholder for CDP implementation)
  client.sendWebSocketMessage("Echo: " + message);
}
