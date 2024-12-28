#define CATCH_CONFIG_MAIN
#include "catch2/catch_amalgamated.hpp"
#include "common/viewport.hpp"

TEST_CASE("TrViewport isEqual method", "[TrViewport]") {
  TrViewport viewport(800, 600, 100, 100);
  REQUIRE(viewport.isEqual(800, 600, 100, 100));
  REQUIRE_FALSE(viewport.isEqual(1024, 768, 100, 100));
}

TEST_CASE("TrViewport set method", "[TrViewport]") {
  TrViewport viewport;
  viewport.set(800, 600, 100, 100);
  REQUIRE(viewport.isEqual(800, 600, 100, 100));
}

TEST_CASE("TrViewport toString method", "[TrViewport]") {
  TrViewport viewport(800, 600, 100, 100);
  REQUIRE(viewport.toString() == "Viewport(800, 600, 100,100)");
}

TEST_CASE("TrViewport << operator", "[TrViewport]") {
  TrViewport viewport(800, 600, 100, 100);
  std::ostringstream oss;
  oss << viewport;
  REQUIRE(oss.str() == "Viewport(800, 600, 100,100)");
}
