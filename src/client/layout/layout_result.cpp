#include "./layout_result.hpp"

namespace client_layout
{
  using namespace std;

  LayoutResult::LayoutResult(const dom::geometry::DOMRect &rect)
      : fragment_(rect)
  {
  }

  bool LayoutResult::needsResize(const Fragment &lastFragment) const
  {
    return fragment_.contentWidth() != lastFragment.contentWidth() ||
           fragment_.contentHeight() != lastFragment.contentHeight();
  }

  bool LayoutResult::needsRelayout(const Fragment &lastFragment) const
  {
    // TODO(yorkie): implement this method.
    return false;
  }
}
