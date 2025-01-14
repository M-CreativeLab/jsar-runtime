#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#define private public
#define protected public
#include <common/events_v2/native_message.hpp>

using namespace events_comm;

TEST_CASE("TrNativeEventMessage class", "[TrNativeEventMessage]") {
  TrNativeEventMessage message;
  REQUIRE(message.getType() == TrNativeEventType::Unset);

  TrNativeEventMessage messageWithType(TrNativeEventType::RpcRequest, 0, nullptr);
  REQUIRE(messageWithType.getType() == TrNativeEventType::RpcRequest);
}

TEST_CASE("TrNativeEventMessage serialize and deserialize methods", "[TrNativeEventMessage]") {
  TrNativeEventMessage message(TrNativeEventType::RpcRequest, 0, nullptr);
  void* data = nullptr;
  size_t size = 0;
  REQUIRE(message.serialize(&data, &size) == true);
  REQUIRE(data != nullptr);
  REQUIRE(size > 0);
  free(data);
}
