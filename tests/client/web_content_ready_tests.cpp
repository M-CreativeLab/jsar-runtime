#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/builtin_scene/web_content.hpp>

using namespace builtin_scene;

TEST_CASE("WebContent content ready flag", "[WebContent]")
{
  SECTION("WebContent starts with content not ready")
  {
    WebContent content("test", 100.0f, 100.0f);
    REQUIRE(content.isContentReady() == false);
    REQUIRE(content.isContentDirty() == true);
    REQUIRE(content.isSurfaceDirty() == true);
  }

  SECTION("WebContent can be marked as ready")
  {
    WebContent content("test", 100.0f, 100.0f);
    REQUIRE(content.isContentReady() == false);
    
    content.setContentReady(true);
    REQUIRE(content.isContentReady() == true);
  }

  SECTION("WebContent ready state can be toggled")
  {
    WebContent content("test", 100.0f, 100.0f);
    
    content.setContentReady(true);
    REQUIRE(content.isContentReady() == true);
    
    content.setContentReady(false);
    REQUIRE(content.isContentReady() == false);
  }
}