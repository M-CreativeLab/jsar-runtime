#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/dom/element.hpp>
#include <client/html/html_element.hpp>
#include <client/dom/events/pointer_event.hpp>
#include <memory>

using namespace dom;

TEST_CASE("Element integration test - onclick to click event migration", "[Element][Integration]")
{
  auto element = std::make_shared<Element>("div", std::nullopt);

  // Test 1: Legacy onclick handler should work with new system
  element->setOnClickHandler("console.log('legacy onclick')");
  
  // Verify it's accessible through both old and new APIs
  REQUIRE(element->hasOnClickHandler() == true);
  REQUIRE(element->hasEventHandler("click") == true);
  REQUIRE(element->getOnClickHandlerCode() == "console.log('legacy onclick')");
  REQUIRE(element->getEventHandlerCode("click") == "console.log('legacy onclick')");

  // Test 2: New system should work for click events
  element->setEventHandler("click", "console.log('new click handler')");
  
  // Should be accessible through both APIs
  REQUIRE(element->hasOnClickHandler() == true);
  REQUIRE(element->hasEventHandler("click") == true);
  REQUIRE(element->getOnClickHandlerCode() == "console.log('new click handler')");
  REQUIRE(element->getEventHandlerCode("click") == "console.log('new click handler')");

  // Test 3: Non-click events should only work with new API
  element->setEventHandler("pointerdown", "console.log('pointer down')");
  
  REQUIRE(element->hasEventHandler("pointerdown") == true);
  REQUIRE(element->getEventHandlerCode("pointerdown") == "console.log('pointer down')");
  
  // onclick methods should not be affected by non-click handlers
  REQUIRE(element->getOnClickHandlerCode() == "console.log('new click handler')");

  // Test 4: Multiple event types can coexist
  element->setEventHandler("mouseover", "console.log('mouse over')");
  element->setEventHandler("mouseout", "console.log('mouse out')");
  
  REQUIRE(element->hasEventHandler("click") == true);
  REQUIRE(element->hasEventHandler("pointerdown") == true);
  REQUIRE(element->hasEventHandler("mouseover") == true);
  REQUIRE(element->hasEventHandler("mouseout") == true);
  
  // Each should have its own handler
  REQUIRE(element->getEventHandlerCode("click") == "console.log('new click handler')");
  REQUIRE(element->getEventHandlerCode("pointerdown") == "console.log('pointer down')");
  REQUIRE(element->getEventHandlerCode("mouseover") == "console.log('mouse over')");
  REQUIRE(element->getEventHandlerCode("mouseout") == "console.log('mouse out')");
}

TEST_CASE("Element simulation integration", "[Element][Integration][Simulation]")
{
  auto element = std::make_shared<Element>("button", std::nullopt);

  // Set up a click handler
  element->setEventHandler("click", "handleClick()");
  REQUIRE(element->hasEventHandler("click") == true);

  // Simulate click should not crash even without proper document context
  REQUIRE_NOTHROW(element->simulateClick(glm::vec3(0.0f, 0.0f, 0.0f)));

  // Test backward compatibility: onclick handler should also work
  element->setOnClickHandler("legacyOnClick()");
  REQUIRE_NOTHROW(element->simulateClick(glm::vec3(1.0f, 1.0f, 1.0f)));
}

TEST_CASE("Function reference handling", "[Element][Integration][Function]")
{
  auto element = std::make_shared<Element>("span", std::nullopt);

  // Test function reference for click
  void* clickFunction = reinterpret_cast<void*>(0x12345678);
  element->setEventHandlerFunction("click", clickFunction);
  
  REQUIRE(element->hasEventHandler("click") == true);
  REQUIRE(element->hasOnClickHandler() == true); // Should be accessible through legacy API
  REQUIRE(element->getEventHandlerCode("click") == ""); // Code should be empty
  REQUIRE(element->getOnClickHandlerCode() == ""); // Legacy API should also return empty

  // Test function reference for non-click event
  void* pointerFunction = reinterpret_cast<void*>(0x87654321);
  element->setEventHandlerFunction("pointerdown", pointerFunction);
  
  REQUIRE(element->hasEventHandler("pointerdown") == true);
  REQUIRE(element->getEventHandlerCode("pointerdown") == "");

  // Setting code should clear function reference
  element->setEventHandler("click", "newClickHandler()");
  REQUIRE(element->hasEventHandler("click") == true);
  REQUIRE(element->getEventHandlerCode("click") == "newClickHandler()");

  // pointerdown function should still exist
  REQUIRE(element->hasEventHandler("pointerdown") == true);
}

TEST_CASE("Event type case sensitivity and validation", "[Element][Integration][Types]")
{
  auto element = std::make_shared<Element>("div", std::nullopt);

  // Test various event types that should be supported
  std::vector<std::string> eventTypes = {
    "click", "pointerdown", "pointerup", "pointermove",
    "mousedown", "mouseup", "mousemove", "mouseover", "mouseout",
    "keydown", "keyup", "focus", "blur"
  };

  for (const auto& eventType : eventTypes)
  {
    std::string handlerCode = "handle" + eventType + "()";
    element->setEventHandler(eventType, handlerCode);
    
    REQUIRE(element->hasEventHandler(eventType) == true);
    REQUIRE(element->getEventHandlerCode(eventType) == handlerCode);
  }

  // Verify they don't interfere with each other
  for (const auto& eventType : eventTypes)
  {
    std::string expectedCode = "handle" + eventType + "()";
    REQUIRE(element->getEventHandlerCode(eventType) == expectedCode);
  }
}