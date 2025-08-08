#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <memory>
#include <vector>
#include <client/builtin_scene/layers/LayerTypes.hpp>
#include <client/builtin_scene/layers/RenderListBuilder.hpp>
#include <client/builtin_scene/layers/ClipStack.hpp>
#include <client/builtin_scene/ecs.hpp>
#include <client/builtin_scene/web_content.hpp>

using namespace builtin_scene::layers;
using namespace builtin_scene;

TEST_CASE("LayerNode basic functionality", "[layers]")
{
  SECTION("LayerNode creation and properties")
  {
    ecs::EntityId testEntity(42);
    LayerNode node(testEntity, 1);
    
    REQUIRE(node.entityId == testEntity);
    REQUIRE(node.layerIndex == 1);
    REQUIRE(node.zIndex == 0);
    REQUIRE(node.translateZ == Catch::Approx(0.0f));
    REQUIRE(node.scrollOffset == glm::vec3(0.0f));
    REQUIRE(node.transform == glm::mat4(1.0f));
    REQUIRE_FALSE(node.isScrollable);
    REQUIRE_FALSE(node.hasOverflow);
    REQUIRE_FALSE(node.needsClipping);
  }
  
  SECTION("LayerNode parent-child relationships")
  {
    auto parent = std::make_shared<LayerNode>(ecs::EntityId(1), 0);
    auto child = std::make_shared<LayerNode>(ecs::EntityId(2), 1);
    
    child->parent = parent;
    parent->children.push_back(child);
    
    REQUIRE(parent->children.size() == 1);
    REQUIRE(parent->children[0] == child);
    REQUIRE(child->parent.lock() == parent);
  }
  
  SECTION("LayerNode absolute transform calculation")
  {
    auto parent = std::make_shared<LayerNode>(ecs::EntityId(1), 0);
    auto child = std::make_shared<LayerNode>(ecs::EntityId(2), 1);
    
    // Set scroll offsets
    parent->scrollOffset = glm::vec3(10.0f, 20.0f, 0.0f);
    child->scrollOffset = glm::vec3(5.0f, 10.0f, 0.0f);
    
    // Establish relationship
    child->parent = parent;
    parent->children.push_back(child);
    
    // Test accumulated scroll offset
    glm::vec3 accumulated = child->getAccumulatedScrollOffset();
    REQUIRE(accumulated.x == Catch::Approx(15.0f));
    REQUIRE(accumulated.y == Catch::Approx(30.0f));
    REQUIRE(accumulated.z == Catch::Approx(0.0f));
  }
}

TEST_CASE("RenderCommand creation", "[layers]")
{
  SECTION("RenderCommand basic properties")
  {
    ecs::EntityId testEntity(42);
    RenderCommand cmd(RenderCommand::kRenderWebContent, testEntity);
    
    REQUIRE(cmd.type == RenderCommand::kRenderWebContent);
    REQUIRE(cmd.entityId == testEntity);
    REQUIRE_FALSE(cmd.useScissor);
    REQUIRE_FALSE(cmd.useStencil);
    REQUIRE(cmd.transform == glm::mat4(1.0f));
    REQUIRE(cmd.scrollOffset == glm::vec3(0.0f));
  }
  
  SECTION("Clip command creation")
  {
    RenderCommand clipCmd(RenderCommand::kPushClip);
    SkRect rect = SkRect::MakeXYWH(10, 10, 100, 100);
    clipCmd.clipRect = rect;
    clipCmd.useScissor = true;
    
    REQUIRE(clipCmd.type == RenderCommand::kPushClip);
    REQUIRE(clipCmd.clipRect.has_value());
    REQUIRE(clipCmd.clipRect.value() == rect);
    REQUIRE(clipCmd.useScissor);
  }
}

TEST_CASE("LayerRenderConfig defaults", "[layers]")
{
  LayerRenderConfig config;
  
  REQUIRE_FALSE(config.enableDebugOverlay);
  REQUIRE(config.preferScissorClipping);
  REQUIRE(config.enableStencilClipping);
  REQUIRE(config.enableBatching);
  REQUIRE(config.devicePixelRatio == Catch::Approx(1.0f));
}

TEST_CASE("ClipStack basic operations", "[layers]")
{
  LayerRenderConfig config;
  ClipStack clipStack(config);
  
  SECTION("Empty stack")
  {
    REQUIRE(clipStack.empty());
    REQUIRE(clipStack.depth() == 0);
    REQUIRE_FALSE(clipStack.getCurrentClipRect().has_value());
  }
  
  SECTION("Scissor clipping decision")
  {
    // ClipStack methods are private, so we can only test public interface
    // The actual scissor vs stencil decision is tested through integration
    REQUIRE(clipStack.empty());
  }
}

// Integration test would require a full ECS context
// This test demonstrates the layer system structure
TEST_CASE("Layer system integration concept", "[layers]")
{
  SECTION("Layer hierarchy concept")
  {
    // Create a simple 3-layer hierarchy:
    // Layer 0 (root container)
    //   ├── Layer 1 (scrollable container) 
    //       └── Layer 2 (nested content)
    
    auto layer0 = std::make_shared<LayerNode>(ecs::EntityId(100), 0);
    auto layer1 = std::make_shared<LayerNode>(ecs::EntityId(101), 1);
    auto layer2 = std::make_shared<LayerNode>(ecs::EntityId(102), 2);
    
    // Set up hierarchy
    layer1->parent = layer0;
    layer0->children.push_back(layer1);
    
    layer2->parent = layer1;
    layer1->children.push_back(layer2);
    
    // Configure layer properties
    layer0->clipRect = SkRect::MakeXYWH(0, 0, 400, 300);
    layer0->needsClipping = true;
    
    layer1->clipRect = SkRect::MakeXYWH(10, 10, 200, 150);
    layer1->needsClipping = true;
    layer1->isScrollable = true;
    layer1->scrollOffset = glm::vec3(20.0f, 30.0f, 0.0f);
    
    layer2->clipRect = SkRect::MakeXYWH(5, 5, 180, 100);
    layer2->needsClipping = true;
    
    // Verify hierarchy
    REQUIRE(layer0->children.size() == 1);
    REQUIRE(layer1->children.size() == 1);
    REQUIRE(layer2->children.size() == 0);
    
    // Verify layer indices are in ascending order (painter's order)
    REQUIRE(layer0->layerIndex < layer1->layerIndex);
    REQUIRE(layer1->layerIndex < layer2->layerIndex);
    
    // Verify scroll offset accumulation
    glm::vec3 accumulatedScroll = layer2->getAccumulatedScrollOffset();
    REQUIRE(accumulatedScroll.x == Catch::Approx(20.0f));
    REQUIRE(accumulatedScroll.y == Catch::Approx(30.0f));
    
    // This conceptually demonstrates how the render commands would be ordered:
    // 1. PushClip (layer0)
    // 2. RenderWebContent (layer0)
    // 3. PushTransform (layer1 scroll offset)
    // 4. PushClip (layer1)
    // 5. RenderWebContent (layer1)
    // 6. PushClip (layer2)
    // 7. RenderWebContent (layer2)
    // 8. PopClip (layer2)
    // 9. PopClip (layer1)
    // 10. PopTransform (layer1)
    // 11. PopClip (layer0)
  }
}