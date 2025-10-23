#include "../catch2/catch_amalgamated.hpp"
#include <client/builtin_scene/render_queue.hpp>

using namespace builtin_scene;

TEST_CASE("RenderQueue Tests", "[render-queue]")
{
  SECTION("Basic comparison with only base values")
  {
    RenderQueue queue1(100);
    RenderQueue queue2(200);
    
    // Lower base value should render first
    REQUIRE(queue1 < queue2);
    REQUIRE_FALSE(queue2 < queue1);
  }

  SECTION("zIndex takes precedence over base")
  {
    RenderQueue lowBase(100);
    lowBase.zIndex = 2.0f;
    
    RenderQueue highBase(200);
    highBase.zIndex = 1.0f;
    
    // Lower zIndex renders first, regardless of base
    REQUIRE(highBase < lowBase);
    REQUIRE_FALSE(lowBase < highBase);
  }

  SECTION("translateZ takes precedence over zIndex and base")
  {
    RenderQueue backElement(100);
    backElement.zIndex = 999.0f;
    backElement.translateZ = 0.0f;
    
    RenderQueue frontElement(200);
    frontElement.zIndex = 0.0f;
    frontElement.translateZ = 10.0f;
    
    // Lower translateZ renders first, regardless of zIndex or base
    REQUIRE(backElement < frontElement);
    REQUIRE_FALSE(frontElement < backElement);
  }

  SECTION("Elements with same values are equal")
  {
    RenderQueue queue1(100);
    queue1.zIndex = 5.0f;
    queue1.translateZ = 2.5f;
    
    RenderQueue queue2(100);
    queue2.zIndex = 5.0f;
    queue2.translateZ = 2.5f;
    
    REQUIRE(queue1 == queue2);
    REQUIRE_FALSE(queue1 != queue2);
    REQUIRE_FALSE(queue1 < queue2);
    REQUIRE_FALSE(queue2 < queue1);
  }

  SECTION("Elements with different values are not equal")
  {
    RenderQueue queue1(100);
    queue1.zIndex = 5.0f;
    queue1.translateZ = 2.5f;
    
    RenderQueue queue2(100);
    queue2.zIndex = 5.0f;
    queue2.translateZ = 3.0f; // Different translateZ
    
    REQUIRE_FALSE(queue1 == queue2);
    REQUIRE(queue1 != queue2);
  }

  SECTION("Priority order: translateZ > zIndex > base")
  {
    // Test that the priority order is respected
    RenderQueue element1(300); // Highest base
    element1.zIndex = 0.0f;
    element1.translateZ = 1.0f; // Lowest translateZ
    
    RenderQueue element2(200); // Middle base
    element2.zIndex = 999.0f; // Highest zIndex
    element2.translateZ = 2.0f; // Middle translateZ
    
    RenderQueue element3(100); // Lowest base
    element3.zIndex = 1.0f; // Middle zIndex
    element3.translateZ = 3.0f; // Highest translateZ
    
    // Order should be: element1 < element2 < element3 (by translateZ)
    REQUIRE(element1 < element2);
    REQUIRE(element2 < element3);
    REQUIRE(element1 < element3);
  }

  SECTION("zIndex with decimal values")
  {
    RenderQueue queue1(100);
    queue1.zIndex = 1.5f;
    
    RenderQueue queue2(100);
    queue2.zIndex = 1.6f;
    
    REQUIRE(queue1 < queue2);
  }

  SECTION("Negative zIndex values")
  {
    RenderQueue negativeZ(100);
    negativeZ.zIndex = -1.0f;
    
    RenderQueue positiveZ(100);
    positiveZ.zIndex = 1.0f;
    
    REQUIRE(negativeZ < positiveZ);
  }

  SECTION("Negative translateZ values")
  {
    RenderQueue negativeTranslateZ(100);
    negativeTranslateZ.translateZ = -5.0f;
    
    RenderQueue positiveTranslateZ(100);
    positiveTranslateZ.translateZ = 5.0f;
    
    REQUIRE(negativeTranslateZ < positiveTranslateZ);
  }

  SECTION("Greater than operator")
  {
    RenderQueue smaller(100);
    RenderQueue larger(200);
    
    REQUIRE(larger > smaller);
    REQUIRE_FALSE(smaller > larger);
    REQUIRE_FALSE(smaller > smaller); // Same element is not greater than itself
  }

  SECTION("Output operator includes all fields")
  {
    RenderQueue queue(42);
    queue.zIndex = 1.5f;
    queue.translateZ = 2.5f;
    
    std::ostringstream output;
    output << queue;
    
    std::string result = output.str();
    REQUIRE(result.find("base=42") != std::string::npos);
    REQUIRE(result.find("zIndex=1.5") != std::string::npos);
    REQUIRE(result.find("translateZ=2.5") != std::string::npos);
  }

  SECTION("Complex scenario with multiple elements")
  {
    // Create a realistic scenario with multiple elements
    RenderQueue background(100);
    background.zIndex = -1.0f;
    background.translateZ = 0.0f;
    
    RenderQueue content1(200);
    content1.zIndex = 0.0f;
    content1.translateZ = 0.0f;
    
    RenderQueue content2(300);
    content2.zIndex = 0.0f;
    content2.translateZ = 0.0f;
    
    RenderQueue overlay(400);
    overlay.zIndex = 10.0f;
    overlay.translateZ = 0.0f;
    
    RenderQueue popup(500);
    popup.zIndex = 10.0f;
    popup.translateZ = 5.0f;
    
    // Expected order: background < content1 < content2 < overlay < popup
    REQUIRE(background < content1);
    REQUIRE(content1 < content2); // Same zIndex, order by base
    REQUIRE(content2 < overlay);
    REQUIRE(overlay < popup); // Same zIndex, order by translateZ
  }

  SECTION("Zero values are handled correctly")
  {
    RenderQueue allZeros(0);
    allZeros.zIndex = 0.0f;
    allZeros.translateZ = 0.0f;
    
    RenderQueue nonZeroBase(1);
    nonZeroBase.zIndex = 0.0f;
    nonZeroBase.translateZ = 0.0f;
    
    REQUIRE(allZeros < nonZeroBase);
  }
}