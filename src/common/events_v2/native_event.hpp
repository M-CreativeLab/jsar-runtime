#pragma once

#include <chrono>
#include <string>

#include "common/options.hpp"
#include "./event.hpp"
#include "./event_target.hpp"
#include "./event_listener.hpp"

using namespace std;

namespace events_comm
{
  /**
   * The Native Events are used to communicate between the script process and the renderer process.
   *
   * `RpcRequest`: send a request to the peer process.
   * `RpcResponse`: send a response to the peer process.
   * `DocumentRequest`: send a document request to the script process.
   * `DocumentEvent`: report a document event to the renderer process.
   */

#define TR_NATIVE_EVENTS_MAP(XX) \
  XX(RpcRequest)                 \
  XX(RpcResponse)                \
  XX(DocumentRequest)            \
  XX(DocumentEvent)

  enum class TrNativeEventType
  {
    Unset = 0,
    Unknown,

#define XX(eventType) eventType,
    TR_NATIVE_EVENTS_MAP(XX)
#undef XX
  };

  using TrNativeEvent = TrEvent<TrNativeEventType>;
  using TrNativeEventTarget = TrEventTarget<TrNativeEventType>;
  using TrNativeEventListener = TrEventListener<TrNativeEventType, TrNativeEvent>;

  class TrRpcRequest : public TrEventDetailObject
  {
  public:
    TrRpcRequest() = default;
    TrRpcRequest(uint32_t documentId, string method, vector<string> args)
        : documentId(documentId), method(method), args(args)
    {
    }

  protected:
    void serialize(rapidjson::Document &destDoc) override
    {
      auto &allocator = destDoc.GetAllocator();
      destDoc.AddMember("documentId", documentId, allocator);
      destDoc.AddMember("method", rapidjson::Value(method.c_str(), allocator), allocator);

      rapidjson::Value argsArray(rapidjson::kArrayType);
      for (auto &arg : args)
        argsArray.PushBack(rapidjson::Value(arg.c_str(), allocator), allocator);
      destDoc.AddMember("args", argsArray, allocator);
    }
    void deserialize(rapidjson::Document &srcDoc) override
    {
      documentId = srcDoc["documentId"].GetUint();
      method = srcDoc["method"].GetString();
      if (srcDoc.HasMember("args") && srcDoc["args"].IsArray())
      {
        auto &argsArray = srcDoc["args"];
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
    uint32_t documentId;
    string method;
    vector<string> args;

    friend class TrEventDetailStorage;
  };

  /**
   * The class represents a response to a RPC request, there are 2 ways to create a response: using `TrRpcResponse` directly
   * or subclassing it and adding more fields.
   *
   * The first way is for simple response with only success and message fields.
   *
   * And the second way is for more complex response, to custom the response data, you need to prepare the `dataDoc` and append items
   * before sending the response.
   */
  class TrRpcResponse : public TrEventDetailObject
  {
  public:
    TrRpcResponse() = default;
    TrRpcResponse(bool success) : success(success)
    {
    }

  protected:
    void serialize(rapidjson::Document &destDoc) override
    {
      auto &allocator = destDoc.GetAllocator();
      destDoc.AddMember("success", success, allocator);
      destDoc.AddMember("message", rapidjson::Value(message.c_str(), allocator), allocator);
      if (dataDoc != nullptr)
      {
        rapidjson::Value dataValue(rapidjson::kObjectType);
        dataValue.CopyFrom(*dataDoc, allocator);
        destDoc.AddMember("data", dataValue, allocator);
      }
    }
    void deserialize(rapidjson::Document &srcDoc) override
    {
      success = srcDoc["success"].GetBool();
      message = srcDoc["message"].GetString();
      if (srcDoc.HasMember("data") && srcDoc["data"].IsObject())
      {
        dataDoc = make_unique<rapidjson::Document>();
        dataDoc->CopyFrom(srcDoc["data"], dataDoc->GetAllocator());
      }
    }

  public:
    bool success;
    string message;

  protected:
    /**
     * To custom the response data, you can initialize the `dataDoc` and append items to it.
     */
    unique_ptr<rapidjson::Document> dataDoc;

    friend class TrEventDetailStorage;
  };

  class TrDocumentRequest : public TrEventDetailObject
  {
  public:
    TrDocumentRequest() = default;
    TrDocumentRequest(TrDocumentRequestInit &init)
        : documentId(init.id),
          url(init.url),
          disableCache(init.disableCache),
          isPreview(init.isPreview),
          runScripts(init.runScripts)
    {
    }

  protected:
    void serialize(rapidjson::Document &destDoc) override
    {
      auto &allocator = destDoc.GetAllocator();
      destDoc.AddMember("documentId", documentId, allocator);
      destDoc.AddMember("url", rapidjson::Value(url.c_str(), allocator), allocator);
      destDoc.AddMember("disableCache", disableCache, allocator);
      destDoc.AddMember("isPreview", isPreview, allocator);
      destDoc.AddMember("defaultHTTPHeaders",
                        rapidjson::Value(defaultHTTPHeaders.c_str(), allocator),
                        allocator);
    }
    void deserialize(rapidjson::Document &srcDoc) override
    {
      documentId = srcDoc["documentId"].GetUint();
      url = srcDoc["url"].GetString();
      disableCache = srcDoc["disableCache"].GetBool();
      isPreview = srcDoc["isPreview"].GetBool();

      if (srcDoc.HasMember("defaultHTTPHeaders") &&
          srcDoc["defaultHTTPHeaders"].IsString())
        defaultHTTPHeaders = srcDoc["defaultHTTPHeaders"].GetString();
    }

  public:
    uint32_t documentId;
    std::string url;
    bool disableCache = false;
    bool isPreview = false;
    TrScriptRunMode runScripts = TrScriptRunMode::Dangerously;
    // The default HTTP headers to be sent with the client-side HTTP requests.
    std::string defaultHTTPHeaders;

    friend class TrEventDetailStorage;
  };

  class TrDocumentEvent : public TrEventDetailObject
  {
  public:
    TrDocumentEvent() = default;
    TrDocumentEvent(uint32_t documentId, TrDocumentEventType eventType)
        : documentId(documentId), eventType(eventType)
    {
      auto now = chrono::system_clock::now();
      timestamp = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
    }

  public:
    string toString()
    {
      return documentEventToName(eventType);
    }

  protected:
    void serialize(rapidjson::Document &destDoc) override
    {
      destDoc.AddMember("documentId", documentId, destDoc.GetAllocator());
      destDoc.AddMember("eventType", static_cast<int>(eventType), destDoc.GetAllocator());

      rapidjson::Value timestampValue;
      timestampValue.SetInt64(timestamp);
      destDoc.AddMember("timestamp", timestampValue, destDoc.GetAllocator());
    }
    void deserialize(rapidjson::Document &srcDoc) override
    {
      if (!srcDoc.HasMember("documentId") || !srcDoc["documentId"].IsUint())
        documentId = 0;
      else
        documentId = srcDoc["documentId"].GetUint();

      if (!srcDoc.HasMember("eventType") || !srcDoc["eventType"].IsInt())
        eventType = TrDocumentEventType::Unknown;
      else
        eventType = static_cast<TrDocumentEventType>(srcDoc["eventType"].GetInt());

      if (srcDoc.HasMember("timestamp") && srcDoc["timestamp"].IsInt64())
        timestamp = srcDoc["timestamp"].GetInt64();
    }

  public:
    uint32_t documentId;
    TrDocumentEventType eventType;
    long long timestamp = 0;

    friend class TrEventDetailStorage;
  };
}
