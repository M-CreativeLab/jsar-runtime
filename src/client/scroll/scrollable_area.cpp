#include <iostream>
#include <algorithm>
#include <common/math_utils.hpp>

#include "./scrollable_area.hpp"

namespace client_scroll
{
  using namespace std;
  using namespace transmute::common;
  using namespace client_layout;

  ScrollableArea::ScrollableArea()
      : scroll_origin_(0.0f, 0.0f, 0.0f)
      , scroll_offset_(0.0f, 0.0f, 0.0f)
      , target_scroll_offset_(0.0f, 0.0f, 0.0f)
      , is_smoothly_scrolling_(false)
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
    {
      cerr << "Skipping scrollTo() because overflow_rect_ is not set." << endl;
      return;
    }

    // Optimize scroll bounds checking with early exit and clamping
    glm::vec3 new_offset = offset;
    
    // Clamp horizontal scroll
    if (overflow_rect_->x > scroll_origin_.x)
    {
      float max_scroll_x = overflow_rect_->x - scroll_origin_.x;
      new_offset.x = std::clamp(offset.x, 0.0f, max_scroll_x);
    }
    else
    {
      new_offset.x = 0.0f;
    }
    
    // Clamp vertical scroll (note: negative values for upward scroll)
    if (overflow_rect_->y > scroll_origin_.y)
    {
      float max_scroll_y = overflow_rect_->y - scroll_origin_.y;
      new_offset.y = std::clamp(offset.y, -max_scroll_y, 0.0f);
    }
    else
    {
      new_offset.y = 0.0f;
    }
    
    // Only update if the offset actually changed
    if (new_offset != scroll_offset_)
    {
      scroll_offset_ = new_offset;
    }
  }

  void ScrollableArea::updateAfterLayout(const Fragment &fragment)
  {
    scroll_origin_ = fragment.size();
    overflow_rect_ = fragment.contentSize();
  }

  void ScrollableArea::smoothScrollTo(const glm::vec3 &offset)
  {
    if (!overflow_rect_.has_value())
    {
      cerr << "Skipping smoothScrollTo() because overflow_rect_ is not set." << endl;
      return;
    }

    // Set target and enable smooth scrolling
    target_scroll_offset_ = offset;
    is_smoothly_scrolling_ = true;
    
    // Apply immediate bounds checking to target
    if (overflow_rect_->x > scroll_origin_.x)
    {
      float max_scroll_x = overflow_rect_->x - scroll_origin_.x;
      target_scroll_offset_.x = std::clamp(offset.x, 0.0f, max_scroll_x);
    }
    else
    {
      target_scroll_offset_.x = 0.0f;
    }
    
    if (overflow_rect_->y > scroll_origin_.y)
    {
      float max_scroll_y = overflow_rect_->y - scroll_origin_.y;
      target_scroll_offset_.y = std::clamp(offset.y, -max_scroll_y, 0.0f);
    }
    else
    {
      target_scroll_offset_.y = 0.0f;
    }
  }

  void ScrollableArea::updateSmoothScroll()
  {
    if (!is_smoothly_scrolling_)
      return;
      
    // Calculate the difference between current and target
    glm::vec3 diff = target_scroll_offset_ - scroll_offset_;
    
    // Check if we're close enough to stop
    if (glm::length(diff) < 0.5f) // Stop when within 0.5 pixels
    {
      scroll_offset_ = target_scroll_offset_;
      is_smoothly_scrolling_ = false;
      return;
    }
    
    // Apply smooth interpolation
    scroll_offset_ += diff * smooth_scroll_factor_;
  }
}
