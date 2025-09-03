#include <catch2/catch.hpp>
#include <memory>

#include "../src/examples/content_bar_canvas.hpp"
#include "../src/examples/content_bar_3d.hpp"
#include "../src/examples/event_proxy.hpp"
#include "../src/examples/bar_component.hpp"

using namespace jsar::example;

// Mock Content class for testing
class TestContent
{
public:
  TestContent(int id) : id_(id) {}
  int getId() const { return id_; }

private:
  int id_;
};

TEST_CASE("EventProxy can register and forward events", "[EventProxy]")
{
  EventProxy proxy;
  bool eventHandled = false;

  // Register a test event handler
  proxy.registerHandler("test_event", [&](Content *content, const std::string &eventType, void *eventData) -> bool
                        {
    eventHandled = true;
    return true; });

  REQUIRE(proxy.hasHandlers("test_event"));

  // Forward a test event
  TestContent testContent(1);
  bool result = proxy.forwardEvent(reinterpret_cast<Content *>(&testContent), "test_event", nullptr);

  REQUIRE(result);
  REQUIRE(eventHandled);
}

TEST_CASE("ContentBarCanvas can generate textures", "[ContentBarCanvas]")
{
  // Note: This test may fail without proper OpenGL context, but tests the basic structure
  ContentBarCanvas canvas;

  int width, height;
  canvas.getTextureDimensions(width, height);

  REQUIRE(width > 0);
  REQUIRE(height > 0);
}

TEST_CASE("ContentBarCanvas handles events correctly", "[ContentBarCanvas]")
{
  ContentBarCanvas canvas;
  TestContent testContent(1);

  // Test hover event
  bool hovered = true;
  bool result = canvas.handleCanvasEvent(reinterpret_cast<Content *>(&testContent), "hover", &hovered);
  REQUIRE(result);

  // Test drag event
  bool dragging = true;
  result = canvas.handleCanvasEvent(reinterpret_cast<Content *>(&testContent), "drag", &dragging);
  REQUIRE(result);

  // Test unknown event
  result = canvas.handleCanvasEvent(reinterpret_cast<Content *>(&testContent), "unknown", nullptr);
  REQUIRE_FALSE(result);
}

TEST_CASE("BarComponent maintains API compatibility", "[BarComponent]")
{
  // Note: This test may fail without proper OpenGL context, but tests the basic structure
  
  // Test that BarComponent can be instantiated
  REQUIRE_NOTHROW([]()
                  {
    BarComponent barComponent; });

  // Test that the facade provides access to components
  BarComponent barComponent;

  REQUIRE(barComponent.getCanvasComponent() != nullptr);
  REQUIRE(barComponent.get3dComponent() != nullptr);
  REQUIRE(barComponent.getEventProxy() != nullptr);
}

TEST_CASE("Refactored architecture maintains separation of concerns", "[Architecture]")
{
  BarComponent barComponent;

  // Verify that components are properly separated
  auto canvas = barComponent.getCanvasComponent();
  auto bar3d = barComponent.get3dComponent();
  auto eventProxy = barComponent.getEventProxy();

  REQUIRE(canvas != nullptr);
  REQUIRE(bar3d != nullptr);
  REQUIRE(eventProxy != nullptr);

  // Verify that each component has distinct responsibilities
  // Canvas handles UI/texture generation
  int width, height;
  canvas->getTextureDimensions(width, height);
  REQUIRE(width > 0);
  REQUIRE(height > 0);

  // EventProxy handles event forwarding
  REQUIRE_FALSE(eventProxy->hasHandlers("nonexistent_event"));
}