#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/dom/element.hpp>
#include <client/dom/document.hpp>
#include <client/dom/events/pointer_event.hpp>
#include <memory>

using namespace dom;

// Helper function to create a test element
std::shared_ptr<Element> createTestElement()
{
  auto element = std::make_shared<Element>("div", std::nullopt);
  return element;
}

TEST_CASE("Element setEventHandler and getEventHandlerCode", "[Element][EventHandler]")
{
  auto element = createTestElement();

  // Test setting and getting handler code for click event
  element->setEventHandler("click", "alert('clicked!')");
  REQUIRE(element->getEventHandlerCode("click") == "alert('clicked!')");
  REQUIRE(element->hasEventHandler("click") == true);

  // Test setting and getting handler code for pointerdown event
  element->setEventHandler("pointerdown", "console.log('pointer down')");
  REQUIRE(element->getEventHandlerCode("pointerdown") == "console.log('pointer down')");
  REQUIRE(element->hasEventHandler("pointerdown") == true);

  // Test non-existent event type
  REQUIRE(element->getEventHandlerCode("nonexistent") == "");
  REQUIRE(element->hasEventHandler("nonexistent") == false);
}

TEST_CASE("Element backward compatibility with onclick methods", "[Element][EventHandler]")
{
  auto element = createTestElement();

  // Test that setOnClickHandler works with new system
  element->setOnClickHandler("alert('onclick!')");
  REQUIRE(element->getOnClickHandlerCode() == "alert('onclick!')");
  REQUIRE(element->hasOnClickHandler() == true);
  
  // Test that the new system can retrieve onclick handler
  REQUIRE(element->getEventHandlerCode("click") == "alert('onclick!')");
  REQUIRE(element->hasEventHandler("click") == true);

  // Test that setEventHandler works for click and is accessible via onclick methods
  element->setEventHandler("click", "console.log('new click handler')");
  REQUIRE(element->getOnClickHandlerCode() == "console.log('new click handler')");
  REQUIRE(element->hasOnClickHandler() == true);
}

TEST_CASE("Element setEventHandlerFunction", "[Element][EventHandler]")
{
  auto element = createTestElement();

  // Test setting function reference for click event
  void* dummyFunction = reinterpret_cast<void*>(0x12345678);
  element->setEventHandlerFunction("click", dummyFunction);
  REQUIRE(element->hasEventHandler("click") == true);
  REQUIRE(element->getEventHandlerCode("click") == ""); // Should be empty when function is set

  // Test setting function reference for pointerdown event
  void* anotherFunction = reinterpret_cast<void*>(0x87654321);
  element->setEventHandlerFunction("pointerdown", anotherFunction);
  REQUIRE(element->hasEventHandler("pointerdown") == true);
  REQUIRE(element->getEventHandlerCode("pointerdown") == ""); // Should be empty when function is set
}

TEST_CASE("Element event handler replacement", "[Element][EventHandler]")
{
  auto element = createTestElement();

  // Set handler code first
  element->setEventHandler("click", "alert('first handler')");
  REQUIRE(element->getEventHandlerCode("click") == "alert('first handler')");
  REQUIRE(element->hasEventHandler("click") == true);

  // Replace with function reference
  void* dummyFunction = reinterpret_cast<void*>(0x12345678);
  element->setEventHandlerFunction("click", dummyFunction);
  REQUIRE(element->getEventHandlerCode("click") == ""); // Code should be cleared
  REQUIRE(element->hasEventHandler("click") == true); // But should still have handler

  // Replace with new handler code
  element->setEventHandler("click", "alert('second handler')");
  REQUIRE(element->getEventHandlerCode("click") == "alert('second handler')");
  REQUIRE(element->hasEventHandler("click") == true);
}

TEST_CASE("Element executeEventHandler basic functionality", "[Element][EventHandler]")
{
  auto element = createTestElement();

  // Note: We can't fully test executeEventHandler without a proper document and browsing context
  // But we can test that it doesn't crash and behaves correctly with missing dependencies

  // Test with no handler
  REQUIRE_NOTHROW(element->executeEventHandler("click", nullptr));

  // Test with handler code but no document context
  element->setEventHandler("click", "alert('test')");
  REQUIRE_NOTHROW(element->executeEventHandler("click", nullptr));

  // Test with event object
  auto clickEvent = events::PointerEvent::Click();
  REQUIRE_NOTHROW(element->executeEventHandler("click", clickEvent.get()));
}

TEST_CASE("Element multiple event types", "[Element][EventHandler]")
{
  auto element = createTestElement();

  // Set handlers for multiple event types
  element->setEventHandler("click", "handleClick()");
  element->setEventHandler("pointerdown", "handlePointerDown()");
  element->setEventHandler("pointerup", "handlePointerUp()");
  element->setEventHandler("mouseover", "handleMouseOver()");

  // Verify all are set correctly
  REQUIRE(element->hasEventHandler("click") == true);
  REQUIRE(element->hasEventHandler("pointerdown") == true);
  REQUIRE(element->hasEventHandler("pointerup") == true);
  REQUIRE(element->hasEventHandler("mouseover") == true);

  REQUIRE(element->getEventHandlerCode("click") == "handleClick()");
  REQUIRE(element->getEventHandlerCode("pointerdown") == "handlePointerDown()");
  REQUIRE(element->getEventHandlerCode("pointerup") == "handlePointerUp()");
  REQUIRE(element->getEventHandlerCode("mouseover") == "handleMouseOver()");

  // Verify they don't interfere with each other
  element->setEventHandler("click", "newClickHandler()");
  REQUIRE(element->getEventHandlerCode("click") == "newClickHandler()");
  REQUIRE(element->getEventHandlerCode("pointerdown") == "handlePointerDown()"); // Should be unchanged
}