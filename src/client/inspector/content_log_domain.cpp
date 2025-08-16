#include "./content_log_domain.hpp"
#include "runtime/inspector/cdp_handler.hpp" // For CdpMessage and CdpResponse
#include "common/debug.hpp"
#include <rapidjson/document.h>
#include <chrono>

using namespace std;

ContentCdpLogDomain::ContentCdpLogDomain()
{
  DEBUG(LOG_TAG_INSPECTOR, "Content CDP Log: Domain initialized");
}

string ContentCdpLogDomain::handleMethod(const string &method, const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "Content CDP Log: Handling method: %s", method.c_str());

  if (method == "enable")
  {
    return enable(message);
  }
  else if (method == "disable")
  {
    return disable(message);
  }
  else if (method == "clear")
  {
    return clear(message);
  }
  else if (method == "startViolationsReport")
  {
    return startViolationsReport(message);
  }
  else if (method == "stopViolationsReport")
  {
    return stopViolationsReport(message);
  }

  return CdpResponse::error(message.id, -32601, "Log." + method + " is not supported in content process");
}

string ContentCdpLogDomain::enable(const CdpMessage &message)
{
  enabled_ = true;

  // Send any buffered log entries as events
  for (const auto &entry : logBuffer_)
  {
    string eventJson = createLogEntryEvent(entry);
    if (!sendEvent(eventJson))
      cerr << "Content CDP Log: Failed to send buffered entry event: " << entry.text << endl;
  }

  rapidjson::Document result;
  result.SetObject();
  return CdpResponse::success(message.id, result);
}

string ContentCdpLogDomain::disable(const CdpMessage &message)
{
  enabled_ = false;
  violationsReportEnabled_ = false;
  violationTypes_.clear();

  rapidjson::Document result;
  result.SetObject();
  return CdpResponse::success(message.id, result);
}

string ContentCdpLogDomain::clear(const CdpMessage &message)
{
  logBuffer_.clear();

  rapidjson::Document result;
  result.SetObject();
  return CdpResponse::success(message.id, result);
}

string ContentCdpLogDomain::startViolationsReport(const CdpMessage &message)
{
  violationsReportEnabled_ = true;
  violationTypes_.clear();

  // Extract config from params if present
  if (message.params.IsObject() && message.params.HasMember("config"))
  {
    const auto &config = message.params["config"];
    if (config.IsArray())
    {
      for (const auto &configItem : config.GetArray())
      {
        if (configItem.IsObject() && configItem.HasMember("name"))
        {
          if (configItem["name"].IsString())
          {
            violationTypes_.push_back(configItem["name"].GetString());
            DEBUG(LOG_TAG_INSPECTOR, "Content CDP Log: Added violation type: %s", configItem["name"].GetString());
          }
        }
      }
    }
  }

  // If no specific types configured, enable all common violation types
  if (violationTypes_.empty())
  {
    violationTypes_ = {
      "longTask",
      "longLayout",
      "blockedEvent",
      "blockedParser",
      "discouragedAPIUse",
      "handler",
      "recurringHandler"};
  }

  rapidjson::Document result;
  result.SetObject();
  return CdpResponse::success(message.id, result);
}

string ContentCdpLogDomain::stopViolationsReport(const CdpMessage &message)
{
  DEBUG(LOG_TAG_INSPECTOR, "Content CDP Log: Stopping violations report");
  violationsReportEnabled_ = false;
  violationTypes_.clear();

  rapidjson::Document result;
  result.SetObject();
  return CdpResponse::success(message.id, result);
}

void ContentCdpLogDomain::addLogEntry(const LogEntry &entry)
{
  // Store in buffer
  logBuffer_.push_back(entry);

  // Keep buffer size reasonable (last 1000 entries)
  if (logBuffer_.size() > 1000)
    logBuffer_.erase(logBuffer_.begin());

  if (enabled_)
  {
    // Send Log.entryAdded event to the host process
    string eventJson = createLogEntryEvent(entry);
    if (!sendEvent(eventJson))
      cerr << "Content CDP Log: Failed to send Log.entryAdded event: " << entry.text << endl;
  }
}

string ContentCdpLogDomain::createLogEntryEvent(const LogEntry &entry)
{
  rapidjson::Document event;
  event.SetObject();
  auto &allocator = event.GetAllocator();

  event.AddMember("method", rapidjson::Value("Log.entryAdded", allocator), allocator);

  rapidjson::Value params(rapidjson::kObjectType);
  rapidjson::Value entryJson = logEntryToJson(entry, allocator);
  params.AddMember("entry", entryJson, allocator);
  event.AddMember("params", params, allocator);

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  event.Accept(writer);

  return buffer.GetString();
}

rapidjson::Value ContentCdpLogDomain::logEntryToJson(const LogEntry &entry, rapidjson::Document::AllocatorType &allocator)
{
  rapidjson::Value entryJson(rapidjson::kObjectType);

  entryJson.AddMember("source", rapidjson::Value(entry.source.c_str(), allocator), allocator);
  entryJson.AddMember("level", rapidjson::Value(entry.level.c_str(), allocator), allocator);
  entryJson.AddMember("text", rapidjson::Value(entry.text.c_str(), allocator), allocator);
  entryJson.AddMember("timestamp", rapidjson::Value(entry.timestamp), allocator);

  if (!entry.category.empty())
  {
    entryJson.AddMember("category", rapidjson::Value(entry.category.c_str(), allocator), allocator);
  }

  if (!entry.url.empty())
  {
    entryJson.AddMember("url", rapidjson::Value(entry.url.c_str(), allocator), allocator);
  }

  if (entry.lineNumber > 0)
  {
    entryJson.AddMember("lineNumber", rapidjson::Value(entry.lineNumber), allocator);
  }

  if (entry.stackTrace > 0)
  {
    entryJson.AddMember("stackTrace", rapidjson::Value(entry.stackTrace), allocator);
  }

  if (entry.networkRequestId > 0)
  {
    entryJson.AddMember("networkRequestId", rapidjson::Value(entry.networkRequestId), allocator);
  }

  if (entry.workerId > 0)
  {
    entryJson.AddMember("workerId", rapidjson::Value(entry.workerId), allocator);
  }

  if (!entry.args.empty())
  {
    rapidjson::Value argsArray(rapidjson::kArrayType);
    for (const auto &arg : entry.args)
    {
      argsArray.PushBack(rapidjson::Value(arg.c_str(), allocator), allocator);
    }
    entryJson.AddMember("args", argsArray, allocator);
  }

  return entryJson;
}