#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/dom/dom_event.hpp>
#include <client/dom/dom_event_target.hpp>
#include <memory>
#include <chrono>
#include <thread>

using namespace dom;

TEST_CASE("DOM Event constructor", "[DOM Event]")
{
  auto event = std::make_shared<Event>(DOMEventConstructorType::kEvent, DOMEventType::Click);
  
  REQUIRE(event->constructor() == DOMEventConstructorType::kEvent);
  REQUIRE(event->type == DOMEventType::Click);
  REQUIRE(event->typeStr() == "click");
  REQUIRE(event->bubbles() == true);  // Default
  REQUIRE(event->cancelable() == true); // Default  
  REQUIRE(event->composed() == false);  // Default
  REQUIRE(event->eventPhase() == EventPhase::NONE);
  REQUIRE(event->defaultPrevented() == false);
}

TEST_CASE("DOM Event initialization options", "[DOM Event]")
{
  DOMEventInit init;
  init.bubbles = false;
  init.cancelable = false;
  init.composed = true;
  
  auto event = std::make_shared<Event>(DOMEventConstructorType::kEvent, DOMEventType::Click, init);
  
  REQUIRE(event->bubbles() == false);
  REQUIRE(event->cancelable() == false);
  REQUIRE(event->composed() == true);
}

TEST_CASE("DOM Event preventDefault", "[DOM Event]")
{
  auto event = std::make_shared<Event>(DOMEventConstructorType::kEvent, DOMEventType::Click);
  
  REQUIRE(event->defaultPrevented() == false);
  event->preventDefault();
  REQUIRE(event->defaultPrevented() == true);
}

TEST_CASE("DOM Event preventDefault on non-cancelable event", "[DOM Event]")
{
  DOMEventInit init;
  init.cancelable = false;
  
  auto event = std::make_shared<Event>(DOMEventConstructorType::kEvent, DOMEventType::Click, init);
  
  REQUIRE(event->defaultPrevented() == false);
  event->preventDefault();
  REQUIRE(event->defaultPrevented() == false); // Should not be prevented
}

TEST_CASE("DOM Event stopPropagation", "[DOM Event]")
{
  auto event = std::make_shared<Event>(DOMEventConstructorType::kEvent, DOMEventType::Click);
  
  REQUIRE(event->stopPropagationFlag() == false);
  event->stopPropagation();
  REQUIRE(event->stopPropagationFlag() == true);
}

TEST_CASE("DOM Event stopImmediatePropagation", "[DOM Event]")
{
  auto event = std::make_shared<Event>(DOMEventConstructorType::kEvent, DOMEventType::Click);
  
  REQUIRE(event->stopPropagationFlag() == false);
  REQUIRE(event->stopImmediatePropagationFlag() == false);
  
  event->stopImmediatePropagation();
  
  REQUIRE(event->stopPropagationFlag() == true);
  REQUIRE(event->stopImmediatePropagationFlag() == true);
}

TEST_CASE("DOM Event target handling", "[DOM Event]")
{
  auto event = std::make_shared<Event>(DOMEventConstructorType::kEvent, DOMEventType::Click);
  auto target = std::make_shared<DOMEventTarget>();
  auto currentTarget = std::make_shared<DOMEventTarget>();
  
  // Initially null
  REQUIRE(event->target() == nullptr);
  REQUIRE(event->currentTarget() == nullptr);
  
  // Set target and currentTarget
  event->setTarget(target);
  event->setCurrentTarget(currentTarget);
  
  REQUIRE(event->target() == target);
  REQUIRE(event->currentTarget() == currentTarget);
}

TEST_CASE("DOM Event phase handling", "[DOM Event]")
{
  auto event = std::make_shared<Event>(DOMEventConstructorType::kEvent, DOMEventType::Click);
  
  REQUIRE(event->eventPhase() == EventPhase::NONE);
  
  event->setEventPhase(EventPhase::CAPTURING_PHASE);
  REQUIRE(event->eventPhase() == EventPhase::CAPTURING_PHASE);
  
  event->setEventPhase(EventPhase::AT_TARGET);
  REQUIRE(event->eventPhase() == EventPhase::AT_TARGET);
  
  event->setEventPhase(EventPhase::BUBBLING_PHASE);
  REQUIRE(event->eventPhase() == EventPhase::BUBBLING_PHASE);
}

TEST_CASE("DOM Event timeStamp", "[DOM Event]")
{
  auto event1 = std::make_shared<Event>(DOMEventConstructorType::kEvent, DOMEventType::Click);
  
  // Sleep a bit to ensure different timestamps
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  
  auto event2 = std::make_shared<Event>(DOMEventConstructorType::kEvent, DOMEventType::MouseDown);
  
  // TimeStamp should be set and event2 should have a later timestamp
  REQUIRE(event1->timeStamp() > 0);
  REQUIRE(event2->timeStamp() > 0);
  REQUIRE(event2->timeStamp() > event1->timeStamp());
}

TEST_CASE("DOM Event copy constructor", "[DOM Event]")
{
  auto original = std::make_shared<Event>(DOMEventConstructorType::kEvent, DOMEventType::Click);
  auto target = std::make_shared<DOMEventTarget>();
  
  original->setTarget(target);
  original->setEventPhase(EventPhase::CAPTURING_PHASE);
  original->preventDefault();
  
  auto copy = Event(*original);
  
  REQUIRE(copy.constructor() == original->constructor());
  REQUIRE(copy.type == original->type);
  REQUIRE(copy.target() == original->target());
  REQUIRE(copy.eventPhase() == original->eventPhase());
  REQUIRE(copy.defaultPrevented() == original->defaultPrevented());
}