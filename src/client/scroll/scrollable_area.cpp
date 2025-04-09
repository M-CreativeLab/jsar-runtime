#include "./scrollable_area.hpp"

namespace client_scroll
{
  using namespace std;

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
    return glm::vec3(0, 0, 0);
  }
}
