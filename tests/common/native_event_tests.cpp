#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#define private public
#define protected public
#include <common/events_v2/native_event.hpp>

using namespace events_comm;

TEST_CASE("TrNativeEvent class", "[TrNativeEvent]") {
  TrNativeEvent event(TrNativeEventType::RpcRequest);
  REQUIRE(event.type == TrNativeEventType::RpcRequest);
  REQUIRE(event.id != 0);
}

TEST_CASE("TrRpcRequest class", "[TrRpcRequest]") {
  TrRpcRequest request(1, "method", {"arg1", "arg2"});
  REQUIRE(request.documentId == 1);
  REQUIRE(request.method == "method");
  REQUIRE(request.args.size() == 2);
  REQUIRE(request.args[0] == "arg1");
  REQUIRE(request.args[1] == "arg2");

  rapidjson::Document doc;
  doc.SetObject();
  request.serialize(doc);
  REQUIRE(doc.HasMember("documentId"));
  REQUIRE(doc["documentId"].GetUint() == 1);
  REQUIRE(doc.HasMember("method"));
  REQUIRE(doc["method"].GetString() == std::string("method"));
  REQUIRE(doc.HasMember("args"));
  REQUIRE(doc["args"].IsArray());
  REQUIRE(doc["args"].Size() == 2);
  REQUIRE(doc["args"][0].GetString() == std::string("arg1"));
  REQUIRE(doc["args"][1].GetString() == std::string("arg2"));

  TrRpcRequest deserializedRequest;
  deserializedRequest.deserialize(doc);
  REQUIRE(deserializedRequest.documentId == 1);
  REQUIRE(deserializedRequest.method == "method");
  REQUIRE(deserializedRequest.args.size() == 2);
  REQUIRE(deserializedRequest.args[0] == "arg1");
  REQUIRE(deserializedRequest.args[1] == "arg2");
}

TEST_CASE("TrRpcResponse class", "[TrRpcResponse]") {
  TrRpcResponse response(true);
  response.message = "Success";
  REQUIRE(response.success == true);
  REQUIRE(response.message == "Success");

  rapidjson::Document doc;
  doc.SetObject();
  response.serialize(doc);
  REQUIRE(doc.HasMember("success"));
  REQUIRE(doc["success"].GetBool() == true);
  REQUIRE(doc.HasMember("message"));
  REQUIRE(doc["message"].GetString() == std::string("Success"));

  TrRpcResponse deserializedResponse;
  deserializedResponse.deserialize(doc);
  REQUIRE(deserializedResponse.success == true);
  REQUIRE(deserializedResponse.message == "Success");
}

TEST_CASE("TrDocumentRequest class", "[TrDocumentRequest]") {
  TrDocumentRequestInit init;
  {
    init.id = 1;
    init.url = "url";
    init.disableCache = true;
    init.isPreview = false;
    init.runScripts = TrScriptRunMode::Dangerously;
  }
  TrDocumentRequest request(init);
  REQUIRE(request.documentId == 1);
  REQUIRE(request.url == "url");
  REQUIRE(request.disableCache == true);
  REQUIRE(request.isPreview == false);
  REQUIRE(request.runScripts == TrScriptRunMode::Dangerously);

  rapidjson::Document doc;
  doc.SetObject();
  request.serialize(doc);
  REQUIRE(doc.HasMember("documentId"));
  REQUIRE(doc["documentId"].GetUint() == 1);
  REQUIRE(doc.HasMember("url"));
  REQUIRE(doc["url"].GetString() == std::string("url"));
  REQUIRE(doc.HasMember("disableCache"));
  REQUIRE(doc["disableCache"].GetBool() == true);
  REQUIRE(doc.HasMember("isPreview"));
  REQUIRE(doc["isPreview"].GetBool() == false);

  TrDocumentRequest deserializedRequest;
  deserializedRequest.deserialize(doc);
  REQUIRE(deserializedRequest.documentId == 1);
  REQUIRE(deserializedRequest.url == "url");
  REQUIRE(deserializedRequest.disableCache == true);
  REQUIRE(deserializedRequest.isPreview == false);
  REQUIRE(deserializedRequest.runScripts == TrScriptRunMode::Dangerously);
}

TEST_CASE("TrDocumentEvent class", "[TrDocumentEvent]") {
  TrDocumentEvent event(1, TrDocumentEventType::Unknown);
  REQUIRE(event.documentId == 1);
  REQUIRE(event.eventType == TrDocumentEventType::Unknown);
  REQUIRE(event.timestamp != 0);

  rapidjson::Document doc;
  doc.SetObject();
  event.serialize(doc);
  REQUIRE(doc.HasMember("documentId"));
  REQUIRE(doc["documentId"].GetUint() == 1);
  REQUIRE(doc.HasMember("eventType"));
  REQUIRE(doc["eventType"].GetInt() == static_cast<int>(TrDocumentEventType::Unknown));
  REQUIRE(doc.HasMember("timestamp"));
  REQUIRE(doc["timestamp"].GetInt64() == event.timestamp);

  TrDocumentEvent deserializedEvent;
  deserializedEvent.deserialize(doc);
  REQUIRE(deserializedEvent.documentId == 1);
  REQUIRE(deserializedEvent.eventType == TrDocumentEventType::Unknown);
  REQUIRE(deserializedEvent.timestamp == event.timestamp);
}
