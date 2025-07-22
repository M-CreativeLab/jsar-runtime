#include "./cdp_runtime_domain.hpp"
#include "../constellation.hpp"
#include "../embedder.hpp"
#include "../common/debug.hpp"
#include <rapidjson/document.h>

using namespace std;

CdpRuntimeDomain::CdpRuntimeDomain(TrConstellation *constellation)
    : constellation_(constellation)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP: Runtime domain initialized");
}

std::string CdpRuntimeDomain::handleMethod(const std::string &method, const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP Runtime: Handling method: %s", method.c_str());

  if (method == "enable")
  {
    return enable(message);
  }
  else if (method == "disable")
  {
    return disable(message);
  }
  else if (method == "getVersion")
  {
    return getVersion(message);
  }
  else if (method == "evaluate")
  {
    return evaluate(message);
  }
  else
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP Runtime: Unknown method: %s", method.c_str());
    return CdpResponse::error(message.id, -32601, "Runtime." + method + " is not supported");
  }
}

std::string CdpRuntimeDomain::enable(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP Runtime: Enabling runtime domain");

  // Create empty result for enable command
  rapidjson::Document result;
  result.SetObject();

  return CdpResponse::success(message.id, result);
}

std::string CdpRuntimeDomain::disable(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP Runtime: Disabling runtime domain");

  // Create empty result for disable command
  rapidjson::Document result;
  result.SetObject();

  return CdpResponse::success(message.id, result);
}

std::string CdpRuntimeDomain::getVersion(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP Runtime: Getting version");

  auto embedder = constellation_->getEmbedder();
  std::string version = embedder->getVersion();

  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  result.AddMember("product", rapidjson::Value().SetString("JSAR", allocator), allocator);
  result.AddMember("revision", rapidjson::Value().SetString(version.c_str(), allocator), allocator);
  result.AddMember("userAgent", rapidjson::Value().SetString(("JSAR/" + version).c_str(), allocator), allocator);
  result.AddMember("jsVersion", rapidjson::Value().SetString("ES2021", allocator), allocator);

  return CdpResponse::success(message.id, result);
}

std::string CdpRuntimeDomain::evaluate(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP Runtime: Evaluate method called (not implemented)");

  // For now, just return a placeholder response
  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  rapidjson::Value resultValue;
  resultValue.SetObject();
  resultValue.AddMember("type", rapidjson::Value().SetString("string", allocator), allocator);
  resultValue.AddMember("value", rapidjson::Value().SetString("Evaluate not implemented in JSAR", allocator), allocator);

  result.AddMember("result", resultValue, allocator);

  return CdpResponse::success(message.id, result);
}

void CdpRuntimeDomain::addProtocolDefinition(rapidjson::Value &domains, rapidjson::Document::AllocatorType &allocator)
{
  rapidjson::Value runtimeDomain;
  runtimeDomain.SetObject();
  runtimeDomain.AddMember("domain", rapidjson::Value().SetString("Runtime", allocator), allocator);
  runtimeDomain.AddMember("description", rapidjson::Value().SetString("Runtime domain exposes JavaScript runtime by means of remote evaluation and mirror objects.", allocator), allocator);
  
  rapidjson::Value commands;
  commands.SetArray();
  
  // Runtime.enable
  rapidjson::Value enableCmd;
  enableCmd.SetObject();
  enableCmd.AddMember("name", rapidjson::Value().SetString("enable", allocator), allocator);
  enableCmd.AddMember("description", rapidjson::Value().SetString("Enables reporting of execution contexts creation.", allocator), allocator);
  commands.PushBack(enableCmd, allocator);
  
  // Runtime.disable
  rapidjson::Value disableCmd;
  disableCmd.SetObject();
  disableCmd.AddMember("name", rapidjson::Value().SetString("disable", allocator), allocator);
  disableCmd.AddMember("description", rapidjson::Value().SetString("Disables reporting of execution contexts creation.", allocator), allocator);
  commands.PushBack(disableCmd, allocator);
  
  // Runtime.getVersion
  rapidjson::Value versionCmd;
  versionCmd.SetObject();
  versionCmd.AddMember("name", rapidjson::Value().SetString("getVersion", allocator), allocator);
  versionCmd.AddMember("description", rapidjson::Value().SetString("Returns the JavaScript runtime version information.", allocator), allocator);
  commands.PushBack(versionCmd, allocator);
  
  // Runtime.evaluate
  rapidjson::Value evaluateCmd;
  evaluateCmd.SetObject();
  evaluateCmd.AddMember("name", rapidjson::Value().SetString("evaluate", allocator), allocator);
  evaluateCmd.AddMember("description", rapidjson::Value().SetString("Evaluates expression on global object.", allocator), allocator);
  commands.PushBack(evaluateCmd, allocator);
  
  runtimeDomain.AddMember("commands", commands, allocator);
  domains.PushBack(runtimeDomain, allocator);
}