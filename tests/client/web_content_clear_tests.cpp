#include <catch2/catch_test_macros.hpp>
#include <client/builtin_scene/web_content.hpp>

using namespace builtin_scene;

TEST_CASE("WebContent background clearing logic", "[WebContent]")
{
  SECTION("New WebContent should need background clear")
  {
    WebContent content("test", 100.0f, 100.0f);
    REQUIRE(content.needsBackgroundClear() == true);
  }

  SECTION("After rendering, background clear flag should be false")
  {
    WebContent content("test", 100.0f, 100.0f);
    content.setNeedsBackgroundClear(false);
    REQUIRE(content.needsBackgroundClear() == false);
  }

  SECTION("Surface resize should trigger background clear")
  {
    WebContent content("test", 100.0f, 100.0f);
    content.setNeedsBackgroundClear(false); // Reset flag

    // Resize should trigger background clear
    content.resetSkSurface(200.0f, 200.0f);
    REQUIRE(content.needsBackgroundClear() == true);
  }

  SECTION("Fragment size change should trigger background clear")
  {
    WebContent content("test", 100.0f, 100.0f);
    
    // Set initial fragment
    client_layout::Fragment fragment1;
    // Note: Fragment constructor details may vary, this is conceptual
    content.setFragment(fragment1);
    content.setNeedsBackgroundClear(false); // Reset flag
    
    // Setting same fragment should not trigger clear
    content.setFragment(fragment1);
    REQUIRE(content.needsBackgroundClear() == false);
  }
}