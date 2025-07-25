#include <iostream>
#include <unordered_map>
#include <glm/glm.hpp>
#include <client/layout/layout_object.hpp>
#include <client/layout/fragment.hpp>

#include "./multi_layer_scroll_api.hpp"
#include "./layer_manager.hpp"
#include "./scroll_layer.hpp"

// Global state for multi-layer scrolling
namespace {
  std::unique_ptr<client_scroll::LayerManager> g_layerManager = nullptr;
  bool g_multiLayerEnabled = true;
  
  client_scroll::LayerManager& getGlobalLayerManager() {
    if (!g_layerManager) {
      g_layerManager = std::make_unique<client_scroll::LayerManager>();
    }
    return *g_layerManager;
  }
}

namespace jsar_multi_layer_scroll {
  
  bool shouldCreateScrollLayer(std::shared_ptr<client_layout::LayoutObject> layoutObject) {
    if (!g_multiLayerEnabled || !layoutObject) {
      return false;
    }
    
    auto& manager = getGlobalLayerManager();
    return manager.shouldCreateScrollLayer(layoutObject);
  }
  
  uint32_t createScrollLayerIfNeeded(std::shared_ptr<client_layout::LayoutObject> layoutObject) {
    if (!shouldCreateScrollLayer(layoutObject)) {
      return 0;
    }
    
    auto& manager = getGlobalLayerManager();
    
    // Get fragment for positioning
    auto fragment = layoutObject->fragment();
    glm::vec3 position(fragment.position().x, fragment.position().y, 0.0f);
    glm::vec2 size(fragment.size().x, fragment.size().y);
    
    auto layer = manager.createLayer(position, size);
    manager.associateLayerWithLayoutObject(layer->getId(), layoutObject);
    
    layer->updateAfterLayout(fragment);
    
    std::cout << "Created scroll layer " << layer->getId() 
              << " for layout object at (" << position.x << ", " << position.y << ")" << std::endl;
    
    return layer->getId();
  }
  
  void updateScrollLayersAfterLayout() {
    if (!g_multiLayerEnabled) {
      return;
    }
    
    auto& manager = getGlobalLayerManager();
    manager.updateLayersAfterLayout();
  }
  
  glm::vec3 getScrollOffsetForLayoutObject(std::shared_ptr<client_layout::LayoutObject> layoutObject) {
    if (!g_multiLayerEnabled || !layoutObject) {
      return glm::vec3(0.0f);
    }
    
    auto& manager = getGlobalLayerManager();
    auto layer = manager.getLayerForLayoutObject(layoutObject);
    if (layer) {
      return layer->getScrollableArea().getScrollOffset();
    }
    
    return glm::vec3(0.0f);
  }
  
  bool scrollLayoutObject(std::shared_ptr<client_layout::LayoutObject> layoutObject, const glm::vec3& offset) {
    if (!g_multiLayerEnabled || !layoutObject) {
      return false;
    }
    
    auto& manager = getGlobalLayerManager();
    auto layer = manager.getLayerForLayoutObject(layoutObject);
    if (layer) {
      layer->getScrollableArea().scrollBy(offset);
      layer->setDirty(true);
      
      std::cout << "Scrolled layer " << layer->getId() 
                << " by (" << offset.x << ", " << offset.y << ", " << offset.z << ")" << std::endl;
      return true;
    }
    
    return false;
  }
  
  size_t getActiveLayerCount() {
    if (!g_multiLayerEnabled) {
      return 0;
    }
    
    auto& manager = getGlobalLayerManager();
    return manager.getAllLayersSorted().size();
  }
  
  void setMultiLayerScrollingEnabled(bool enabled) {
    g_multiLayerEnabled = enabled;
    std::cout << "Multi-layer scrolling " << (enabled ? "enabled" : "disabled") << std::endl;
  }
  
  bool isMultiLayerScrollingEnabled() {
    return g_multiLayerEnabled;
  }
}

namespace jsar_multi_layer_integration {
  
  void initializeMultiLayerScrolling() {
    std::cout << "Initializing multi-layer scrolling system" << std::endl;
    
    // Initialize the global layer manager
    getGlobalLayerManager();
    
    // Set default enabled state
    jsar_multi_layer_scroll::setMultiLayerScrollingEnabled(true);
  }
  
  void shutdownMultiLayerScrolling() {
    std::cout << "Shutting down multi-layer scrolling system" << std::endl;
    
    if (g_layerManager) {
      g_layerManager.reset();
    }
  }
  
  void onLayoutComplete() {
    jsar_multi_layer_scroll::updateScrollLayersAfterLayout();
  }
  
  std::vector<uint32_t> getLayersToRender() {
    std::vector<uint32_t> layerIds;
    
    if (!jsar_multi_layer_scroll::isMultiLayerScrollingEnabled()) {
      return layerIds;
    }
    
    auto& manager = getGlobalLayerManager();
    auto layers = manager.getAllLayersSorted();
    
    for (auto layer : layers) {
      if (layer->isVisible()) {
        layerIds.push_back(layer->getId());
      }
    }
    
    return layerIds;
  }
  
  bool routeScrollEvent(float x, float y, const glm::vec3& scrollDelta) {
    if (!jsar_multi_layer_scroll::isMultiLayerScrollingEnabled()) {
      return false;
    }
    
    auto& manager = getGlobalLayerManager();
    auto layers = manager.getAllLayersSorted();
    
    // Find the topmost layer that contains the point (x, y)
    for (auto it = layers.rbegin(); it != layers.rend(); ++it) {
      auto layer = *it;
      if (!layer->isVisible()) {
        continue;
      }
      
      auto position = layer->getPosition();
      auto size = layer->getSize();
      
      // Simple point-in-rectangle test
      if (x >= position.x && x <= position.x + size.x &&
          y >= position.y && y <= position.y + size.y) {
        
        layer->getScrollableArea().scrollBy(scrollDelta);
        layer->setDirty(true);
        
        std::cout << "Routed scroll event to layer " << layer->getId() << std::endl;
        return true;
      }
    }
    
    return false;
  }
}