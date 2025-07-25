#include <algorithm>
#include <iostream>
#include <client/cssom/computed_style.hpp>
#include <client/cssom/values/computed/overflow.hpp>

#include "./layer_manager.hpp"

namespace client_scroll
{
  using namespace std;
  using namespace client_cssom;
  using namespace client_cssom::values::computed;

  LayerManager::LayerManager()
      : nextLayerId_(1)
  {
  }

  std::shared_ptr<ScrollLayer> LayerManager::createLayer(const glm::vec3& position, const glm::vec2& size)
  {
    uint32_t layerId = nextLayerId_++;
    auto layer = std::make_shared<ScrollLayer>(layerId, position, size);
    layers_[layerId] = layer;
    
    cout << "Created scroll layer " << layerId << " at position (" 
         << position.x << ", " << position.y << ", " << position.z 
         << ") with size (" << size.x << ", " << size.y << ")" << endl;
    
    return layer;
  }

  std::shared_ptr<ScrollLayer> LayerManager::getLayer(uint32_t layerId) const
  {
    auto it = layers_.find(layerId);
    return (it != layers_.end()) ? it->second : nullptr;
  }

  void LayerManager::removeLayer(uint32_t layerId)
  {
    // Remove from layoutObjectToLayer_ map
    for (auto it = layoutObjectToLayer_.begin(); it != layoutObjectToLayer_.end();) {
      if (it->second == layerId) {
        it = layoutObjectToLayer_.erase(it);
      } else {
        ++it;
      }
    }
    
    // Remove from layers_ map
    layers_.erase(layerId);
    
    cout << "Removed scroll layer " << layerId << endl;
  }

  std::vector<std::shared_ptr<ScrollLayer>> LayerManager::getAllLayersSorted() const
  {
    std::vector<std::shared_ptr<ScrollLayer>> sortedLayers;
    sortedLayers.reserve(layers_.size());
    
    for (const auto& pair : layers_) {
      sortedLayers.push_back(pair.second);
    }
    
    // Sort by depth (lower depth rendered first, higher depth on top)
    std::sort(sortedLayers.begin(), sortedLayers.end(),
              [](const std::shared_ptr<ScrollLayer>& a, const std::shared_ptr<ScrollLayer>& b) {
                return a->getDepth() < b->getDepth();
              });
    
    return sortedLayers;
  }

  void LayerManager::associateLayerWithLayoutObject(uint32_t layerId, std::shared_ptr<client_layout::LayoutObject> layoutObject)
  {
    if (layoutObject && layers_.find(layerId) != layers_.end()) {
      layoutObjectToLayer_[layoutObject] = layerId;
    }
  }

  std::shared_ptr<ScrollLayer> LayerManager::getLayerForLayoutObject(std::shared_ptr<client_layout::LayoutObject> layoutObject) const
  {
    auto it = layoutObjectToLayer_.find(layoutObject);
    if (it != layoutObjectToLayer_.end()) {
      return getLayer(it->second);
    }
    return nullptr;
  }

  bool LayerManager::shouldCreateScrollLayer(std::shared_ptr<client_layout::LayoutObject> layoutObject) const
  {
    if (!layoutObject) {
      return false;
    }

    // Check if this layout object has scrollable overflow
    if (!hasScrollableOverflow(layoutObject)) {
      return false;
    }

    // Check if we already have a layer for this object
    if (getLayerForLayoutObject(layoutObject) != nullptr) {
      return false;
    }

    return true;
  }

  void LayerManager::updateLayersAfterLayout()
  {
    for (auto& pair : layers_) {
      auto layer = pair.second;
      if (layer->isDirty()) {
        // Layer updates will be handled by the rendering system
        cout << "Layer " << layer->getId() << " marked for update" << endl;
      }
    }
  }

  void LayerManager::cleanupUnusedLayers()
  {
    std::vector<uint32_t> layersToRemove;
    
    for (const auto& pair : layers_) {
      auto layer = pair.second;
      
      // Check if layer has associated layout objects
      bool hasAssociatedObjects = false;
      for (const auto& objPair : layoutObjectToLayer_) {
        if (objPair.second == layer->getId()) {
          hasAssociatedObjects = true;
          break;
        }
      }
      
      if (!hasAssociatedObjects) {
        layersToRemove.push_back(layer->getId());
      }
    }
    
    for (uint32_t layerId : layersToRemove) {
      removeLayer(layerId);
    }
  }

  bool LayerManager::hasScrollableOverflow(std::shared_ptr<client_layout::LayoutObject> layoutObject) const
  {
    // Check if the layout object has CSS overflow: scroll or auto
    // This is a simplified check - in a full implementation, we'd need to access
    // the computed style and check overflow properties
    
    // For now, we'll assume any box model object with content overflow could be scrollable
    // In the actual implementation, this should check:
    // - overflow-x: scroll or auto
    // - overflow-y: scroll or auto
    // - content size > container size
    
    return layoutObject->isBoxModelObject();
  }

  int LayerManager::calculateLayerDepth(std::shared_ptr<client_layout::LayoutObject> layoutObject) const
  {
    // Calculate depth based on DOM tree depth or z-index
    // For now, use a simple depth calculation
    int depth = 0;
    auto current = layoutObject;
    while (current && current->parent()) {
      depth++;
      current = current->parent();
    }
    return depth;
  }
}