#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <common/events_v2/native_sender.hpp>
#include <common/events_v2/native_event.hpp>
#include <common/ipc.hpp>

using namespace events_comm;
using namespace ipc;

class MockClient : public TrOneShotClient<TrNativeEventMessage> {
public:
  MockClient() : TrOneShotClient<TrNativeEventMessage>(nullptr) {}
  bool sendRaw(void* data, size_t size) override { return true; }
};

TEST_CASE("TrNativeEventSender dispatchEvent method", "[TrNativeEventSender]") {
  MockClient client;
  TrNativeEventSender sender(&client);

  auto event = TrNativeEvent::MakeEvent<TrNativeEvent>(TrNativeEventType::RpcRequest);
  bool result = sender.dispatchEvent(event, 0);
  REQUIRE(result == true);
}
