#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <common/events_v2/native_receiver.hpp>
#include <common/events_v2/native_message.hpp>
#include <common/ipc.hpp>

using namespace events_comm;
using namespace ipc;

class MockClient : public TrOneShotClient<TrNativeEventMessage> {
public:
  MockClient() : TrOneShotClient<TrNativeEventMessage>(nullptr) {}
  bool sendRaw(void* data, size_t size) override { return true; }
  bool recvRaw(void* data, size_t size, int timeout) override { return true; }
};

TEST_CASE("TrNativeEventReceiver recvEvent method", "[TrNativeEventReceiver]") {
  MockClient client;
  TrNativeEventReceiver receiver(&client);

  TrNativeEventMessage* message = receiver.recvEvent(1000);
  REQUIRE(message != nullptr);
  delete message;
}

TEST_CASE("TrNativeEventReceiver recvEventOn method", "[TrNativeEventReceiver]") {
  MockClient client;
  TrNativeEventReceiver receiver(&client);

  TrNativeEventMessage message;
  bool result = receiver.recvEventOn(message, 1000);
  REQUIRE(result == true);
}
