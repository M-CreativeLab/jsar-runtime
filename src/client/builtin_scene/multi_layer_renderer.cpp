#include <iostream>
#include <memory>
#include <client/layout/layout_view.hpp>
#include <client/layout/layout_object.hpp>
#include <client/layout/layout_object_child_list.hpp>

#include "./multi_layer_renderer.hpp"
#include "./web_content.hpp"

namespace builtin_scene::web_renderer
{
  using namespace std;
  using namespace client_scroll;

  MultiLayerContext::MultiLayerContext()
      : layer_manager_()
      , multi_layer_enabled_(true)
  {
  }

  void MultiLayerInitSystem::onExecute()
  {
    // Initialize the multi-layer context if not already present
    auto multiLayerCtx = getResource<MultiLayerContext>();
    if (multiLayerCtx == nullptr) {
      cout << "Initializing multi-layer rendering context" << endl;
      // The context should be added during app setup
    }
  }

  void MultiLayerRenderSystem::onExecute()
  {
    auto multiLayerCtx = getMultiLayerContext();
    if (!multiLayerCtx || !multiLayerCtx->isMultiLayerEnabled()) {
      // Fall back to single-layer rendering
      RenderBaseSystem::onExecute();
      return;
    }

    auto& layerManager = multiLayerCtx->getLayerManager();
    
    // Process layout objects to identify new scrollable containers
    auto list = queryEntitiesWithComponent<WebContent>([](const WebContent &content) -> bool
                                                       { return content.canvas() != nullptr; });
    
    for (auto &item : list) {
      auto layoutObject = getLayoutObjectFromEntity(item.first);
      if (layoutObject) {
        processLayoutObjectForLayers(layoutObject);
      }
    }
    
    // Update layers after layout changes
    layerManager.updateLayersAfterLayout();
    
    // Render layers in depth order
    renderLayersInOrder();
    
    // Clean up unused layers
    layerManager.cleanupUnusedLayers();
  }

  void MultiLayerRenderSystem::render(ecs::EntityId entity, WebContent &content)
  {
    // This method is called by renderLayersInOrder for each layer
    // Use the existing rendering pipeline but with layer-specific content
    
    if (!content.canvas() || !content.isDirty()) {
      return;
    }

    cout << "Rendering content for entity " << entity << " in multi-layer mode" << endl;
    
    // The actual rendering logic will be handled by the existing
    // RenderBackgroundSystem, RenderImageSystem, etc.
    // We just need to ensure they operate on the correct layer's content
  }

  void MultiLayerRenderSystem::processLayoutObjectForLayers(std::shared_ptr<client_layout::LayoutObject> layoutObject)
  {
    auto multiLayerCtx = getMultiLayerContext();
    if (!multiLayerCtx) return;
    
    auto& layerManager = multiLayerCtx->getLayerManager();
    
    // Check if this layout object should have its own scroll layer
    if (layerManager.shouldCreateScrollLayer(layoutObject)) {
      // Get the fragment for positioning and sizing
      auto fragment = layoutObject->fragment();
      glm::vec3 position(fragment.position().x, fragment.position().y, 0.0f);
      glm::vec2 size(fragment.size().x, fragment.size().y);
      
      auto layer = layerManager.createLayer(position, size);
      layerManager.associateLayerWithLayoutObject(layer->getId(), layoutObject);
      
      // Update layer with fragment information
      layer->updateAfterLayout(fragment);
      
      // Calculate and set layer depth
      int depth = calculateLayerDepth(layoutObject);
      layer->setDepth(depth);
      
      cout << "Created scroll layer for layout object at depth " << depth << endl;
    }
    
    // Recursively process child layout objects
    auto childList = layoutObject->virtualChildren();
    if (childList) {
      for (auto child : *childList) {
        processLayoutObjectForLayers(child);
      }
    }
  }

  void MultiLayerRenderSystem::renderLayersInOrder()
  {
    auto multiLayerCtx = getMultiLayerContext();
    if (!multiLayerCtx) return;
    
    auto& layerManager = multiLayerCtx->getLayerManager();
    auto layers = layerManager.getAllLayersSorted();
    
    cout << "Rendering " << layers.size() << " scroll layers" << endl;
    
    for (auto layer : layers) {
      if (!layer->isVisible() || !layer->isDirty()) {
        continue;
      }
      
      auto webContent = layer->getWebContent();
      if (webContent) {
        // Create a temporary entity ID for this layer's content
        // In a full implementation, each layer would have its own entity
        ecs::EntityId layerEntity = 0; // Placeholder
        render(layerEntity, *webContent);
        
        // Mark layer as clean after rendering
        layer->setDirty(false);
      }
    }
  }

  std::shared_ptr<MultiLayerContext> MultiLayerRenderSystem::getMultiLayerContext()
  {
    return getResource<MultiLayerContext>();
  }

  // Helper method to get layout object from entity (simplified)
  std::shared_ptr<client_layout::LayoutObject> MultiLayerRenderSystem::getLayoutObjectFromEntity(ecs::EntityId entity)
  {
    // In a full implementation, this would properly map entities to layout objects
    // For now, return nullptr as this requires deeper integration
    return nullptr;
  }

  // Helper method to calculate layer depth
  int MultiLayerRenderSystem::calculateLayerDepth(std::shared_ptr<client_layout::LayoutObject> layoutObject)
  {
    int depth = 0;
    auto current = layoutObject;
    while (current && current->parent()) {
      depth++;
      current = current->parent();
    }
    return depth;
  }
}