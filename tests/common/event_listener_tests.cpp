#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <common/events_v2/event_listener.hpp>

using namespace events_comm;

TEST_CASE("TrEventListener constructor and operator()", "[TrEventListener]") {
  bool callbackInvoked = false;
  auto callback = [&](int type, std::shared_ptr<int> event) {
    callbackInvoked = true;
    REQUIRE(type == 1);
    REQUIRE(*event == 42);
  };

  TrEventListener<int, int> listener(callback);
  listener(1, std::make_shared<int>(42));

  REQUIRE(callbackInvoked);
}

TEST_CASE("TrEventListener equals method", "[TrEventListener]") {
  auto callback = [](int, std::shared_ptr<int>) {};

  TrEventListener<int, int> listener1(callback);
  TrEventListener<int, int> listener2(callback);

  REQUIRE_FALSE(listener1.equals(listener2));

  TrEventListener<int, int> listener3 = listener1;
  REQUIRE(listener1.equals(listener3));
}
