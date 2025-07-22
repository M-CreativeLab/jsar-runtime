#include "./cdp_myexample_domain.hpp"
#include "../common/debug.hpp"
#include <rapidjson/document.h>

using namespace std;

CdpMyExampleDomain::CdpMyExampleDomain()
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP: MyExample domain initialized");
}

std::string CdpMyExampleDomain::handleMethod(const std::string &method, const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP MyExample: Handling method: %s", method.c_str());

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
  else
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP MyExample: Unknown method: %s", method.c_str());
    return CdpResponse::error(message.id, -32601, "Example." + method + " is not supported");
  }
}

std::string CdpMyExampleDomain::ping(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP MyExample: Ping called");

  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  result.AddMember("message", rapidjson::Value().SetString("pong", allocator), allocator);
  result.AddMember("timestamp", rapidjson::Value().SetInt64(time(nullptr)), allocator);

  return CdpResponse::success(message.id, result);
}

std::string CdpMyExampleDomain::echo(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP MyExample: Echo called");

  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  // Echo back the params that were sent
  rapidjson::Value paramsCopy;
  paramsCopy.CopyFrom(message.params, allocator);
  result.AddMember("echoed", paramsCopy, allocator);

  return CdpResponse::success(message.id, result);
}

std::string CdpMyExampleDomain::getInfo(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP MyExample: GetInfo called");

  rapidjson::Document result;
  result.SetObject();
  auto &allocator = result.GetAllocator();

  result.AddMember("domain", rapidjson::Value().SetString("Example", allocator), allocator);
  result.AddMember("description", rapidjson::Value().SetString("Sample CDP domain for testing connectivity", allocator), allocator);
  result.AddMember("version", rapidjson::Value().SetString("1.0", allocator), allocator);

  rapidjson::Value methods;
  methods.SetArray();
  methods.PushBack(rapidjson::Value().SetString("ping", allocator), allocator);
  methods.PushBack(rapidjson::Value().SetString("echo", allocator), allocator);
  methods.PushBack(rapidjson::Value().SetString("getInfo", allocator), allocator);

  result.AddMember("methods", methods, allocator);

  return CdpResponse::success(message.id, result);
}

void CdpMyExampleDomain::addProtocolDefinition(rapidjson::Value &domains, rapidjson::Document::AllocatorType &allocator)
{
  rapidjson::Value exampleDomain;
  exampleDomain.SetObject();
  exampleDomain.AddMember("domain", rapidjson::Value().SetString("Example", allocator), allocator);
  exampleDomain.AddMember("description", rapidjson::Value().SetString("Sample domain for testing CDP connectivity and method invocation.", allocator), allocator);
  
  rapidjson::Value commands;
  commands.SetArray();
  
  // Example.ping
  rapidjson::Value pingCmd;
  pingCmd.SetObject();
  pingCmd.AddMember("name", rapidjson::Value().SetString("ping", allocator), allocator);
  pingCmd.AddMember("description", rapidjson::Value().SetString("Simple ping command that responds with pong.", allocator), allocator);
  commands.PushBack(pingCmd, allocator);
  
  // Example.echo
  rapidjson::Value echoCmd;
  echoCmd.SetObject();
  echoCmd.AddMember("name", rapidjson::Value().SetString("echo", allocator), allocator);
  echoCmd.AddMember("description", rapidjson::Value().SetString("Echoes back the provided parameters.", allocator), allocator);
  commands.PushBack(echoCmd, allocator);
  
  // Example.getInfo
  rapidjson::Value getInfoCmd;
  getInfoCmd.SetObject();
  getInfoCmd.AddMember("name", rapidjson::Value().SetString("getInfo", allocator), allocator);
  getInfoCmd.AddMember("description", rapidjson::Value().SetString("Returns information about the Example domain.", allocator), allocator);
  commands.PushBack(getInfoCmd, allocator);
  
  exampleDomain.AddMember("commands", commands, allocator);
  domains.PushBack(exampleDomain, allocator);
}