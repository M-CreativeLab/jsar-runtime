#include <glm/glm.hpp>
#include <client/dom/node.hpp>
#include <client/cssom/types/length.hpp>

#include "./layout_box_model_object.hpp"
#include "./layout_view.hpp"

namespace client_layout
{
  using namespace std;
  using namespace client_cssom::types;

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

  float LayoutBoxModelObject::getComputedLengthValue(const std::string &propertyName) const
  {
    auto nodeStyle = style();
    if (!nodeStyle.has_value() || !nodeStyle->hasProperty(propertyName))
      return 0.0f;
    else
    {
      auto length = nodeStyle->getPropertyValueAs<Length>(propertyName);
      if (length.isAbsoluteLength())
        return length.computeAbsoluteLengthInPixels();
      else if (length.isViewportBasedRelativeLength())
        return length.computeViewportBasedLengthInPixels(viewRef().viewport.xyz());
      else
      {
        assert(false && "Unsupported length type.");
      }
    }
  }

  void LayoutBoxModelObject::styleDidChange()
  {
    LayoutObject::styleDidChange();

    updateFromStyle();
  }
}
