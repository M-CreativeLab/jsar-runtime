#include <optional>
#include <client/macros.h>
#include "./css_style_declaration.hpp"

namespace client_cssom
{
  using namespace std;
  using namespace crates::layout;
  using namespace crates::layout::style;

  CSSStyleDeclaration::operator LayoutStyle() const
  {
    LayoutStyle layoutStyle;

#define LAYOUT_USE_PROPERTY_WITH_TYPE(PROP, NAME, T) \
  if (hasProperty(PROP))                             \
    layoutStyle.set##NAME(getPropertyValueAs<T>(PROP));

#define LAYOUT_USE_PROPERTY(PROP, NAME) \
  LAYOUT_USE_PROPERTY_WITH_TYPE(PROP, NAME, NAME)

    // Set display
    LAYOUT_USE_PROPERTY("display", Display)
    LAYOUT_USE_PROPERTY("box-sizing", BoxSizing)
    LAYOUT_USE_PROPERTY("position", Position)

    // Set width & height
    LAYOUT_USE_PROPERTY_WITH_TYPE("width", Width, Dimension)
    LAYOUT_USE_PROPERTY_WITH_TYPE("height", Height, Dimension)
    LAYOUT_USE_PROPERTY_WITH_TYPE("min-width", MinWidth, Dimension)
    LAYOUT_USE_PROPERTY_WITH_TYPE("min-height", MinHeight, Dimension)
    LAYOUT_USE_PROPERTY_WITH_TYPE("max-width", MaxWidth, Dimension)
    LAYOUT_USE_PROPERTY_WITH_TYPE("max-height", MaxHeight, Dimension)

    // Set overflow(x, y)
    if (hasProperty("overflow"))
    {
      auto overflow = getPropertyValueAs<Overflow>("overflow");
      layoutStyle.setOverflowX(overflow);
      layoutStyle.setOverflowY(overflow);
    }
    LAYOUT_USE_PROPERTY_WITH_TYPE("overflow-x", OverflowX, Overflow)
    LAYOUT_USE_PROPERTY_WITH_TYPE("overflow-y", OverflowY, Overflow)

    // Set margin(top, right, bottom, left)
    LAYOUT_USE_PROPERTY_WITH_TYPE("margin-top", MarginTop, LengthPercentageAuto)
    LAYOUT_USE_PROPERTY_WITH_TYPE("margin-right", MarginRight, LengthPercentageAuto)
    LAYOUT_USE_PROPERTY_WITH_TYPE("margin-bottom", MarginBottom, LengthPercentageAuto)
    LAYOUT_USE_PROPERTY_WITH_TYPE("margin-left", MarginLeft, LengthPercentageAuto)

    // Set padding(top, right, bottom, left)
    LAYOUT_USE_PROPERTY_WITH_TYPE("padding-top", PaddingTop, LengthPercentage)
    LAYOUT_USE_PROPERTY_WITH_TYPE("padding-right", PaddingRight, LengthPercentage)
    LAYOUT_USE_PROPERTY_WITH_TYPE("padding-bottom", PaddingBottom, LengthPercentage)
    LAYOUT_USE_PROPERTY_WITH_TYPE("padding-left", PaddingLeft, LengthPercentage)

    // Set border(top, right, bottom, left)
    LAYOUT_USE_PROPERTY_WITH_TYPE("border-top-width", BorderTop, LengthPercentage)
    LAYOUT_USE_PROPERTY_WITH_TYPE("border-right-width", BorderRight, LengthPercentage)
    LAYOUT_USE_PROPERTY_WITH_TYPE("border-bottom-width", BorderBottom, LengthPercentage)
    LAYOUT_USE_PROPERTY_WITH_TYPE("border-left-width", BorderLeft, LengthPercentage)

    // Set flex properties
    LAYOUT_USE_PROPERTY("align-items", AlignItems)
    LAYOUT_USE_PROPERTY("align-self", AlignSelf)
    LAYOUT_USE_PROPERTY("justify-items", JustifyItems)
    LAYOUT_USE_PROPERTY("justify-self", JustifySelf)
    LAYOUT_USE_PROPERTY("align-content", AlignContent)
    LAYOUT_USE_PROPERTY("justify-content", JustifyContent)
    LAYOUT_USE_PROPERTY_WITH_TYPE("row-gap", RowGap, LengthPercentage)
    LAYOUT_USE_PROPERTY_WITH_TYPE("column-gap", ColumnGap, LengthPercentage)
    LAYOUT_USE_PROPERTY("flex-direction", FlexDirection)
    LAYOUT_USE_PROPERTY("flex-wrap", FlexWrap)
    LAYOUT_USE_PROPERTY_WITH_TYPE("flex-grow", FlexGrow, float)
    LAYOUT_USE_PROPERTY_WITH_TYPE("flex-shrink", FlexShrink, float)

#undef LAYOUT_USE_PROPERTY
#undef LAYOUT_USE_PROPERTY_WITH_TYPE

#ifdef TR_CLIENT_CSSOM_VERBOSE
    cout << "toLayoutStyle => " << layoutStyle << endl;
    cout << "source css: " << cssText() << endl;
#endif
    // Return the layout style
    return layoutStyle;
  }

  bool CSSStyleDeclaration::equals(const CSSStyleDeclaration &other) const
  {
    if (length() != other.length()) // quick check if the length is different
      return false;
    return cssText() == other.cssText(); // TODO: support more quick check?
  }

  bool CSSStyleDeclaration::update(const CSSStyleDeclaration &other, bool omitIfPresent)
  {
    bool isChanged = false;
    for (size_t i = 0; i < other.length(); i++)
    {
      auto name = other.item(i);
      if (hasProperty(name))
      {
        bool isSelfPropImportant = getPropertyPriority(name) == CSSPropertyPriority::Important;
        bool isOtherPropImportant = other.getPropertyPriority(name) == CSSPropertyPriority::Important;
        if (isSelfPropImportant && isOtherPropImportant)
        {
          isSelfPropImportant = false;
          isOtherPropImportant = true;
        }

        // Omit this property if it is already present, or if the other one is not important
        if (omitIfPresent && !isOtherPropImportant)
          continue;

        // Skip the property if the self one is important
        if (isSelfPropImportant)
          continue;

        auto value = other.getPropertyValue(name);
        if (getPropertyValue(name) != value)
        {
          setProperty(name, value,
                      isOtherPropImportant
                          ? CSSPropertyPriority::Important
                          : CSSPropertyPriority::Normal);
          isChanged = true;
        }
      }
      else
      {
        setProperty(name, other.getPropertyValue(name), other.getPropertyPriority(name));
        isChanged = true;
      }
    }
    return isChanged;
  }
}
