#include <chrono>
#include <rapidjson/document.h>
#include <common/debug.hpp>
#include <runtime/content.hpp>
#include <runtime/constellation.hpp>
#include <renderer/renderer.hpp>
#include <renderer/content_renderer.hpp>

#include "./cdp_jsar_universal_rendering_server_domain.hpp"
#include "./inspector_client.hpp"

using namespace std;

CdpJsarUniversalRenderingServerDomain::CdpJsarUniversalRenderingServerDomain(TrConstellation *constellation, const string &clientId)
    : constellation_(constellation)
    , clientId_(clientId)
    , lastEventTime_(chrono::steady_clock::now())
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP: JSAR.UniversalRenderingServer domain initialized for client: %s", clientId_.c_str());
}

CdpJsarUniversalRenderingServerDomain::~CdpJsarUniversalRenderingServerDomain()
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP: JSAR.UniversalRenderingServer domain destroying for client: %s", clientId_.c_str());

  // Unregister callback if it was registered
  if (callbackId_ != -1)
  {
    auto renderer = getRenderer();
    if (renderer)
    {
      renderer->unregisterCommandBufferExecutionCallback(callbackId_);
      DEBUG(LOG_TAG_INSPECTOR, "CDP: Unregistered command buffer callback for client: %s", clientId_.c_str());
    }
  }
}

string CdpJsarUniversalRenderingServerDomain::handleMethod(const string &method, const CdpMessage &message, const string &clientId)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP JSAR.UniversalRenderingServer: Handling method: %s for client: %s", method.c_str(), clientId.c_str());

  if (method == "enableTracing")
  {
    return enableTracing(message, clientId);
  }
  else if (method == "disableTracing")
  {
    return disableTracing(message, clientId);
  }
  else if (method == "setClientFrameRate")
  {
    return setClientFrameRate(message);
  }
  else if (method == "setEventThrottle")
  {
    return setEventThrottle(message);
  }
  else if (method == "getRendererInfo")
  {
    return getRendererInfo(message);
  }
  else if (method == "getContentRenderers")
  {
    return getContentRenderers(message);
  }
  else
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP JSAR.UniversalRenderingServer: Unknown method: %s", method.c_str());
    return CdpResponse::error(message.id, -32601, "JSAR.UniversalRenderingServer." + method + " is not supported");
  }
}

renderer::TrRenderer *CdpJsarUniversalRenderingServerDomain::getRenderer() const
{
  if (!constellation_ || !constellation_->renderer)
    return nullptr;
  return constellation_->renderer.get();
}

string CdpJsarUniversalRenderingServerDomain::enableTracing(const CdpMessage &message, const string &clientId)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP JSAR.UniversalRenderingServer: Enabling tracing and command buffer dispatching for client: %s", clientId.c_str());

  auto *renderer = getRenderer();
  if (!renderer)
  {
    return CdpResponse::error(message.id, -32000, "Renderer not available");
  }

  renderer->enableTracing();
  tracingEnabled_ = true;

  // Register command buffer callback if not already registered
  if (callbackId_ == -1)
  {
    callbackId_ = renderer->registerCommandBufferExecutionCallback(
      [this](const vector<commandbuffers::TrCommandBufferBase *> &commandBuffers, const renderer::TrContentRenderer *contentRenderer)
      {
        this->onCommandBufferExecuted(commandBuffers, contentRenderer);
      });
    DEBUG(LOG_TAG_INSPECTOR, "CDP: Registered command buffer callback (ID: %d) for client: %s", callbackId_, clientId_.c_str());
  }

  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  result.AddMember("success", rapidjson::Value().SetBool(true), allocator);
  result.AddMember("tracingEnabled", rapidjson::Value().SetBool(true), allocator);
  result.AddMember("commandBufferDispatchingEnabled", rapidjson::Value().SetBool(true), allocator);

  return CdpResponse::success(message.id, result);
}

string CdpJsarUniversalRenderingServerDomain::disableTracing(const CdpMessage &message, const string &clientId)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP JSAR.UniversalRenderingServer: Disabling tracing and command buffer dispatching for client: %s", clientId.c_str());

  auto *renderer = getRenderer();
  if (!renderer)
  {
    return CdpResponse::error(message.id, -32000, "Renderer not available");
  }

  renderer->isTracingEnabled = false;
  tracingEnabled_ = false;

  // Unregister command buffer callback if it was registered
  if (callbackId_ != -1)
  {
    renderer->unregisterCommandBufferExecutionCallback(callbackId_);
    DEBUG(LOG_TAG_INSPECTOR, "CDP: Unregistered command buffer callback (ID: %d) for client: %s", callbackId_, clientId_.c_str());
    callbackId_ = -1;
  }

  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  result.AddMember("success", rapidjson::Value().SetBool(true), allocator);
  result.AddMember("tracingEnabled", rapidjson::Value().SetBool(false), allocator);
  result.AddMember("commandBufferDispatchingEnabled", rapidjson::Value().SetBool(false), allocator);

  return CdpResponse::success(message.id, result);
}

string CdpJsarUniversalRenderingServerDomain::setClientFrameRate(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP JSAR.UniversalRenderingServer: Setting client frame rate");

  auto *renderer = getRenderer();
  if (!renderer)
  {
    return CdpResponse::error(message.id, -32000, "Renderer not available");
  }

  // Extract frame rate from params
  if (!message.params.IsObject() || !message.params.HasMember("frameRate"))
  {
    return CdpResponse::error(message.id, -32602, "Missing required parameter: frameRate");
  }

  const auto &frameRateValue = message.params["frameRate"];
  if (!frameRateValue.IsUint())
  {
    return CdpResponse::error(message.id, -32602, "Parameter frameRate must be a positive integer");
  }

  uint32_t frameRate = frameRateValue.GetUint();
  renderer->configureClientFrameRate(frameRate);

  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  result.AddMember("success", rapidjson::Value().SetBool(true), allocator);
  result.AddMember("frameRate", rapidjson::Value().SetUint(renderer->clientDefaultFrameRate), allocator);

  return CdpResponse::success(message.id, result);
}

string CdpJsarUniversalRenderingServerDomain::setEventThrottle(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP JSAR.UniversalRenderingServer: Setting event throttle");

  // Extract throttle interval from params
  if (!message.params.IsObject() || !message.params.HasMember("intervalMs"))
  {
    return CdpResponse::error(message.id, -32602, "Missing required parameter: intervalMs");
  }

  const auto &intervalValue = message.params["intervalMs"];
  if (!intervalValue.IsUint())
  {
    return CdpResponse::error(message.id, -32602, "Parameter intervalMs must be a positive integer");
  }

  uint32_t intervalMs = intervalValue.GetUint();

  // Enforce reasonable limits: 10ms to 10000ms (100 events/sec to 0.1 events/sec)
  if (intervalMs < 10 || intervalMs > 10000)
  {
    return CdpResponse::error(message.id, -32602, "Parameter intervalMs must be between 10 and 10000");
  }

  eventThrottleMs_ = intervalMs;

  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  result.AddMember("success", rapidjson::Value().SetBool(true), allocator);
  result.AddMember("intervalMs", rapidjson::Value().SetUint(eventThrottleMs_), allocator);
  result.AddMember("maxEventsPerSecond", rapidjson::Value().SetDouble(1000.0 / eventThrottleMs_), allocator);

  return CdpResponse::success(message.id, result);
}

string CdpJsarUniversalRenderingServerDomain::getRendererInfo(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP JSAR.UniversalRenderingServer: Getting renderer info");

  auto *renderer = getRenderer();
  if (!renderer)
  {
    return CdpResponse::error(message.id, -32000, "Renderer not available");
  }

  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  result.AddMember("tracingEnabled", rapidjson::Value().SetBool(renderer->isTracingEnabled), allocator);
  result.AddMember("fps", rapidjson::Value().SetUint(renderer->getFps()), allocator);
  result.AddMember("clientFrameRate", rapidjson::Value().SetUint(renderer->clientDefaultFrameRate), allocator);
  result.AddMember("uptime", rapidjson::Value().SetUint(renderer->getUptime()), allocator);
  result.AddMember("stencilClearDisabled", rapidjson::Value().SetBool(renderer->isStencilClearDisabled), allocator);
  result.AddMember("hostContextSummaryEnabled", rapidjson::Value().SetBool(renderer->isHostContextSummaryEnabled), allocator);
  result.AddMember("appContextSummaryEnabled", rapidjson::Value().SetBool(renderer->isAppContextSummaryEnabled), allocator);
  result.AddMember("useDoubleWideFramebuffer", rapidjson::Value().SetBool(renderer->useDoubleWideFramebuffer), allocator);
  result.AddMember("commandBufferPort", rapidjson::Value().SetUint(renderer->getCommandBufferChanPort()), allocator);

  // Add event throttling statistics
  result.AddMember("eventThrottleMs", rapidjson::Value().SetUint(eventThrottleMs_), allocator);
  result.AddMember("totalEventsReceived", rapidjson::Value().SetUint64(totalEventsReceived_), allocator);
  result.AddMember("totalEventsSent", rapidjson::Value().SetUint64(totalEventsSent_), allocator);
  double dropRate = totalEventsReceived_ > 0 ? (1.0 - (double)totalEventsSent_ / totalEventsReceived_) * 100.0 : 0.0;
  result.AddMember("eventDropRate", rapidjson::Value().SetDouble(dropRate), allocator);

  return CdpResponse::success(message.id, result);
}

string CdpJsarUniversalRenderingServerDomain::getContentRenderers(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP JSAR.UniversalRenderingServer: Getting content renderers");

  auto *renderer = getRenderer();
  if (!renderer)
  {
    return CdpResponse::error(message.id, -32000, "Renderer not available");
  }

  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  rapidjson::Value renderers;
  renderers.SetArray();

  // Iterate through all content renderers and collect their information
  renderer->iterateContentRenderers([&](const renderer::TrContentRenderer &contentRenderer)
                                    {
    rapidjson::Value rendererInfo;
    rendererInfo.SetObject();

    // Add basic content renderer information
    rendererInfo.AddMember("contentId", rapidjson::Value().SetUint(contentRenderer.contentId), allocator);
    rendererInfo.AddMember("contextId", rapidjson::Value().SetUint(contentRenderer.contextId), allocator);
    
    // Add runtime information if available
    auto content = contentRenderer.getContent();
    if (content)
    {
      rendererInfo.AddMember("contentPid", rapidjson::Value().SetInt(content->pid), allocator);
      
      rapidjson::Value urlValue;
      urlValue.SetString(content->requestInit.url.c_str(), allocator);
      rendererInfo.AddMember("url", urlValue, allocator);
    }

    renderers.PushBack(rendererInfo, allocator); });

  result.AddMember("contentRenderers", renderers, allocator);

  return CdpResponse::success(message.id, result);
}

void CdpJsarUniversalRenderingServerDomain::setInspectorClient(TrInspectorClient *client)
{
  inspectorClient_ = client;
  DEBUG(LOG_TAG_INSPECTOR, "CDP JSAR.UniversalRenderingServer: Inspector client set for client: %s", clientId_.c_str());
}

void CdpJsarUniversalRenderingServerDomain::onCommandBufferExecuted(const vector<commandbuffers::TrCommandBufferBase *> &commandBuffers, const renderer::TrContentRenderer *contentRenderer)
{
  totalEventsReceived_++;

  if (tracingEnabled_ && inspectorClient_)
  {
    // Check throttling - only send if enough time has passed
    auto now = chrono::steady_clock::now();
    auto timeSinceLastEvent = chrono::duration_cast<chrono::milliseconds>(now - lastEventTime_).count();

    if (timeSinceLastEvent >= static_cast<int64_t>(eventThrottleMs_))
    {
      sendCommandBufferEvent(commandBuffers, contentRenderer);
      lastEventTime_ = now;
      totalEventsSent_++;
    }
    // Events are dropped if throttling condition isn't met
  }
}

void CdpJsarUniversalRenderingServerDomain::sendCommandBufferEvent(const vector<commandbuffers::TrCommandBufferBase *> &commandBuffers, const renderer::TrContentRenderer *contentRenderer)
{
  rapidjson::Document params;
  params.SetObject();
  auto &allocator = params.GetAllocator();

  params.AddMember("timestamp", rapidjson::Value().SetUint64(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count()), allocator);

  // Serialize the command buffers to structured JSON data
  string commandBufferDataStr = serializeCommandBuffers(commandBuffers, contentRenderer);

  // Parse the serialized JSON and include it as structured data
  rapidjson::Document commandBufferJson;
  rapidjson::ParseResult parseResult = commandBufferJson.Parse(commandBufferDataStr.c_str());

  if (parseResult)
  {
    // Copy the parsed command buffer data into the params
    rapidjson::Value commandBufferDataValue;
    commandBufferDataValue.CopyFrom(commandBufferJson, allocator);
    params.AddMember("commandBufferData", commandBufferDataValue, allocator);
  }
  else
  {
    // Fallback to string if parsing fails
    params.AddMember("commandBufferData", rapidjson::Value().SetString(commandBufferDataStr.c_str(), allocator), allocator);
    params.AddMember("parseError", rapidjson::Value().SetBool(true), allocator);
  }

  string eventMessage = CdpResponse::event("JSAR.UniversalRenderingServer.commandBufferExecuted", params);

  // Send to this client's inspector client
  if (inspectorClient_ && inspectorClient_->isWebSocket())
  {
    inspectorClient_->sendWebSocketMessage(eventMessage);
  }
  else
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP JSAR.UniversalRenderingServer: Failed to send - inspectorClient: %p, isWebSocket: %s", inspectorClient_, inspectorClient_ ? (inspectorClient_->isWebSocket() ? "true" : "false") : "null");
  }
}

string CdpJsarUniversalRenderingServerDomain::serializeCommandBuffers(const vector<commandbuffers::TrCommandBufferBase *> &commandBuffers, const renderer::TrContentRenderer *contentRenderer)
{
  rapidjson::Document commandBufferData;
  commandBufferData.SetObject();
  auto &allocator = commandBufferData.GetAllocator();

  // Add metadata
  commandBufferData.AddMember("totalCount", rapidjson::Value().SetUint(commandBuffers.size()), allocator);
  if (contentRenderer)
  {
    commandBufferData.AddMember("contentId", rapidjson::Value().SetUint(contentRenderer->contentId), allocator);
  }

  // Add detailed information for each command buffer using their toJson() method
  rapidjson::Value commandBuffersArray(rapidjson::kArrayType);
  for (size_t i = 0; i < commandBuffers.size(); ++i)
  {
    auto *cmdBuffer = commandBuffers[i];
    if (cmdBuffer)
    {
      rapidjson::Value cmdInfo = cmdBuffer->toJson(allocator);

      // Add sequence index
      cmdInfo.AddMember("sequenceIndex", rapidjson::Value().SetUint(i), allocator);

      commandBuffersArray.PushBack(cmdInfo, allocator);
    }
  }

  commandBufferData.AddMember("commandBuffers", commandBuffersArray, allocator);

  // Convert to string
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  commandBufferData.Accept(writer);

  return buffer.GetString();
}

string CdpJsarUniversalRenderingServerDomain::getDomainName() const
{
  return "JSAR.UniversalRenderingServer";
}

string CdpJsarUniversalRenderingServerDomain::getDomainDescription() const
{
  return "Universal Rendering Server domain for debugging and controlling the unified rendering backend.";
}

vector<CdpCommand> CdpJsarUniversalRenderingServerDomain::getCommands() const
{
  return {
    {"enableTracing", "Enable tracing in TrRenderer and command buffer event dispatching to this CDP client.", nullptr},
    {"disableTracing", "Disable tracing in TrRenderer and command buffer event dispatching to this CDP client.", nullptr},
    {"setClientFrameRate", "Control the client-side FPS in TrRenderer. Requires frameRate parameter.", nullptr},
    {"setEventThrottle", "Control the command buffer event throttling rate. Requires intervalMs parameter (10-10000ms).", nullptr},
    {"getRendererInfo", "Get current renderer state information including FPS, tracing status, and configuration.", nullptr},
    {"getContentRenderers", "Get list of all content renderer instances for debugging.", nullptr}};
}
