#include "./scroll_layer.hpp"

namespace client_scroll
{
  ScrollLayer::ScrollLayer(uint32_t layerId, const glm::vec3& position, const glm::vec2& size)
      : layer_id_(layerId)
      , position_(position)
      , size_(size)
      , depth_(0)
      , visible_(true)
      , dirty_(true)
      , scrollable_area_()
  {
  }

  void ScrollLayer::updateAfterLayout(const client_layout::Fragment& fragment)
  {
    // Update the scrollable area with the new fragment
    scrollable_area_.updateAfterLayout(fragment);
    
    // Update layer size from fragment
    size_ = glm::vec2(fragment.size().x, fragment.size().y);
    
    // Mark as dirty for re-rendering
    dirty_ = true;
  }
}