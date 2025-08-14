#include "./content_example_domain.hpp"
#include "runtime/inspector/cdp_handler.hpp" // For CdpMessage and CdpResponse
#include "common/debug.hpp"
#include <rapidjson/document.h>
#include <chrono>

using namespace std;

ContentCdpExampleDomain::ContentCdpExampleDomain()
{
  DEBUG(LOG_TAG_INSPECTOR, "Content CDP Example: Domain initialized");
}

string ContentCdpExampleDomain::handleMethod(const string &method, const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "Content CDP Example: Handling method: %s", method.c_str());

  if (method == "ping")
  {
    return ping(message);
  }
  else if (method == "echo")
  {
    return echo(message);
  }
  else if (method == "getInfo")
  {
    return getInfo(message);
  }

  return CdpResponse::error(message.id, -32601, "Example." + method + " is not supported in content process");
}

string ContentCdpExampleDomain::ping(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "Content CDP Example: Ping received");

  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  result.AddMember("message", rapidjson::Value("pong from content process", allocator), allocator);

  auto now = chrono::system_clock::now();
  auto timestamp = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
  result.AddMember("timestamp", rapidjson::Value().SetInt64(timestamp), allocator);

  return CdpResponse::success(message.id, result);
}

string ContentCdpExampleDomain::echo(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "Content CDP Example: Echo received");

  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  // Echo back the parameters
  rapidjson::Value echoed(rapidjson::kObjectType);
  echoed.CopyFrom(message.params, allocator);

  result.AddMember("echoed", echoed, allocator);
  result.AddMember("source", rapidjson::Value("content-process", allocator), allocator);

  return CdpResponse::success(message.id, result);
}

string ContentCdpExampleDomain::getInfo(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "Content CDP Example: GetInfo received");

  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  result.AddMember("domain", rapidjson::Value("Example", allocator), allocator);
  result.AddMember("description", rapidjson::Value("Content-side example domain for testing CDP connectivity", allocator), allocator);
  result.AddMember("processType", rapidjson::Value("content", allocator), allocator);
  result.AddMember("capabilities", rapidjson::Value("ping, echo, getInfo", allocator), allocator);

  return CdpResponse::success(message.id, result);
}