/**
 * Example usage of the new generalized event handler system
 * This demonstrates how the refactored Element::executeEventHandler() 
 * can be used to handle multiple event types beyond just onclick.
 */

#include <client/dom/element.hpp>
#include <client/dom/events/pointer_event.hpp>
#include <client/dom/events/mouse_event.hpp>
#include <memory>
#include <iostream>

using namespace dom;

void demonstrateNewEventHandlerSystem()
{
  // Create an element (in real usage, this would be from document.createElement or HTML parsing)
  auto button = std::make_shared<Element>("button", std::nullopt);

  std::cout << "=== New Generalized Event Handler System Demo ===" << std::endl;

  // 1. Set handlers for multiple event types
  std::cout << "\n1. Setting handlers for multiple event types:" << std::endl;
  
  button->setEventHandler("click", "console.log('Button clicked!'); handleClick(event);");
  button->setEventHandler("pointerdown", "console.log('Pointer down on button'); event.preventDefault();");
  button->setEventHandler("pointerup", "console.log('Pointer up on button'); handlePointerUp(event);");
  button->setEventHandler("mouseover", "this.style.backgroundColor = 'lightblue';");
  button->setEventHandler("mouseout", "this.style.backgroundColor = '';");
  
  std::cout << "  ✓ Set click handler" << std::endl;
  std::cout << "  ✓ Set pointerdown handler" << std::endl;
  std::cout << "  ✓ Set pointerup handler" << std::endl;
  std::cout << "  ✓ Set mouseover handler" << std::endl;
  std::cout << "  ✓ Set mouseout handler" << std::endl;

  // 2. Verify handlers are set correctly
  std::cout << "\n2. Verifying handlers:" << std::endl;
  
  std::vector<std::string> eventTypes = {"click", "pointerdown", "pointerup", "mouseover", "mouseout"};
  for (const auto& type : eventTypes)
  {
    if (button->hasEventHandler(type))
    {
      std::cout << "  ✓ " << type << ": " << button->getEventHandlerCode(type) << std::endl;
    }
  }

  // 3. Demonstrate backward compatibility
  std::cout << "\n3. Backward compatibility check:" << std::endl;
  
  // The click handler should also be accessible via legacy onclick API
  if (button->hasOnClickHandler())
  {
    std::cout << "  ✓ Legacy onclick API still works: " << button->getOnClickHandlerCode() << std::endl;
  }

  // 4. Simulate events (this would execute the handlers in a real environment)
  std::cout << "\n4. Simulating events:" << std::endl;
  
  // Create event objects
  auto clickEvent = events::PointerEvent::Click();
  auto pointerDownEvent = events::PointerEvent::PointerDown();
  auto mouseOverEvent = events::MouseEvent::MouseOver();

  // Execute handlers with event objects (the key improvement!)
  std::cout << "  ⚡ Executing click handler with PointerEvent..." << std::endl;
  button->executeEventHandler("click", clickEvent.get());
  
  std::cout << "  ⚡ Executing pointerdown handler with PointerEvent..." << std::endl;
  button->executeEventHandler("pointerdown", pointerDownEvent.get());
  
  std::cout << "  ⚡ Executing mouseover handler with MouseEvent..." << std::endl;
  button->executeEventHandler("mouseover", mouseOverEvent.get());

  // 5. Demonstrate handler replacement
  std::cout << "\n5. Handler replacement:" << std::endl;
  
  button->setEventHandler("click", "alert('New click handler!'); newHandleClick(event);");
  std::cout << "  ✓ Updated click handler: " << button->getEventHandlerCode("click") << std::endl;

  // 6. Show function reference support
  std::cout << "\n6. Function reference support:" << std::endl;
  
  void* dummyFunction = reinterpret_cast<void*>(0x12345678);
  button->setEventHandlerFunction("focus", dummyFunction);
  
  if (button->hasEventHandler("focus"))
  {
    std::cout << "  ✓ Function reference set for focus event" << std::endl;
    std::cout << "  ✓ Handler code is empty (as expected): '" << button->getEventHandlerCode("focus") << "'" << std::endl;
  }

  std::cout << "\n=== Demo Complete ===" << std::endl;
  std::cout << "\nKey improvements:" << std::endl;
  std::cout << "  • Support for any event type (not just onclick)" << std::endl;
  std::cout << "  • Event objects passed to handlers" << std::endl;
  std::cout << "  • Backward compatibility maintained" << std::endl;
  std::cout << "  • Clean, extensible API design" << std::endl;
}

void demonstrateUsageScenarios()
{
  std::cout << "\n=== Real-World Usage Scenarios ===" << std::endl;

  auto interactiveElement = std::make_shared<Element>("div", std::nullopt);

  // Scenario 1: Interactive UI element with multiple touch/mouse events
  std::cout << "\nScenario 1: Interactive UI with touch/mouse support" << std::endl;
  
  interactiveElement->setEventHandler("pointerdown", 
    "this.classList.add('pressed'); startInteraction(event);");
  interactiveElement->setEventHandler("pointerup", 
    "this.classList.remove('pressed'); endInteraction(event);");
  interactiveElement->setEventHandler("pointermove", 
    "if (event.pointerType === 'mouse') handleMouseMove(event);");
  
  std::cout << "  ✓ Set up touch/mouse interaction handlers" << std::endl;

  // Scenario 2: Form validation with multiple event types
  auto inputElement = std::make_shared<Element>("input", std::nullopt);
  
  std::cout << "\nScenario 2: Form input with validation" << std::endl;
  
  inputElement->setEventHandler("focus", "highlightField(event.target);");
  inputElement->setEventHandler("blur", "validateField(event.target); removeHighlight(event.target);");
  inputElement->setEventHandler("keydown", "if (event.key === 'Enter') submitForm(event);");
  inputElement->setEventHandler("input", "liveValidation(event.target.value);");
  
  std::cout << "  ✓ Set up form validation handlers" << std::endl;

  // Scenario 3: Media player controls
  auto mediaElement = std::make_shared<Element>("video", std::nullopt);
  
  std::cout << "\nScenario 3: Media player controls" << std::endl;
  
  mediaElement->setEventHandler("click", "togglePlayPause(event.target);");
  mediaElement->setEventHandler("pointerdown", "startSeek(event);");
  mediaElement->setEventHandler("pointermove", "if (seeking) updateSeekPosition(event);");
  mediaElement->setEventHandler("pointerup", "endSeek(event);");
  
  std::cout << "  ✓ Set up media control handlers" << std::endl;

  std::cout << "\nAll scenarios demonstrate the flexibility of the new system!" << std::endl;
}

int main()
{
  demonstrateNewEventHandlerSystem();
  demonstrateUsageScenarios();
  return 0;
}