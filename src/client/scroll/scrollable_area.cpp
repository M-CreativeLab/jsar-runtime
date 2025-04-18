#include <iostream>
#include <common/math_utils.hpp>

#include "./scrollable_area.hpp"

namespace client_scroll
{
  using namespace std;
  using namespace transmute::common;
  using namespace client_layout;

  ScrollableArea::ScrollableArea()
      : scroll_origin_(0.0f, 0.0f, 0.0f),
        scroll_offset_(0.0f, 0.0f, 0.0f)
  {
  }

  glm::vec3 ScrollableArea::scrollOrigin() const
  {
    return scroll_origin_;
  }

  float ScrollableArea::scrollWidth() const
  {
    return overflow_rect_ ? overflow_rect_->x : 0;
  }

  float ScrollableArea::scrollHeight() const
  {
    return overflow_rect_ ? overflow_rect_->y : 0;
  }

  glm::vec3 ScrollableArea::getScrollOffset() const
  {
    return scroll_offset_;
  }

  void ScrollableArea::scrollTo(const glm::vec3 &offset)
  {
    if (!overflow_rect_.has_value())
      return;

    if (overflow_rect_->x > scroll_origin_.x)
    {
      float xOffset = offset.x;
      if (xOffset > 0 && scroll_origin_.x + xOffset <= overflow_rect_->x)
        scroll_offset_.x = xOffset;
    }
    if (overflow_rect_->y > scroll_origin_.y)
    {
      float yOffset = offset.y;
      if (yOffset < 0 && scroll_origin_.y - yOffset <= overflow_rect_->y)
        scroll_offset_.y = yOffset;
    }
  }

  void ScrollableArea::updateAfterLayout(const Fragment &fragment)
  {
    scroll_origin_ = fragment.size();
    overflow_rect_ = fragment.contentSize();
  }
}
