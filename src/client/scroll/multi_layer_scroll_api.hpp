#pragma once

/**
 * Multi-Layer Scrolling Integration for JSAR Runtime
 * 
 * This header provides the main integration points for the multi-layer scrolling system.
 * It's designed to be included by the existing layout and rendering systems.
 */

#include <memory>
#include <vector>
#include <glm/glm.hpp>

// Forward declarations to minimize dependencies
namespace client_scroll {
  class LayerManager;
  class ScrollLayer;
  class ScrollableArea;
}

namespace client_layout {
  class LayoutObject;
  class Fragment;
}

namespace builtin_scene {
  class WebContent;

  namespace web_renderer {
    class MultiLayerContext;
    class MultiLayerRenderSystem;
  }
}

/**
 * Helper functions for integration with existing systems
 */
namespace jsar_multi_layer_scroll {
  
  /**
   * Check if a layout object should create a new scroll layer.
   * This function checks CSS overflow properties and content size.
   */
  bool shouldCreateScrollLayer(std::shared_ptr<client_layout::LayoutObject> layoutObject);
  
  /**
   * Create a scroll layer for the given layout object if needed.
   * Returns the layer ID, or 0 if no layer was created.
   */
  uint32_t createScrollLayerIfNeeded(std::shared_ptr<client_layout::LayoutObject> layoutObject);
  
  /**
   * Update all scroll layers after layout changes.
   * This should be called from the layout system after layout is complete.
   */
  void updateScrollLayersAfterLayout();
  
  /**
   * Get the scroll offset for a given layout object.
   * Returns (0,0,0) if the object doesn't have an associated scroll layer.
   */
  glm::vec3 getScrollOffsetForLayoutObject(std::shared_ptr<client_layout::LayoutObject> layoutObject);
  
  /**
   * Apply scroll to a layout object by the given offset.
   * Returns true if scroll was applied, false if the object is not scrollable.
   */
  bool scrollLayoutObject(std::shared_ptr<client_layout::LayoutObject> layoutObject, const glm::vec3& offset);
  
  /**
   * Get the number of active scroll layers.
   */
  size_t getActiveLayerCount();
  
  /**
   * Enable or disable multi-layer scrolling system.
   * When disabled, falls back to single-layer rendering.
   */
  void setMultiLayerScrollingEnabled(bool enabled);
  
  /**
   * Check if multi-layer scrolling is currently enabled.
   */
  bool isMultiLayerScrollingEnabled();
}

/**
 * Integration hooks for existing systems
 */
namespace jsar_multi_layer_integration {
  
  /**
   * Initialize the multi-layer scrolling system.
   * Should be called during app startup.
   */
  void initializeMultiLayerScrolling();
  
  /**
   * Shutdown the multi-layer scrolling system.
   * Should be called during app shutdown.
   */
  void shutdownMultiLayerScrolling();
  
  /**
   * Hook for layout system: call when layout is complete.
   */
  void onLayoutComplete();
  
  /**
   * Hook for rendering system: get layers to render in order.
   */
  std::vector<uint32_t> getLayersToRender();
  
  /**
   * Hook for input system: route scroll events to appropriate layers.
   */
  bool routeScrollEvent(float x, float y, const glm::vec3& scrollDelta);
}