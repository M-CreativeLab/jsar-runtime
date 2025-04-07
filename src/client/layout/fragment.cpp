#include "./fragment.hpp"

namespace client_layout
{
  const Fragment Fragment::position(const Fragment &childFragment) const
  {
    // Use child fragment which copies the size.
    dom::geometry::DOMRect newRect = childFragment.rect();

    // Set the position of the child fragment.
    newRect.x() = rect_.x() + childFragment.left();
    newRect.y() = rect_.y() + childFragment.top();

    // Returns a new fragment.
    return Fragment(newRect);
  }
}
