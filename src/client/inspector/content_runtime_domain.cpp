#include "./content_runtime_domain.hpp"
#include "runtime/inspector/cdp_handler.hpp" // For CdpMessage and CdpResponse
#include "common/debug.hpp"
#include <rapidjson/document.h>

using namespace std;

ContentCdpRuntimeDomain::ContentCdpRuntimeDomain()
{
  DEBUG(LOG_TAG_INSPECTOR, "Content CDP Runtime: Domain initialized");
}

string ContentCdpRuntimeDomain::handleMethod(const string &method, const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "Content CDP Runtime: Handling method: %s", method.c_str());

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

  return CdpResponse::error(message.id, -32601, "Runtime." + method + " is not supported in content process");
}

string ContentCdpRuntimeDomain::enable(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "Content CDP Runtime: Enabling domain");
  enabled_ = true;

  rapidjson::Document result;
  result.SetObject();
  return CdpResponse::success(message.id, result);
}

string ContentCdpRuntimeDomain::disable(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "Content CDP Runtime: Disabling domain");
  enabled_ = false;

  rapidjson::Document result;
  result.SetObject();
  return CdpResponse::success(message.id, result);
}

string ContentCdpRuntimeDomain::getVersion(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "Content CDP Runtime: Getting version info");

  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  result.AddMember("product", rapidjson::Value("JSAR-Content", allocator), allocator);
  result.AddMember("revision", rapidjson::Value("0.9.0", allocator), allocator);
  result.AddMember("userAgent", rapidjson::Value("JSAR-Content/0.9.0", allocator), allocator);
  result.AddMember("jsVersion", rapidjson::Value("ES2021", allocator), allocator);
  result.AddMember("processType", rapidjson::Value("content", allocator), allocator);

  return CdpResponse::success(message.id, result);
}