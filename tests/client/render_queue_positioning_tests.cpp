#include "../catch2/catch_amalgamated.hpp"
#include <client/builtin_scene/render_queue.hpp>

using namespace builtin_scene;

TEST_CASE("RenderQueue Positioning Tests", "[render-queue-positioning]")
{
  SECTION("Non-positioned elements render before positioned when zIndex=0")
  {
    // Create two elements with zIndex=0, one positioned and one not
    RenderQueue nonPositioned(100);
    nonPositioned.zIndex = 0;
    nonPositioned.isPositioned = false;
    
    RenderQueue positioned(101);
    positioned.zIndex = 0;
    positioned.isPositioned = true;
    
    // Non-positioned should render before positioned (be "less than")
    REQUIRE(nonPositioned < positioned);
    REQUIRE_FALSE(positioned < nonPositioned);
  }

  SECTION("Positioned elements with higher zIndex still render after non-positioned with lower zIndex")
  {
    RenderQueue nonPositioned(100);
    nonPositioned.zIndex = 1;
    nonPositioned.isPositioned = false;
    
    RenderQueue positioned(101);
    positioned.zIndex = 2;
    positioned.isPositioned = true;
    
    // zIndex takes precedence over positioning
    REQUIRE(nonPositioned < positioned);
  }

  SECTION("Elements with same zIndex and same positioning state use base for ordering")
  {
    RenderQueue element1(100);
    element1.zIndex = 0;
    element1.isPositioned = true;
    
    RenderQueue element2(101);
    element2.zIndex = 0;
    element2.isPositioned = true;
    
    // Base number determines order when zIndex and positioning are equal
    REQUIRE(element1 < element2);
  }

  SECTION("translateZ takes precedence over all other factors")
  {
    RenderQueue frontElement(200);
    frontElement.zIndex = 0;
    frontElement.isPositioned = false;
    frontElement.translateZ = 10.0f;
    
    RenderQueue backElement(100);
    backElement.zIndex = 999;
    backElement.isPositioned = true;
    backElement.translateZ = 0.0f;
    
    // translateZ takes precedence
    REQUIRE(backElement < frontElement);
  }

  SECTION("Equality comparison includes isPositioned")
  {
    RenderQueue queue1(100);
    queue1.zIndex = 0;
    queue1.translateZ = 5.0f;
    queue1.isPositioned = true;
    
    RenderQueue queue2(100);
    queue2.zIndex = 0;
    queue2.translateZ = 5.0f;
    queue2.isPositioned = true;
    
    RenderQueue queue3(100);
    queue3.zIndex = 0;
    queue3.translateZ = 5.0f;
    queue3.isPositioned = false;
    
    REQUIRE(queue1 == queue2);
    REQUIRE_FALSE(queue1 == queue3);
    REQUIRE(queue1 != queue3);
  }

  SECTION("Non-zIndex-0 elements are not affected by positioning")
  {
    RenderQueue nonPositioned(100);
    nonPositioned.zIndex = 1;
    nonPositioned.isPositioned = false;
    
    RenderQueue positioned(101);
    positioned.zIndex = 1;
    positioned.isPositioned = true;
    
    // When zIndex is not 0, positioning should not affect order
    // They should be equal except for base number
    REQUIRE(nonPositioned < positioned); // Only base differs
  }

  SECTION("Output operator includes isPositioned")
  {
    RenderQueue queue(42);
    queue.zIndex = 1;
    queue.translateZ = 2.5f;
    queue.isPositioned = true;
    
    std::ostringstream output;
    output << queue;
    
    std::string result = output.str();
    REQUIRE(result.find("base=42") != std::string::npos);
    REQUIRE(result.find("zIndex=1") != std::string::npos);
    REQUIRE(result.find("translateZ=2.5") != std::string::npos);
    REQUIRE(result.find("isPositioned=true") != std::string::npos);
  }
}