#include "../catch2/catch_amalgamated.hpp"
#include <client/builtin_scene/instanced_mesh.hpp>

using namespace builtin_scene;

TEST_CASE("Instance scroll shadow properties", "[scroll_shadow]")
{
  SECTION("Default values")
  {
    Instance instance;

    // Default scroll shadow properties should be initialized properly
    REQUIRE(instance.getScrollShadowColor() == glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
    REQUIRE(instance.getScrollShadowMaxHeight() == 0.2f);
    REQUIRE(instance.getScrollOffset() == glm::vec2(0.0f, 0.0f));
    REQUIRE(instance.getContentSize() == glm::vec2(0.0f, 0.0f));
  }

  SECTION("Scroll shadow color setter")
  {
    Instance instance;

    glm::vec4 testColor(1.0f, 0.5f, 0.2f, 0.8f);
    instance.setScrollShadowColor(testColor);

    REQUIRE(instance.getScrollShadowColor() == testColor);

    // Test convenience overload
    instance.setScrollShadowColor(0.0f, 1.0f, 0.0f, 0.5f);
    REQUIRE(instance.getScrollShadowColor() == glm::vec4(0.0f, 1.0f, 0.0f, 0.5f));
  }

  SECTION("Scroll shadow max height setter")
  {
    Instance instance;

    instance.setScrollShadowMaxHeight(0.3f);
    REQUIRE(instance.getScrollShadowMaxHeight() == 0.3f);
  }

  SECTION("Scroll offset setter")
  {
    Instance instance;

    glm::vec2 offset(10.0f, 20.0f);
    instance.setScrollOffset(offset);
    REQUIRE(instance.getScrollOffset() == offset);

    // Test convenience overload
    instance.setScrollOffset(5.0f, 15.0f);
    REQUIRE(instance.getScrollOffset() == glm::vec2(5.0f, 15.0f));
  }

  SECTION("Content size setter")
  {
    Instance instance;

    glm::vec2 size(200.0f, 300.0f);
    instance.setContentSize(size);
    REQUIRE(instance.getContentSize() == size);

    // Test convenience overload
    instance.setContentSize(100.0f, 150.0f);
    REQUIRE(instance.getContentSize() == glm::vec2(100.0f, 150.0f));
  }

  SECTION("No-op when setting same values")
  {
    Instance instance;

    // Setting same values should return early (this tests the optimization)
    instance.setScrollShadowColor(0.0f, 0.0f, 0.0f, 0.0f);
    instance.setScrollShadowMaxHeight(0.2f);
    instance.setScrollOffset(0.0f, 0.0f);
    instance.setContentSize(0.0f, 0.0f);

    // All should still equal default values
    REQUIRE(instance.getScrollShadowColor() == glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
    REQUIRE(instance.getScrollShadowMaxHeight() == 0.2f);
    REQUIRE(instance.getScrollOffset() == glm::vec2(0.0f, 0.0f));
    REQUIRE(instance.getContentSize() == glm::vec2(0.0f, 0.0f));
  }
}

TEST_CASE("InstanceData attributes after scroll shadow refactoring", "[scroll_shadow]")
{
  SECTION("Instance attributes no longer include scroll shadow fields")
  {
    // Test that scroll shadow attributes are properly removed from vertex attributes
    // since they are now managed via texture-based storage
    const auto &attributes = InstancedMeshBase::INSTANCE_ATTRIBUTES;
    auto hasAttribute = [&](const std::string &name)
    {
      return std::find(attributes.begin(), attributes.end(), name) != attributes.end();
    };

    // Scroll shadow attributes should no longer be in the vertex attributes
    REQUIRE_FALSE(hasAttribute("instanceScrollShadowColor"));
    REQUIRE_FALSE(hasAttribute("instanceScrollShadowMaxHeight"));
    REQUIRE_FALSE(hasAttribute("instanceScrollOffset"));
    REQUIRE_FALSE(hasAttribute("instanceContentSize"));
    
    // But other attributes should still be present
    REQUIRE(hasAttribute("instanceTransform"));
    REQUIRE(hasAttribute("instanceColor"));
    REQUIRE(hasAttribute("instanceBorderRadius"));
  }

  SECTION("InstanceData output stream no longer includes scroll shadow fields")
  {
    InstanceData data;
    data.color = glm::vec4(1.0f, 0.5f, 0.2f, 0.8f);
    data.dimensions = glm::vec2(200.0f, 300.0f);

    std::ostringstream output;
    output << data;
    std::string outputStr = output.str();

    // Check that scroll shadow fields are no longer included in InstanceData output
    REQUIRE(outputStr.find("scrollShadowColor") == std::string::npos);
    REQUIRE(outputStr.find("scrollShadowMaxHeight") == std::string::npos);
    REQUIRE(outputStr.find("scrollOffset") == std::string::npos);
    REQUIRE(outputStr.find("contentSize") == std::string::npos);
    
    // But other fields should still be present
    REQUIRE(outputStr.find("color") != std::string::npos);
    REQUIRE(outputStr.find("dimensions") != std::string::npos);
  }
}