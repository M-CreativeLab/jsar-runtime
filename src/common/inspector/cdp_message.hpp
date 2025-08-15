#pragma once

#include <string>
#include <memory>
#include <functional>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/error/en.h>

// CDP (Chrome DevTools Protocol) Message Structure
struct CdpMessage
{
  int64_t id = -1;
  std::string method;
  rapidjson::Value params;

  static std::unique_ptr<CdpMessage> parse(const std::string &json)
  {
    auto message = std::make_unique<CdpMessage>();

    rapidjson::Document doc;
    doc.Parse(json.c_str());

    if (doc.HasParseError())
    {
      return nullptr;
    }

    // Extract id (optional for events)
    if (doc.HasMember("id"))
    {
      if (doc["id"].IsInt64())
      {
        message->id = doc["id"].GetInt64();
      }
      else if (doc["id"].IsInt())
      {
        message->id = doc["id"].GetInt();
      }
    }

    // Extract method
    if (doc.HasMember("method") && doc["method"].IsString())
    {
      message->method = doc["method"].GetString();
    }

    // Extract params (optional)
    if (doc.HasMember("params"))
    {
      message->params.CopyFrom(doc["params"], doc.GetAllocator());
    }

    return message;
  }
};

// CDP Response Builder
class CdpResponse
{
public:
  static std::string success(int64_t id, const rapidjson::Value &result)
  {
    rapidjson::Document doc;
    doc.SetObject();
    auto &allocator = doc.GetAllocator();

    doc.AddMember("id", id, allocator);
    rapidjson::Value resultCopy;
    resultCopy.CopyFrom(result, allocator);
    doc.AddMember("result", resultCopy, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    return buffer.GetString();
  }

  static std::string error(int64_t id, int code, const std::string &message)
  {
    rapidjson::Document doc;
    doc.SetObject();
    auto &allocator = doc.GetAllocator();

    doc.AddMember("id", id, allocator);

    rapidjson::Value errorObj(rapidjson::kObjectType);
    errorObj.AddMember("code", code, allocator);
    errorObj.AddMember("message", rapidjson::Value(message.c_str(), allocator), allocator);

    doc.AddMember("error", errorObj, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    return buffer.GetString();
  }

  static std::string event(const std::string &method, const rapidjson::Value &params)
  {
    rapidjson::Document doc;
    doc.SetObject();
    auto &allocator = doc.GetAllocator();

    doc.AddMember("method", rapidjson::Value(method.c_str(), allocator), allocator);
    rapidjson::Value paramsCopy;
    paramsCopy.CopyFrom(params, allocator);
    doc.AddMember("params", paramsCopy, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    return buffer.GetString();
  }
};

// CDP Command Definition
struct CdpCommand
{
  std::string name;
  std::string description;
  std::function<std::string(const CdpMessage &)> handler;
};