/**
 * Multi-Layer Scrolling Usage Example
 * 
 * This example demonstrates how to integrate the multi-layer scrolling system
 * with the existing JSAR runtime layout and rendering pipeline.
 */

#include <iostream>
#include <memory>
#include <client/scroll/multi_layer_scroll_api.hpp>

// Example: How to integrate with the layout system
void exampleLayoutIntegration() {
  std::cout << "=== Multi-Layer Scrolling Integration Example ===" << std::endl;
  
  // 1. Initialize the multi-layer scrolling system (called once at startup)
  jsar_multi_layer_integration::initializeMultiLayerScrolling();
  
  // 2. Check if the system is enabled
  if (jsar_multi_layer_scroll::isMultiLayerScrollingEnabled()) {
    std::cout << "Multi-layer scrolling is enabled" << std::endl;
  }
  
  // 3. Simulate layout objects (in real implementation, these would come from DOM)
  std::cout << "\nSimulating layout process..." << std::endl;
  
  // Note: In real implementation, these would be actual LayoutObject instances
  // from the DOM tree that have overflow: scroll or auto
  
  std::cout << "- Processing layout object with scrollable content" << std::endl;
  std::cout << "- Checking if scroll layer should be created" << std::endl;
  
  // In the real implementation:
  // for each LayoutObject with scrollable content:
  //   uint32_t layerId = jsar_multi_layer_scroll::createScrollLayerIfNeeded(layoutObject);
  //   if (layerId != 0) {
  //     std::cout << "Created scroll layer: " << layerId << std::endl;
  //   }
  
  // 4. After layout is complete, update all layers
  jsar_multi_layer_integration::onLayoutComplete();
  
  std::cout << "Active scroll layers: " << jsar_multi_layer_scroll::getActiveLayerCount() << std::endl;
}

// Example: How to handle scroll events
void exampleScrollEventHandling() {
  std::cout << "\n=== Scroll Event Handling Example ===" << std::endl;
  
  // Simulate a scroll event at position (100, 100) with delta (0, -10, 0)
  float mouseX = 100.0f;
  float mouseY = 100.0f;
  glm::vec3 scrollDelta(0.0f, -10.0f, 0.0f);  // Scroll up by 10 pixels
  
  std::cout << "Simulating scroll event at (" << mouseX << ", " << mouseY << ")" << std::endl;
  std::cout << "Scroll delta: (" << scrollDelta.x << ", " << scrollDelta.y << ", " << scrollDelta.z << ")" << std::endl;
  
  bool handled = jsar_multi_layer_integration::routeScrollEvent(mouseX, mouseY, scrollDelta);
  
  if (handled) {
    std::cout << "Scroll event was handled by a scroll layer" << std::endl;
  } else {
    std::cout << "Scroll event not handled - no scroll layer at this position" << std::endl;
  }
}

// Example: How to integrate with the rendering system
void exampleRenderingIntegration() {
  std::cout << "\n=== Rendering Integration Example ===" << std::endl;
  
  // Get layers that need to be rendered, in depth order
  auto layersToRender = jsar_multi_layer_integration::getLayersToRender();
  
  std::cout << "Layers to render: " << layersToRender.size() << std::endl;
  
  for (uint32_t layerId : layersToRender) {
    std::cout << "- Rendering layer " << layerId << std::endl;
    
    // In real implementation:
    // 1. Get the WebContent for this layer
    // 2. Apply the scroll offset as a transform
    // 3. Render the layer's content to its surface
    // 4. Composite the layer surface with the main render target
  }
}

// Example: Configuration and control
void exampleConfiguration() {
  std::cout << "\n=== Configuration Example ===" << std::endl;
  
  // Toggle multi-layer scrolling
  std::cout << "Multi-layer scrolling enabled: " << jsar_multi_layer_scroll::isMultiLayerScrollingEnabled() << std::endl;
  
  jsar_multi_layer_scroll::setMultiLayerScrollingEnabled(false);
  std::cout << "Disabled multi-layer scrolling" << std::endl;
  std::cout << "Multi-layer scrolling enabled: " << jsar_multi_layer_scroll::isMultiLayerScrollingEnabled() << std::endl;
  
  jsar_multi_layer_scroll::setMultiLayerScrollingEnabled(true);
  std::cout << "Re-enabled multi-layer scrolling" << std::endl;
}

int main() {
  std::cout << "JSAR Runtime Multi-Layer Scrolling System Example" << std::endl;
  std::cout << "==================================================" << std::endl;
  
  try {
    exampleLayoutIntegration();
    exampleScrollEventHandling();
    exampleRenderingIntegration();
    exampleConfiguration();
    
    // Cleanup
    jsar_multi_layer_integration::shutdownMultiLayerScrolling();
    
    std::cout << "\n=== Example completed successfully ===" << std::endl;
    
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}

/**
 * Integration Points Summary:
 * 
 * 1. Layout System Integration:
 *    - Call createScrollLayerIfNeeded() for each layout object during layout
 *    - Call onLayoutComplete() after layout is finished
 * 
 * 2. Rendering System Integration:
 *    - Call getLayersToRender() to get layers in depth order
 *    - Render each layer separately with its scroll offset applied
 *    - Composite layers in the correct order
 * 
 * 3. Input System Integration:
 *    - Call routeScrollEvent() for scroll input events
 *    - The system will automatically route to the appropriate layer
 * 
 * 4. CSS Integration:
 *    - shouldCreateScrollLayer() checks CSS overflow properties
 *    - Layers are created for elements with overflow: scroll or auto
 *    - Content larger than container triggers layer creation
 * 
 * 5. Performance Benefits:
 *    - Each scrollable container renders independently
 *    - Only dirty layers re-render on scroll
 *    - Reduced overdraw and better GPU utilization
 *    - Smooth scrolling for nested scrollable areas
 */