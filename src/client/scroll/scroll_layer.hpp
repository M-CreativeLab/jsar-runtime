#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <client/layout/fragment.hpp>
#include <client/builtin_scene/web_content.hpp>

#include "./scrollable_area.hpp"

namespace client_scroll
{
  /**
   * Represents a scrollable layer in the multi-layer scrolling system.
   * Each scrollable container gets its own layer for independent rendering.
   */
  class ScrollLayer
  {
  public:
    ScrollLayer(uint32_t layerId, const glm::vec3& position, const glm::vec2& size);
    ~ScrollLayer() = default;

    // Layer identification
    uint32_t getId() const { return layer_id_; }
    
    // Layer positioning and sizing
    const glm::vec3& getPosition() const { return position_; }
    void setPosition(const glm::vec3& position) { position_ = position; }
    
    const glm::vec2& getSize() const { return size_; }
    void setSize(const glm::vec2& size) { size_ = size; }
    
    // Layer depth for ordering
    int getDepth() const { return depth_; }
    void setDepth(int depth) { depth_ = depth; }
    
    // Scrollable area management
    ScrollableArea& getScrollableArea() { return scrollable_area_; }
    const ScrollableArea& getScrollableArea() const { return scrollable_area_; }
    
    // WebContent for this layer
    std::shared_ptr<builtin_scene::WebContent> getWebContent() const { return web_content_; }
    void setWebContent(std::shared_ptr<builtin_scene::WebContent> content) { web_content_ = content; }
    
    // Layer visibility and dirty state
    bool isVisible() const { return visible_; }
    void setVisible(bool visible) { visible_ = visible; }
    
    bool isDirty() const { return dirty_; }
    void setDirty(bool dirty) { dirty_ = dirty; }
    
    // Update layer after layout changes
    void updateAfterLayout(const client_layout::Fragment& fragment);

  private:
    uint32_t layer_id_;
    glm::vec3 position_;
    glm::vec2 size_;
    int depth_;
    bool visible_;
    bool dirty_;
    
    ScrollableArea scrollable_area_;
    std::shared_ptr<builtin_scene::WebContent> web_content_;
  };
}