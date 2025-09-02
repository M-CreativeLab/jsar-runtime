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

  SECTION("Content ready state is independent of dirty state")
  {
    WebContent content("test", 100.0f, 100.0f);
    
    // Initially not ready but dirty
    REQUIRE(content.isContentReady() == false);
    REQUIRE(content.isContentDirty() == true);
    
    // Mark as ready
    content.setContentReady(true);
    REQUIRE(content.isContentReady() == true);
    REQUIRE(content.isContentDirty() == true); // Should still be dirty
    
    // Clear dirty flag
    content.setContentDirty(false);
    REQUIRE(content.isContentReady() == true); // Should still be ready
    REQUIRE(content.isContentDirty() == false);
    
    // Make dirty again
    content.setContentDirty(true);
    REQUIRE(content.isContentReady() == true); // Should still be ready
    REQUIRE(content.isContentDirty() == true);
  }
}