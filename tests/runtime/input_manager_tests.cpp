#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#define private public
#define protected public
#include <runtime/input-manager/input_manager.hpp>
#include <common/events_v2/native_event.hpp>

using namespace input_manager;
using namespace events_comm;

TEST_CASE("TrKeyboardEventData creation", "[InputManager]") {
  auto eventData = keyboard_events::createKeyDownEvent("a", "KeyA", false, true, false, false);
  
  REQUIRE(eventData.type == "keydown");
  REQUIRE(eventData.key == "a");
  REQUIRE(eventData.code == "KeyA");
  REQUIRE(eventData.altKey == false);
  REQUIRE(eventData.ctrlKey == true);
  REQUIRE(eventData.metaKey == false);
  REQUIRE(eventData.shiftKey == false);
  REQUIRE(eventData.repeat == false);
  REQUIRE(eventData.location == 0);
  REQUIRE(eventData.timestamp > 0);
}

TEST_CASE("TrKeyboardEventData key up creation", "[InputManager]") {
  auto eventData = keyboard_events::createKeyUpEvent("Enter", "Enter", true, false, false, false);
  
  REQUIRE(eventData.type == "keyup");
  REQUIRE(eventData.key == "Enter");
  REQUIRE(eventData.code == "Enter");
  REQUIRE(eventData.altKey == true);
  REQUIRE(eventData.ctrlKey == false);
  REQUIRE(eventData.metaKey == false);
  REQUIRE(eventData.shiftKey == false);
}

TEST_CASE("TrKeyboardEventData key press creation", "[InputManager]") {
  auto eventData = keyboard_events::createKeyPressEvent("Space", "Space", false, false, true, true);
  
  REQUIRE(eventData.type == "keypress");
  REQUIRE(eventData.key == "Space");
  REQUIRE(eventData.code == "Space");
  REQUIRE(eventData.altKey == false);
  REQUIRE(eventData.ctrlKey == false);
  REQUIRE(eventData.metaKey == true);
  REQUIRE(eventData.shiftKey == true);
}

TEST_CASE("TrInputEvent class", "[InputEvent]") {
  TrInputEvent event(1, TrInputEventType::KeyboardDown, "{\"type\":\"keydown\",\"key\":\"a\"}");
  REQUIRE(event.documentId == 1);
  REQUIRE(event.inputEventType == TrInputEventType::KeyboardDown);
  REQUIRE(event.eventData == "{\"type\":\"keydown\",\"key\":\"a\"}");
  REQUIRE(event.timestamp > 0);
}

TEST_CASE("TrInputEvent serialization", "[InputEvent]") {
  TrInputEvent event(42, TrInputEventType::KeyboardUp, "{\"key\":\"b\",\"code\":\"KeyB\"}");
  
  rapidjson::Document doc;
  doc.SetObject();
  event.serialize(doc);
  
  REQUIRE(doc.HasMember("documentId"));
  REQUIRE(doc["documentId"].GetUint() == 42);
  REQUIRE(doc.HasMember("inputEventType"));
  REQUIRE(doc["inputEventType"].GetInt() == static_cast<int>(TrInputEventType::KeyboardUp));
  REQUIRE(doc.HasMember("eventData"));
  REQUIRE(doc["eventData"].GetString() == std::string("{\"key\":\"b\",\"code\":\"KeyB\"}"));
  REQUIRE(doc.HasMember("timestamp"));
  REQUIRE(doc["timestamp"].GetInt64() > 0);
}

TEST_CASE("TrInputEvent deserialization", "[InputEvent]") {
  rapidjson::Document doc;
  doc.SetObject();
  auto& allocator = doc.GetAllocator();
  
  doc.AddMember("documentId", 123, allocator);
  doc.AddMember("inputEventType", static_cast<int>(TrInputEventType::KeyboardPress), allocator);
  doc.AddMember("eventData", rapidjson::Value("{\"test\":\"data\"}", allocator), allocator);
  doc.AddMember("timestamp", 1234567890LL, allocator);
  
  TrInputEvent event;
  event.deserialize(doc);
  
  REQUIRE(event.documentId == 123);
  REQUIRE(event.inputEventType == TrInputEventType::KeyboardPress);
  REQUIRE(event.eventData == "{\"test\":\"data\"}");
  REQUIRE(event.timestamp == 1234567890LL);
}