#include <client/dom/node.hpp>
#include "./layout_box_model_object.hpp"

namespace client_layout
{
  using namespace std;

  LayoutBoxModelObject::LayoutBoxModelObject(shared_ptr<dom::Node> node)
      : LayoutObject(node)
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

  std::shared_ptr<client_scroll::ScrollableArea> LayoutBoxModelObject::getScrollableArea() const
  {
    return nullptr;
  }
}
