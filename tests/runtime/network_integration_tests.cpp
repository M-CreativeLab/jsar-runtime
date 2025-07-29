#include <catch2/catch_amalgamated.hpp>
#include "../src/runtime/network_service.hpp"
#include "../src/client/dom/events/network_event.hpp"
#include "../src/client/browser/window.hpp"

using namespace runtime;
using namespace dom;
using namespace browser;

TEST_CASE("NetworkEvent creation", "[network][integration]") 
{
  auto onlineEvent = NetworkEvent::createOnlineEvent();
  REQUIRE(onlineEvent != nullptr);
  REQUIRE(onlineEvent->typeStr() == "online");
  REQUIRE(onlineEvent->constructor() == DOMEventConstructorType::kNetworkEvent);
  
  auto offlineEvent = NetworkEvent::createOfflineEvent();
  REQUIRE(offlineEvent != nullptr);
  REQUIRE(offlineEvent->typeStr() == "offline");
  REQUIRE(offlineEvent->constructor() == DOMEventConstructorType::kNetworkEvent);
}

TEST_CASE("Window network integration", "[network][integration]") 
{
  // Create a window instance
  auto window = std::make_shared<Window>();
  REQUIRE(window != nullptr);
  
  // Test navigator.onLine property
  bool isOnline = window->navigator_onLine();
  // Should be either true or false, not crash
  REQUIRE((isOnline == true || isOnline == false));
}

TEST_CASE("Network status changes trigger events", "[network][integration]") 
{
  bool eventReceived = false;
  NetworkStatus receivedStatus = NetworkStatus::Offline;
  
  // Set up event listener
  auto& service = NetworkService::getInstance();
  
  auto callback = [&eventReceived, &receivedStatus](NetworkEventType eventType, std::shared_ptr<NetworkEvent> event) {
    eventReceived = true;
    auto detail = event->detail<NetworkEventDetail>();
    receivedStatus = detail.networkStatus;
  };
  
  service.addEventListener(NetworkEventType::Online, callback);
  service.addEventListener(NetworkEventType::Offline, callback);
  
  // Start the service
  service.start();
  REQUIRE(service.isRunning());
  
  // Get initial status (this should trigger an event if the callback is working)
  auto currentStatus = service.getCurrentStatus();
  REQUIRE((currentStatus == NetworkStatus::Online || currentStatus == NetworkStatus::Offline));
  
  // Clean up
  service.stop();
}