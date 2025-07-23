#include "./cdp_runtime_domain.hpp"
#include "../constellation.hpp"
#include "../embedder.hpp"
#include "../../common/debug.hpp"
#include <rapidjson/document.h>

using namespace std;

CdpRuntimeDomain::CdpRuntimeDomain(TrConstellation *constellation)
    : constellation_(constellation)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP: Runtime domain initialized");
}

string CdpRuntimeDomain::handleMethod(const string &method, const CdpMessage &message, const string &clientId)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP Runtime: Handling method: %s for client: %s", method.c_str(), clientId.c_str());

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

string CdpRuntimeDomain::enable(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP Runtime: Enabling runtime domain");

  // Create empty result for enable command
  rapidjson::Document result;
  result.SetObject();

  return CdpResponse::success(message.id, result);
}

string CdpRuntimeDomain::disable(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP Runtime: Disabling runtime domain");

  // Create empty result for disable command
  rapidjson::Document result;
  result.SetObject();

  return CdpResponse::success(message.id, result);
}

string CdpRuntimeDomain::getVersion(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP Runtime: Getting version");

  auto embedder = constellation_->getEmbedder();
  string version = embedder->getVersion();

  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  result.AddMember("product", rapidjson::Value().SetString("JSAR", allocator), allocator);
  result.AddMember("revision", rapidjson::Value().SetString(version.c_str(), allocator), allocator);
  result.AddMember("userAgent", rapidjson::Value().SetString(("JSAR/" + version).c_str(), allocator), allocator);
  result.AddMember("jsVersion", rapidjson::Value().SetString("ES2021", allocator), allocator);

  return CdpResponse::success(message.id, result);
}

string CdpRuntimeDomain::evaluate(const CdpMessage &message)
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

string CdpRuntimeDomain::getDomainName() const
{
  return "Runtime";
}

string CdpRuntimeDomain::getDomainDescription() const
{
  return "Runtime domain exposes JavaScript runtime by means of remote evaluation and mirror objects.";
}

vector<CdpCommand> CdpRuntimeDomain::getCommands() const
{
  return {
    {"enable", "Enables reporting of execution contexts creation.", nullptr},
    {"disable", "Disables reporting of execution contexts creation.", nullptr},
    {"getVersion", "Returns the JavaScript runtime version information.", nullptr},
    {"evaluate", "Evaluates expression on global object.", nullptr}
  };
}
