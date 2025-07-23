#include "./cdp_jsar_universal_rendering_server_domain.hpp"
#include "../constellation.hpp"
#include "../../common/debug.hpp"
#include "../../renderer/renderer.hpp"
#include "../../renderer/content_renderer.hpp"
#include <rapidjson/document.h>

using namespace std;

CdpJsarUniversalRenderingServerDomain::CdpJsarUniversalRenderingServerDomain(TrConstellation *constellation)
    : constellation_(constellation)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP: JSAR.UniversalRenderingServer domain initialized");
}

string CdpJsarUniversalRenderingServerDomain::handleMethod(const string &method, const CdpMessage &message, const string &clientId)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP JSAR.UniversalRenderingServer: Handling method: %s for client: %s", method.c_str(), clientId.c_str());

  if (method == "enableTracing")
  {
    return enableTracing(message);
  }
  else if (method == "disableTracing")
  {
    return disableTracing(message);
  }
  else if (method == "setClientFrameRate")
  {
    return setClientFrameRate(message);
  }
  else if (method == "getRendererInfo")
  {
    return getRendererInfo(message);
  }
  else if (method == "getContentRenderers")
  {
    return getContentRenderers(message);
  }
  else if (method == "getCommandBuffers")
  {
    return getCommandBuffers(message);
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
  {
    return nullptr;
  }
  return constellation_->renderer.get();
}

string CdpJsarUniversalRenderingServerDomain::enableTracing(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP JSAR.UniversalRenderingServer: Enabling tracing");

  auto *renderer = getRenderer();
  if (!renderer)
  {
    return CdpResponse::error(message.id, -32000, "Renderer not available");
  }

  renderer->enableTracing();

  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  result.AddMember("success", rapidjson::Value().SetBool(true), allocator);
  result.AddMember("tracingEnabled", rapidjson::Value().SetBool(true), allocator);

  return CdpResponse::success(message.id, result);
}

string CdpJsarUniversalRenderingServerDomain::disableTracing(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP JSAR.UniversalRenderingServer: Disabling tracing");

  auto *renderer = getRenderer();
  if (!renderer)
  {
    return CdpResponse::error(message.id, -32000, "Renderer not available");
  }

  renderer->isTracingEnabled = false;

  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  result.AddMember("success", rapidjson::Value().SetBool(true), allocator);
  result.AddMember("tracingEnabled", rapidjson::Value().SetBool(false), allocator);

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
  renderer->iterateContentRenderers([&](const renderer::TrContentRenderer &contentRenderer) {
    rapidjson::Value rendererInfo;
    rendererInfo.SetObject();

    // Add basic content renderer information
    rendererInfo.AddMember("contentId", rapidjson::Value().SetUint(contentRenderer.contentId), allocator);
    rendererInfo.AddMember("contextId", rapidjson::Value().SetUint(contentRenderer.contextId), allocator);
    
    // Add runtime information if available
    auto content = contentRenderer.getContent();
    if (content)
    {
      rendererInfo.AddMember("contentPid", rapidjson::Value().SetInt(content->getPid()), allocator);
      
      rapidjson::Value urlValue;
      urlValue.SetString(content->getUrl().c_str(), allocator);
      rendererInfo.AddMember("url", urlValue, allocator);
    }

    renderers.PushBack(rendererInfo, allocator);
  });

  result.AddMember("contentRenderers", renderers, allocator);

  return CdpResponse::success(message.id, result);
}

string CdpJsarUniversalRenderingServerDomain::getCommandBuffers(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP JSAR.UniversalRenderingServer: Getting command buffers");

  auto *renderer = getRenderer();
  if (!renderer)
  {
    return CdpResponse::error(message.id, -32000, "Renderer not available");
  }

  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  // Add command buffer channel information
  result.AddMember("commandBufferPort", rapidjson::Value().SetUint(renderer->getCommandBufferChanPort()), allocator);
  
  rapidjson::Value commandBufferInfo;
  commandBufferInfo.SetObject();
  commandBufferInfo.AddMember("description", 
    rapidjson::Value().SetString("Command buffers can be inspected by connecting to the command buffer channel port", allocator), 
    allocator);
  commandBufferInfo.AddMember("note", 
    rapidjson::Value().SetString("Use the commandBufferPort to establish IPC connection for detailed command buffer inspection", allocator), 
    allocator);

  result.AddMember("commandBuffers", commandBufferInfo, allocator);

  return CdpResponse::success(message.id, result);
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
    {"enableTracing", "Enable tracing in TrRenderer.", nullptr},
    {"disableTracing", "Disable tracing in TrRenderer.", nullptr},
    {"setClientFrameRate", "Control the client-side FPS in TrRenderer. Requires frameRate parameter.", nullptr},
    {"getRendererInfo", "Get current renderer state information including FPS, tracing status, and configuration.", nullptr},
    {"getContentRenderers", "Get list of all content renderer instances for debugging.", nullptr},
    {"getCommandBuffers", "Get command buffer debugging information and inspection details.", nullptr}
  };
}