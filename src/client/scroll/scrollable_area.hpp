#pragma once

#include <optional>
#include <glm/glm.hpp>
#include <client/layout/fragment.hpp>

namespace endor
{
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
      inline bool scrollBy(const glm::vec3 &offset)
      {
        // Performance optimization: early exit if offset is zero
        if (offset.x == 0.0f && offset.y == 0.0f && offset.z == 0.0f)
          return false;
        return scrollTo(scroll_offset_ + offset);
      }
      bool scrollTo(const glm::vec3 &offset);

      // Performance optimization: check if scrolling is needed
      inline bool needsScrolling() const
      {
        return overflow_rect_.has_value() &&
               (overflow_rect_->x > scroll_origin_.x || overflow_rect_->y > scroll_origin_.y);
      }

      // Performance optimization: check if a fragment is within the visible viewport
      inline bool isFragmentInViewport(const client_layout::Fragment &fragment) const
      {
        if (!overflow_rect_.has_value())
          return true; // No scroll container, everything is visible

        // Calculate visible area bounds considering scroll offset
        float visible_left = scroll_offset_.x;
        float visible_top = scroll_offset_.y;
        float visible_right = visible_left + scroll_origin_.x;
        float visible_bottom = visible_top + scroll_origin_.y;

        // Check if fragment overlaps with visible area
        return !(fragment.right() < visible_left ||
                 fragment.left() > visible_right ||
                 fragment.bottom() < visible_top ||
                 fragment.top() > visible_bottom);
      }

      void updateAfterLayout(const client_layout::Fragment &);

    public:
      friend std::ostream &operator<<(std::ostream &, const ScrollableArea &);

    private:
      glm::vec3 scroll_origin_;
      glm::vec3 scroll_offset_;
      std::optional<glm::vec3> overflow_rect_;
    };
  }
} // namespace endor
