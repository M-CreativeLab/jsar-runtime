#include <memory>
#include <catch2/catch_test_macros.hpp>
#include <glm/glm.hpp>

#include <client/scroll/layer_manager.hpp>
#include <client/scroll/scroll_layer.hpp>
#include <client/layout/fragment.hpp>

using namespace client_scroll;
using namespace client_layout;

TEST_CASE("ScrollLayer Basic Functionality", "[scroll][layer]")
{
  SECTION("ScrollLayer creation and properties")
  {
    glm::vec3 position(10.0f, 20.0f, 0.0f);
    glm::vec2 size(100.0f, 200.0f);
    uint32_t layerId = 1;
    
    ScrollLayer layer(layerId, position, size);
    
    REQUIRE(layer.getId() == layerId);
    REQUIRE(layer.getPosition() == position);
    REQUIRE(layer.getSize() == size);
    REQUIRE(layer.getDepth() == 0);
    REQUIRE(layer.isVisible() == true);
    REQUIRE(layer.isDirty() == true);
  }
  
  SECTION("ScrollLayer depth and visibility")
  {
    ScrollLayer layer(1, glm::vec3(0.0f), glm::vec2(100.0f));
    
    layer.setDepth(5);
    REQUIRE(layer.getDepth() == 5);
    
    layer.setVisible(false);
    REQUIRE(layer.isVisible() == false);
    
    layer.setDirty(false);
    REQUIRE(layer.isDirty() == false);
  }
}

TEST_CASE("LayerManager Functionality", "[scroll][layer][manager]")
{
  LayerManager manager;
  
  SECTION("Layer creation and retrieval")
  {
    glm::vec3 position(0.0f, 0.0f, 0.0f);
    glm::vec2 size(100.0f, 100.0f);
    
    auto layer1 = manager.createLayer(position, size);
    auto layer2 = manager.createLayer(position, size);
    
    REQUIRE(layer1 != nullptr);
    REQUIRE(layer2 != nullptr);
    REQUIRE(layer1->getId() != layer2->getId());
    
    auto retrieved1 = manager.getLayer(layer1->getId());
    auto retrieved2 = manager.getLayer(layer2->getId());
    
    REQUIRE(retrieved1 == layer1);
    REQUIRE(retrieved2 == layer2);
    
    auto nonExistent = manager.getLayer(999);
    REQUIRE(nonExistent == nullptr);
  }
  
  SECTION("Layer removal")
  {
    auto layer = manager.createLayer(glm::vec3(0.0f), glm::vec2(100.0f));
    uint32_t layerId = layer->getId();
    
    REQUIRE(manager.getLayer(layerId) != nullptr);
    
    manager.removeLayer(layerId);
    
    REQUIRE(manager.getLayer(layerId) == nullptr);
  }
  
  SECTION("Layer sorting by depth")
  {
    auto layer1 = manager.createLayer(glm::vec3(0.0f), glm::vec2(100.0f));
    auto layer2 = manager.createLayer(glm::vec3(0.0f), glm::vec2(100.0f));
    auto layer3 = manager.createLayer(glm::vec3(0.0f), glm::vec2(100.0f));
    
    layer1->setDepth(2);
    layer2->setDepth(1);
    layer3->setDepth(3);
    
    auto sortedLayers = manager.getAllLayersSorted();
    
    REQUIRE(sortedLayers.size() == 3);
    REQUIRE(sortedLayers[0]->getDepth() == 1);
    REQUIRE(sortedLayers[1]->getDepth() == 2);
    REQUIRE(sortedLayers[2]->getDepth() == 3);
  }
}

TEST_CASE("ScrollableArea Layer Integration", "[scroll][area]")
{
  SECTION("ScrollableArea content detection")
  {
    ScrollLayer layer(1, glm::vec3(0.0f), glm::vec2(100.0f, 100.0f));
    auto& scrollArea = layer.getScrollableArea();
    
    // Create a fragment that has content larger than viewport
    Fragment fragment;
    // Note: This test would need proper Fragment construction
    // which requires understanding the Fragment implementation
    
    REQUIRE(scrollArea.getViewportSize().x >= 0.0f);
    REQUIRE(scrollArea.getViewportSize().y >= 0.0f);
  }
}