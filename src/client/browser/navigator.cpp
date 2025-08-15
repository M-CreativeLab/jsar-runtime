#include "./navigator.hpp"
#include <common/events_v2/native_event.hpp>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace browser
{
  using namespace events_comm;
  using namespace std;

  Navigator::Navigator(TrClientContextPerProcess *clientContext)
      : dom::DOMEventTarget()
      , client_context_(clientContext)
      , next_request_id_(1)
      , cookie_enabled_(true)
      , on_line_(true)
      , hardware_concurrency_(1)
      , native_event_listener_([this](TrNativeEventType eventType, shared_ptr<TrNativeEvent> event)
                               { this->onNativeEvent(eventType, event); })
  {
    // Initialize navigator properties
    user_agent_ = "Mozilla/5.0 (JSAR Runtime) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36";
    platform_ = "JSAR";
    app_name_ = "JSAR Runtime";
    app_version_ = "1.0.0";
    language_ = "en-US";

    // Register as event listener for native events
    if (client_context_ && client_context_->constellation && client_context_->constellation->nativeEventTarget)
    {
      client_context_->constellation->nativeEventTarget->addEventListener(native_event_listener_);
    }
  }

  Navigator::~Navigator()
  {
    // Unregister event listener
    if (client_context_ && client_context_->constellation && client_context_->constellation->nativeEventTarget)
    {
      client_context_->constellation->nativeEventTarget->removeEventListener(native_event_listener_);
    }
  }

  uint32_t Navigator::makeRpcCall(const string &method,
                                  const vector<string> &args,
                                  function<void(bool, const string &)> callback)
  {
    if (!client_context_)
    {
      if (callback)
      {
        callback(false, "Client context not available");
      }
      return 0;
    }

    uint32_t requestId = next_request_id_++;

    // Store the callback if provided
    if (callback)
    {
      rpc_callbacks_[requestId] = callback;
    }

    // Create and send RPC request
    TrRpcRequest req(client_context_->id, method, args);
    auto event = TrNativeEvent::MakeEvent(TrNativeEventType::RpcRequest, &req);

    if (client_context_->sendEvent(event))
    {
      return requestId;
    }
    else
    {
      // Remove callback on failure
      if (callback)
      {
        rpc_callbacks_.erase(requestId);
        callback(false, "Failed to send RPC request");
      }
      return 0;
    }
  }

  void Navigator::handleDocumentRequest(const string &url, uint32_t documentId, bool disableCache)
  {
    if (!client_context_)
      return;

    // Create document request
    rapidjson::Document docReq;
    docReq.SetObject();
    auto &allocator = docReq.GetAllocator();

    docReq.AddMember("url", rapidjson::Value(url.c_str(), allocator), allocator);
    docReq.AddMember("documentId", documentId, allocator);
    docReq.AddMember("disableCache", disableCache, allocator);

    // Convert to JSON string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    docReq.Accept(writer);

    // Create document request event
    auto requestEvent = TrNativeEvent::MakeEventWithString(TrNativeEventType::DocumentRequest, buffer.GetString());
    client_context_->sendEvent(requestEvent);

    // Dispatch document request event locally
    auto documentRequestEvent = make_shared<dom::Event>("documentrequest");
    dispatchEvent(documentRequestEvent);
  }

  void Navigator::onNativeEvent(TrNativeEventType eventType, shared_ptr<TrNativeEvent> event)
  {
    if (!event)
      return;

    switch (eventType)
    {
    case TrNativeEventType::RpcResponse:
      handleRpcResponse(event->id, event->detailJson());
      break;
    case TrNativeEventType::DocumentEvent:
      handleDocumentEvent(event->detailJson());
      break;
    case TrNativeEventType::DocumentRequest:
      // Handle incoming document requests if needed
      break;
    default:
      // Ignore other event types
      break;
    }
  }

  void Navigator::handleRpcResponse(uint32_t requestId, const string &response)
  {
    auto it = rpc_callbacks_.find(requestId);
    if (it != rpc_callbacks_.end())
    {
      auto callback = it->second;
      rpc_callbacks_.erase(it);

      // Parse the response to determine success
      rapidjson::Document doc;
      bool success = false;
      string message = response;

      if (!doc.Parse(response.c_str()).HasParseError())
      {
        if (doc.IsObject() && doc.HasMember("success"))
        {
          success = doc["success"].GetBool();
          if (doc.HasMember("message"))
          {
            message = doc["message"].GetString();
          }
        }
      }

      callback(success, message);
    }
  }

  void Navigator::handleDocumentEvent(const string &eventData)
  {
    // Parse document event data
    rapidjson::Document doc;
    if (doc.Parse(eventData.c_str()).HasParseError())
    {
      return;
    }

    if (!doc.IsObject())
      return;

    // Create and dispatch document event
    auto documentEvent = make_shared<dom::Event>("documentevent");
    dispatchEvent(documentEvent);
  }
} // namespace browser