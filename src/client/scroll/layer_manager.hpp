#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <client/layout/layout_object.hpp>
#include <client/dom/types.hpp>

#include "./scroll_layer.hpp"

namespace client_scroll
{
  /**
   * Manages multiple scrollable layers for the multi-layer scrolling system.
   * Identifies scrollable containers and creates separate layers for rendering.
   */
  class LayerManager
  {
  public:
    LayerManager();
    ~LayerManager() = default;

    // Layer creation and management
    std::shared_ptr<ScrollLayer> createLayer(const glm::vec3& position, const glm::vec2& size);
    std::shared_ptr<ScrollLayer> getLayer(uint32_t layerId) const;
    void removeLayer(uint32_t layerId);
    
    // Get all layers, sorted by depth
    std::vector<std::shared_ptr<ScrollLayer>> getAllLayersSorted() const;
    
    // Associate layout objects with layers
    void associateLayerWithLayoutObject(uint32_t layerId, std::shared_ptr<client_layout::LayoutObject> layoutObject);
    std::shared_ptr<ScrollLayer> getLayerForLayoutObject(std::shared_ptr<client_layout::LayoutObject> layoutObject) const;
    
    // Layer identification based on CSS properties
    bool shouldCreateScrollLayer(std::shared_ptr<client_layout::LayoutObject> layoutObject) const;
    
    // Update layers after layout changes
    void updateLayersAfterLayout();
    
    // Clean up unused layers
    void cleanupUnusedLayers();

  private:
    uint32_t nextLayerId_;
    std::unordered_map<uint32_t, std::shared_ptr<ScrollLayer>> layers_;
    std::unordered_map<std::shared_ptr<client_layout::LayoutObject>, uint32_t> layoutObjectToLayer_;
    
    // Helper methods
    bool hasScrollableOverflow(std::shared_ptr<client_layout::LayoutObject> layoutObject) const;
    int calculateLayerDepth(std::shared_ptr<client_layout::LayoutObject> layoutObject) const;
  };
}