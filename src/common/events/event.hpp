#pragma once

#include <string>
#include <vector>
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

  enum class TrXSMLEventType
  {
    // Lifecycle Events
    SpawnProcess,
    BeforeScripting,
    BeforeLoading,
    Loaded, // TODO: deprecated
    Load,
    DOMContentLoaded,
    Error,
    // Metrics Events
    FCP,
    LCP,
    // Unknown
    Unknown
  };

  class TrXSMLEvent
  {
  public:
    /**
     * Create a new XSMLEvent from the given source document(JSON).
     */
    TrXSMLEvent(rapidjson::Document &sourceDoc)
    {
      id = sourceDoc["id"].GetUint();
      auto typeStr = sourceDoc["eventType"].GetString();
      if (strcmp(typeStr, "spawnprocess") == 0)
        type = TrXSMLEventType::SpawnProcess;
      else if (strcmp(typeStr, "beforescripting") == 0)
        type = TrXSMLEventType::BeforeScripting;
      else if (strcmp(typeStr, "beforeloading") == 0)
        type = TrXSMLEventType::BeforeLoading;
      else if (strcmp(typeStr, "loaded") == 0)
        type = TrXSMLEventType::Loaded;
      else if (strcmp(typeStr, "load") == 0)
        type = TrXSMLEventType::Load;
      else if (strcmp(typeStr, "DOMContentLoaded") == 0)
        type = TrXSMLEventType::DOMContentLoaded;
      else if (strcmp(typeStr, "fcp") == 0)
        type = TrXSMLEventType::FCP;
      else if (strcmp(typeStr, "lcp") == 0)
        type = TrXSMLEventType::LCP;
      else if (strcmp(typeStr, "error") == 0)
        type = TrXSMLEventType::Error;
    }
    /**
     * Create a new XSMLEvent with the given id and type.
     */
    TrXSMLEvent(int id, TrXSMLEventType type) : id(id), type(type)
    {
    }

  public:
    string serialize()
    {
      rapidjson::Document doc;
      doc.SetObject();
      auto &allocator = doc.GetAllocator();
      doc.AddMember("id", id, allocator);
      auto eventTypeValue = rapidjson::Value(typeToString(type).c_str(), allocator);
      doc.AddMember("eventType", eventTypeValue, allocator);

      rapidjson::StringBuffer buffer;
      rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
      doc.Accept(writer);
      return buffer.GetString();
    }
    string toString()
    {
      return "XSMLEvent(" + typeToString(type) + ")";
    }

  private:
    string typeToString(TrXSMLEventType type)
    {
      switch (type)
      {
      case TrXSMLEventType::SpawnProcess:
        return "spawnprocess";
      case TrXSMLEventType::BeforeScripting:
        return "beforescripting";
      case TrXSMLEventType::BeforeLoading:
        return "beforeloading";
      case TrXSMLEventType::Loaded:
        return "loaded";
      case TrXSMLEventType::Load:
        return "load";
      case TrXSMLEventType::DOMContentLoaded:
        return "DOMContentLoaded";
      case TrXSMLEventType::FCP:
        return "fcp";
      case TrXSMLEventType::LCP:
        return "lcp";
      case TrXSMLEventType::Error:
        return "error";
      default:
        return "unknown";
      }
    }

  public:
    int id;
    TrXSMLEventType type = TrXSMLEventType::Unknown;
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
    string &getString() { return jsonSource; }
    size_t size() { return jsonSource.size(); }

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
    static TrEvent MakeXSMLEvent(TrXSMLEvent xsmlEvent)
    {
      return TrEvent(TrEventType::TR_EVENT_XSML_EVENT, xsmlEvent.serialize());
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
