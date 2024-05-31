#pragma once

#include <string>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "idgen.hpp"
#include "./classes.hpp"
#include "./event_type.hpp"

using namespace std;

namespace events
{
  class TrRpcRequest
  {
  public:
    TrRpcRequest(rapidjson::Document &sourceDoc)
    {
      method = sourceDoc["method"].GetString();
      if (sourceDoc.HasMember("args") && sourceDoc["args"].IsArray())
      {
        auto &argsArray = sourceDoc["args"];
        for (auto &arg : argsArray.GetArray())
        {
          if (arg.IsString())
            args.push_back(arg.GetString());
          else
            args.push_back("");
        }
      }
    }

  public:
    string method;
    vector<string> args;
  };

  class TrRpcResponse
  {
  public:
    static TrRpcResponse MakeErrorResponse(string errorMessage)
    {
      TrRpcResponse res;
      res.success = false;
      res.errorMessage = errorMessage;
      return res;
    }

  public:
    TrRpcResponse()
    {
      doc.SetObject();
    }

  public:
    void makeSuccess()
    {
      success = true;
    }
    void makeError(string errorMessage)
    {
      success = false;
      this->errorMessage = errorMessage;
    }
    string serialize()
    {
      auto &allocator = doc.GetAllocator();
      doc.AddMember("success", success, allocator);
      if (!success)
      {
        auto errorMessageValue = rapidjson::Value(errorMessage.c_str(), allocator);
        doc.AddMember("errorMessage", errorMessageValue, allocator);
      }
      if (dataValue.IsObject())
        doc.AddMember("data", dataValue, allocator);

      rapidjson::StringBuffer buffer;
      rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
      doc.Accept(writer);
      return buffer.GetString();
    }

  protected:
    rapidjson::Document doc;
    rapidjson::Value dataValue;

  public:
    bool success;
    string errorMessage;
  };

  class TrXSMLRequestInit
  {
  public:
    TrXSMLRequestInit(string url, uint32_t id) : url(url), id(id)
    {
    }
    TrXSMLRequestInit(rapidjson::Document &sourceDoc)
    {
      url = sourceDoc["url"].GetString();
      id = sourceDoc["sessionId"].GetUint();
      if (sourceDoc.HasMember("disableCache"))
        disableCache = sourceDoc["disableCache"].GetBool();
      if (sourceDoc.HasMember("isPreview"))
        isPreview = sourceDoc["isPreview"].GetBool();
      if (sourceDoc.HasMember("runScripts"))
        runScripts = sourceDoc["runScripts"].GetString();
    }
    TrXSMLRequestInit(const TrXSMLRequestInit &that)
        : url(that.url),
          id(that.id),
          disableCache(that.disableCache),
          isPreview(that.isPreview),
          runScripts(that.runScripts)
    {
    }

  public:
    string url;
    uint32_t id;
    bool disableCache;
    bool isPreview;
    string runScripts;
  };

  class TrEventDetail
  {
  public:
    TrEventDetail()
    {
    }
    TrEventDetail(string jsonSource) : jsonSource(jsonSource)
    {
    }
    ~TrEventDetail()
    {
    }

  public:
    string &getString()
    {
      return jsonSource;
    }

    template <typename T>
    T get()
    {
      rapidjson::Document jsonDoc;
      jsonDoc.Parse(jsonSource.c_str());
      return T(jsonDoc);
    }

  private:
    string jsonSource;
  };

  static TrIdGenerator eventIdGenerator(1);
  class TrEvent
  {
  public:
    template <typename T>
    static TrEvent MakeRpcResponseEvent(TrEvent &requestEvent, T &res)
    {
      auto id = requestEvent.id;
      auto type = TrEventType::TR_EVENT_RPC_RESPONSE;
      auto detailData = res.serialize();
      return TrEvent(id, type, detailData);
    }

  public:
    TrEvent()
    {
    }
    TrEvent(TrEventType type, string detailData)
        : id(eventIdGenerator.get()), type(type), detail(TrEventDetail(detailData))
    {
    }
    TrEvent(int id, TrEventType type, string detailData)
        : id(id), type(type), detail(TrEventDetail(detailData))
    {
    }

  public:
    int id;
    TrEventType type;
    TrEventDetail detail;
  };
}
