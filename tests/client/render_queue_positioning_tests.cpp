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

  SECTION("Children of positioned elements inherit positioned context")
  {
    // Simulate a positioned parent element
    RenderQueue positionedParent(100);
    positionedParent.zIndex = 0;
    positionedParent.isPositioned = true;
    
    // Child element inherits positioned context even if not directly positioned
    RenderQueue childOfPositioned(101);
    childOfPositioned.zIndex = 0;
    childOfPositioned.isPositioned = true; // Should be true due to positioned ancestor
    
    // Non-positioned element at same level
    RenderQueue nonPositioned(102);
    nonPositioned.zIndex = 0;
    nonPositioned.isPositioned = false;
    
    // Child of positioned element should render after non-positioned
    REQUIRE(nonPositioned < childOfPositioned);
    REQUIRE_FALSE(childOfPositioned < nonPositioned);
  }

  SECTION("Text content inherits positioned context from parent")
  {
    // Simulate text content of a positioned element
    RenderQueue textInPositioned(100);
    textInPositioned.zIndex = 0;
    textInPositioned.isPositioned = true; // Inherited from positioned parent
    
    // Text in non-positioned element
    RenderQueue textInNonPositioned(101);
    textInNonPositioned.zIndex = 0;
    textInNonPositioned.isPositioned = false;
    
    // Text in positioned element should render after text in non-positioned
    REQUIRE(textInNonPositioned < textInPositioned);
    REQUIRE_FALSE(textInPositioned < textInNonPositioned);
  }

  SECTION("Deeply nested children inherit positioned context")
  {
    // Grandchild of positioned element should also inherit positioned context
    RenderQueue grandchildOfPositioned(100);
    grandchildOfPositioned.zIndex = 0;
    grandchildOfPositioned.isPositioned = true; // Inherited through chain
    
    RenderQueue nonPositioned(101);
    nonPositioned.zIndex = 0;
    nonPositioned.isPositioned = false;
    
    // Grandchild should still render after non-positioned
    REQUIRE(nonPositioned < grandchildOfPositioned);
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

  SECTION("Mixed scenario: positioned parent with multiple children vs non-positioned elements")
  {
    // Simulate complex DOM structure:
    // <div style="position: relative"> <!-- positioned parent -->
    //   <p>text content</p>         <!-- child text -->
    //   <span>child element</span>   <!-- child element -->
    // </div>
    // <div>static content</div>      <!-- non-positioned sibling -->
    
    // Positioned parent
    RenderQueue positionedParent(100);
    positionedParent.zIndex = 0;
    positionedParent.isPositioned = true;
    
    // Child text inherits positioned context
    RenderQueue childText(101);
    childText.zIndex = 0;
    childText.isPositioned = true; // Inherited
    
    // Child element inherits positioned context
    RenderQueue childElement(102);
    childElement.zIndex = 0;
    childElement.isPositioned = true; // Inherited
    
    // Non-positioned sibling
    RenderQueue staticSibling(103);
    staticSibling.zIndex = 0;
    staticSibling.isPositioned = false;
    
    // All children of positioned element should render after static sibling
    REQUIRE(staticSibling < positionedParent);
    REQUIRE(staticSibling < childText);
    REQUIRE(staticSibling < childElement);
    
    // Within the positioned context, order by base
    REQUIRE(positionedParent < childText);
    REQUIRE(childText < childElement);
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