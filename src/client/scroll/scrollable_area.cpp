#include <iostream>
#include "./scrollable_area.hpp"

namespace client_scroll
{
  using namespace std;

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
    return 0;
  }

  float ScrollableArea::scrollHeight() const
  {
    return 0;
  }

  glm::vec3 ScrollableArea::getScrollOffset() const
  {
    return scroll_offset_;
  }

  void ScrollableArea::scrollBy(const glm::vec3 &offset)
  {
    scroll_offset_ += offset;
  }

  void ScrollableArea::scrollTo(const glm::vec3 &offset)
  {
    scroll_offset_ = offset;
  }
}
