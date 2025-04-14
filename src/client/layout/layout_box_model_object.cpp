#include <client/dom/node.hpp>
#include "./layout_box_model_object.hpp"

namespace client_layout
{
  using namespace std;

  LayoutBoxModelObject::LayoutBoxModelObject(shared_ptr<dom::Node> node)
      : LayoutObject(node),
        scrollable_area_(make_shared<client_scroll::ScrollableArea>())
  {
  }

  void LayoutBoxModelObject::setDisplay(const string &displayStr)
  {
    setDisplay(DisplayType::Make(displayStr));
  }

  void LayoutBoxModelObject::setDisplay(const DisplayType &display)
  {
    display_ = display;
    setFormattingContext(display_);
  }

  shared_ptr<client_scroll::ScrollableArea> LayoutBoxModelObject::getScrollableArea() const
  {
    return scrollable_area_;
  }

  void LayoutBoxModelObject::updateFromStyle()
  {
    // TODO(yorkie): implement updateFromStyle() in LayoutBoxModelObject.
  }

  void LayoutBoxModelObject::styleDidChange()
  {
    LayoutObject::styleDidChange();

    updateFromStyle();
  }
}
