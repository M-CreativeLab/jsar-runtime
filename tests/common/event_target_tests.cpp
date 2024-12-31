#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <common/events_v2/event_target.hpp>

using namespace events_comm;

enum class TestEventType
{
  EventType1,
  EventType2
};

class TestEvent : public TrEvent<TestEventType>
{
public:
  TestEvent(TestEventType type) : TrEvent(type) {}
};

TEST_CASE("TrEventTarget dispatchEvent method", "[TrEventTarget]") {
  TrEventTarget<TestEventType, TestEvent> target;
  bool callbackInvoked = false;

  auto callback = [&](TestEventType type, std::shared_ptr<TestEvent> event) {
    callbackInvoked = true;
    REQUIRE(type == TestEventType::EventType1);
  };

  target.addEventListener(TestEventType::EventType1, callback);
  target.dispatchEvent(TestEventType::EventType1);

  REQUIRE(callbackInvoked);
}

TEST_CASE("TrEventTarget addEventListener and removeEventListener methods", "[TrEventTarget]") {
  TrEventTarget<TestEventType, TestEvent> target;
  bool callbackInvoked = false;

  auto callback = [&](TestEventType type, std::shared_ptr<TestEvent> event) {
    callbackInvoked = true;
  };

  auto listener = target.addEventListener(TestEventType::EventType1, callback);
  target.dispatchEvent(TestEventType::EventType1);
  REQUIRE(callbackInvoked);

  callbackInvoked = false;
  target.removeEventListener(TestEventType::EventType1, listener);
  target.dispatchEvent(TestEventType::EventType1);
  REQUIRE_FALSE(callbackInvoked);
}

TEST_CASE("TrEventTarget resetGlobalEventListener method", "[TrEventTarget]") {
  TrEventTarget<TestEventType, TestEvent> target;
  bool callbackInvoked = false;

  auto callback = [&](TestEventType type, std::shared_ptr<TestEvent> event) {
    callbackInvoked = true;
  };

  target.resetGlobalEventListener(callback);
  target.dispatchEvent(TestEventType::EventType1);
  REQUIRE(callbackInvoked);

  callbackInvoked = false;
  target.resetGlobalEventListener();
  target.dispatchEvent(TestEventType::EventType1);
  REQUIRE_FALSE(callbackInvoked);
}
