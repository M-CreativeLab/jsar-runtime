#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/builtin_scene/web_content.hpp>
#include <client/cssom/units.hpp>

using namespace builtin_scene;

TEST_CASE("WebContent device pixel ratio", "[web_content]")
{
  SECTION("Default device pixel ratio should be 1.5")
  {
    WebContent content("test", 100.0f, 100.0f);
    REQUIRE(content.devicePixelRatio() == client_cssom::DevicePixelRatio);
    REQUIRE(content.devicePixelRatio() == 1.5f);
  }

  SECTION("Device pixel ratio affects logical dimensions")
  {
    WebContent content("test", 300.0f, 200.0f);
    
    // With device pixel ratio of 1.5, logical dimensions should be physical/ratio
    float expectedLogicalWidth = content.physicalWidth() / 1.5f;
    float expectedLogicalHeight = content.physicalHeight() / 1.5f;
    
    REQUIRE(content.logicalWidth() == Catch::Approx(expectedLogicalWidth));
    REQUIRE(content.logicalHeight() == Catch::Approx(expectedLogicalHeight));
  }

  SECTION("Setting device pixel ratio")
  {
    WebContent content("test", 100.0f, 100.0f);
    
    // Test setting a different ratio
    content.setDevicePixelRatio(2.0f);
    REQUIRE(content.devicePixelRatio() == 2.0f);
    
    // Test that setting ratio marks content as dirty
    content.setDirty(false);
    content.setDevicePixelRatio(3.0f);
    REQUIRE(content.isDirty() == true);
  }

  SECTION("Invalid device pixel ratio should not be set")
  {
    WebContent content("test", 100.0f, 100.0f);
    float originalRatio = content.devicePixelRatio();
    
    // Test invalid values
    content.setDevicePixelRatio(0.0f);
    REQUIRE(content.devicePixelRatio() == originalRatio);
    
    content.setDevicePixelRatio(-1.0f);
    REQUIRE(content.devicePixelRatio() == originalRatio);
  }

  SECTION("Setting same device pixel ratio should not mark as dirty")
  {
    WebContent content("test", 100.0f, 100.0f);
    content.setDirty(false);
    
    // Setting the same ratio should not mark as dirty
    content.setDevicePixelRatio(content.devicePixelRatio());
    REQUIRE(content.isDirty() == false);
  }
}