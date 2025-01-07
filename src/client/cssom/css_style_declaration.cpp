#include <optional>
#include "./css_style_declaration.hpp"

namespace client_cssom
{
  using namespace std;
  using namespace crates::layout;
  using namespace crates::layout::style;

  CSSStyleDeclaration::operator LayoutStyle() const
  {
    LayoutStyle layoutStyle;

    // Set display
    if (hasProperty("display"))
      layoutStyle.setDisplay(getPropertyValueAs<Display>("display"));

    // Set position
    if (hasProperty("position"))
      layoutStyle.setPosition(getPropertyValueAs<Position>("position"));

    // Set width
    if (hasProperty("width"))
      layoutStyle.setWidth(getPropertyValueAs<Dimension>("width"));

    // Set height
    if (hasProperty("height"))
      layoutStyle.setHeight(getPropertyValueAs<Dimension>("height"));

    // Set overflow(x, y)
    if (hasProperty("overflow"))
    {
      auto overflow = getPropertyValueAs<Overflow>("overflow");
      layoutStyle.setOverflowX(overflow);
      layoutStyle.setOverflowY(overflow);
    }
    if (hasProperty("overflow-x"))
      layoutStyle.setOverflowX(getPropertyValueAs<Overflow>("overflow-x"));
    if (hasProperty("overflow-y"))
      layoutStyle.setOverflowY(getPropertyValueAs<Overflow>("overflow-y"));

    // Set flex-grow
    if (hasProperty("flex-grow"))
      layoutStyle.setFlexGrow(getPropertyValueAs<float>("flex-grow"));

    // Set flex-shrink
    if (hasProperty("flex-shrink"))
      layoutStyle.setFlexShrink(getPropertyValueAs<float>("flex-shrink"));

    // Return the layout style
    return layoutStyle;
  }
}
