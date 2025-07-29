#include <catch2/catch_amalgamated.hpp>
#include "../src/runtime/network_monitor.hpp"
#include "../src/runtime/network_service.hpp"

using namespace runtime;

TEST_CASE("NetworkMonitor can be created", "[network]") 
{
  auto monitor = NetworkMonitor::create();
  REQUIRE(monitor != nullptr);
}

TEST_CASE("NetworkService singleton works", "[network]") 
{
  auto& service = NetworkService::getInstance();
  REQUIRE(!service.isRunning());
  
  // Test getting current status (should not crash)
  auto status = service.getCurrentStatus();
  REQUIRE((status == NetworkStatus::Online || status == NetworkStatus::Offline));
}

TEST_CASE("NetworkService can start and stop", "[network]") 
{
  auto& service = NetworkService::getInstance();
  
  // Start the service
  bool started = service.start();
  REQUIRE(started);
  REQUIRE(service.isRunning());
  
  // Stop the service
  service.stop();
  REQUIRE(!service.isRunning());
}