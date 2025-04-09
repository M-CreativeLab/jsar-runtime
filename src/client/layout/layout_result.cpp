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
    return fragment_.width() != lastFragment.width() ||
           fragment_.height() != lastFragment.height();
  }

  bool LayoutResult::needsRelayout(const Fragment &lastFragment) const
  {
    // TODO(yorkie): implement this method.
    return false;
  }
}
