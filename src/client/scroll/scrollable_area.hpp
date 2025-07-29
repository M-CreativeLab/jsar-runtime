#pragma once

#include <optional>
#include <glm/glm.hpp>
#include <client/layout/fragment.hpp>

namespace client_scroll
{
  class ScrollableArea
  {
  public:
    ScrollableArea();

  public:
    glm::vec3 scrollOrigin() const;
    float scrollWidth() const;
    float scrollHeight() const;

    glm::vec3 getScrollOffset() const;
    inline void scrollBy(const glm::vec3 &offset)
    {
      // Performance optimization: early exit if offset is zero
      if (offset.x == 0.0f && offset.y == 0.0f && offset.z == 0.0f)
        return;
      scrollTo(scroll_offset_ + offset);
    }
    void scrollTo(const glm::vec3 &offset);
    
    // Performance optimization: check if scrolling is needed
    inline bool needsScrolling() const
    {
      return overflow_rect_.has_value() && 
             (overflow_rect_->x > scroll_origin_.x || overflow_rect_->y > scroll_origin_.y);
    }

    void updateAfterLayout(const client_layout::Fragment &);

  private:
    glm::vec3 scroll_origin_;
    glm::vec3 scroll_offset_;
    std::optional<glm::vec3> overflow_rect_;
  };
}
