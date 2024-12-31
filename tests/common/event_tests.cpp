#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <common/events_v2/event.hpp>

using namespace events_comm;

enum class TestEventType
{
  EventType1,
  EventType2
};

class TestEventDetail : public TrEventDetailObject
{
public:
  int value;

  void serialize(rapidjson::Document &destDoc) override
  {
    destDoc.AddMember("value", value, destDoc.GetAllocator());
  }

  void deserialize(rapidjson::Document &srcDoc) override
  {
    value = srcDoc["value"].GetInt();
  }
};

class TestEvent : public TrEvent<TestEventType>
{
public:
  TestEvent(TestEventType type) : TrEvent(type) {}
};

TEST_CASE("TrEvent MakeEvent method", "[TrEvent]") {
  TestEventDetail detail;
  detail.value = 42;

  auto event = TrEvent<TestEventType>::MakeEvent<TestEvent>(TestEventType::EventType1, &detail);
  REQUIRE(event->type == TestEventType::EventType1);
  REQUIRE(event->detail<TestEventDetail>().value == 42);
}

TEST_CASE("TrEvent MakeEventWithString method", "[TrEvent]") {
  const char *detailJson = "{\"value\": 42}";

  auto event = TrEvent<TestEventType>::MakeEventWithString<TestEvent>(TestEventType::EventType1, detailJson);
  REQUIRE(event->type == TestEventType::EventType1);
  REQUIRE(event->detail<TestEventDetail>().value == 42);
}

TEST_CASE("TrEvent getDetailJson method", "[TrEvent]") {
  TestEventDetail detail;
  detail.value = 42;

  auto event = TrEvent<TestEventType>::MakeEvent<TestEvent>(TestEventType::EventType1, &detail);
  REQUIRE(event->getDetailJson() == "{\"value\":42}");
}
