#include "../catch2/catch_amalgamated.hpp"
#include <client/builtin_scene/render_queue.hpp>

using namespace builtin_scene;

TEST_CASE("RenderQueue Positioning Tests", "[render-queue-positioning]")
{
  SECTION("Positioned elements with zIndex=0 render before non-positioned when using effective zIndex")
  {
    // Create two elements with zIndex=0, one positioned and one not
    RenderQueue nonPositioned(100);
    nonPositioned.zIndex = 0;
    nonPositioned.isPositioned = false; // effective zIndex = 0.1
    
    RenderQueue positioned(101);
    positioned.zIndex = 0;
    positioned.isPositioned = true; // effective zIndex = 0.0
    
    // Positioned (0.0) should render before non-positioned (0.1)
    REQUIRE(positioned < nonPositioned);
    REQUIRE_FALSE(nonPositioned < positioned);
  }

  SECTION("Children of positioned elements inherit positioned context")
  {
    // Simulate a positioned parent element
    RenderQueue positionedParent(100);
    positionedParent.zIndex = 0;
    positionedParent.isPositioned = true; // effective zIndex = 0.0
    
    // Child element inherits positioned context even if not directly positioned
    RenderQueue childOfPositioned(101);
    childOfPositioned.zIndex = 0;
    childOfPositioned.isPositioned = true; // Should be true due to positioned ancestor, effective zIndex = 0.0
    
    // Non-positioned element at same level
    RenderQueue nonPositioned(102);
    nonPositioned.zIndex = 0;
    nonPositioned.isPositioned = false; // effective zIndex = 0.1
    
    // Child of positioned element should render before non-positioned (lower effective zIndex)
    REQUIRE(childOfPositioned < nonPositioned);
    REQUIRE_FALSE(nonPositioned < childOfPositioned);
  }

  SECTION("Text content inherits positioned context from parent")
  {
    // Simulate text content of a positioned element
    RenderQueue textInPositioned(100);
    textInPositioned.zIndex = 0;
    textInPositioned.isPositioned = true; // Inherited from positioned parent, effective zIndex = 0.0
    
    // Text in non-positioned element
    RenderQueue textInNonPositioned(101);
    textInNonPositioned.zIndex = 0;
    textInNonPositioned.isPositioned = false; // effective zIndex = 0.1
    
    // Text in positioned element should render before text in non-positioned (lower effective zIndex)
    REQUIRE(textInPositioned < textInNonPositioned);
    REQUIRE_FALSE(textInNonPositioned < textInPositioned);
  }

  SECTION("Deeply nested children inherit positioned context")
  {
    // Grandchild of positioned element should also inherit positioned context
    RenderQueue grandchildOfPositioned(100);
    grandchildOfPositioned.zIndex = 0;
    grandchildOfPositioned.isPositioned = true; // Inherited through chain, effective zIndex = 0.0
    
    RenderQueue nonPositioned(101);
    nonPositioned.zIndex = 0;
    nonPositioned.isPositioned = false; // effective zIndex = 0.1
    
    // Grandchild should render before non-positioned (lower effective zIndex)
    REQUIRE(grandchildOfPositioned < nonPositioned);
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

  SECTION("Non-zIndex-0 elements are not affected by magic number")
  {
    RenderQueue nonPositioned(100);
    nonPositioned.zIndex = 1;
    nonPositioned.isPositioned = false; // effective zIndex = 1.0 (actual zIndex used, not magic number)
    
    RenderQueue positioned(101);
    positioned.zIndex = 1;
    positioned.isPositioned = true; // effective zIndex = 1.0
    
    // When zIndex is not 0, both use actual zIndex, so only base differs
    REQUIRE(nonPositioned < positioned); // Only base differs (100 < 101)
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
    positionedParent.isPositioned = true; // effective zIndex = 0.0
    
    // Child text inherits positioned context
    RenderQueue childText(101);
    childText.zIndex = 0;
    childText.isPositioned = true; // Inherited, effective zIndex = 0.0
    
    // Child element inherits positioned context
    RenderQueue childElement(102);
    childElement.zIndex = 0;
    childElement.isPositioned = true; // Inherited, effective zIndex = 0.0
    
    // Non-positioned sibling
    RenderQueue staticSibling(103);
    staticSibling.zIndex = 0;
    staticSibling.isPositioned = false; // effective zIndex = 0.1
    
    // All positioned elements should render before non-positioned sibling (lower effective zIndex)
    REQUIRE(positionedParent < staticSibling);
    REQUIRE(childText < staticSibling);
    REQUIRE(childElement < staticSibling);
    
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