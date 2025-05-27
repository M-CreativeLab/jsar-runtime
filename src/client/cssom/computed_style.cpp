#include <crates/bindings.hpp>

#include "./computed_style.hpp"
#include "./style_traits.hpp"
#include "./values/computed/context.hpp"
#include "./values/specified/border.hpp"
#include "./values/specified/flex.hpp"
#include "./values/specified/font.hpp"
#include "./values/specified/color.hpp"

namespace client_cssom
{
  using namespace std;

  ComputedStyle::Difference ComputedStyle::ComputeDifference(const ComputedStyle *old_style,
                                                             const ComputedStyle *new_style)
  {
    if (old_style == new_style)
      return kEqual;

    if (!old_style || !new_style)
      return kInherited;

    if (old_style->size() != new_style->size())
      return kIndependentInherited;

    // TODO(yorkie): implement the complete difference algorithm.
    return kNonInherited;
  }

  ComputedStyle::ComputedStyle(const CSSStyleDeclaration &style)
      : std::unordered_map<std::string, std::string>()
  {
    for (int index = 0; index < style.length(); index++)
    {
      auto propertyName = style.item(index);
      auto value = style.getPropertyValue(propertyName);
      setPropertyInternal(propertyName, value);
    }
  }

  ComputedStyle::operator crates::layout2::LayoutStyle() const
  {
    using namespace crates::layout2;
    LayoutStyle layoutStyle;

    layoutStyle.setDisplay(display_.toLayoutValue());
    layoutStyle.setBoxSizing(box_sizing_.toLayoutValue());
    layoutStyle.setPosition(position_type_.toLayoutValue());
    layoutStyle.setOverflowX(overflow_x_.toLayoutValue());
    layoutStyle.setOverflowY(overflow_y_.toLayoutValue());

    // Sizes
    layoutStyle.setWidth(width_.toLayoutValue());
    layoutStyle.setHeight(height_.toLayoutValue());
    layoutStyle.setMinWidth(min_width_.toLayoutValue());
    layoutStyle.setMinHeight(min_height_.toLayoutValue());
    layoutStyle.setMaxWidth(max_width_.toLayoutValue());
    layoutStyle.setMaxHeight(max_height_.toLayoutValue());

    // Margin
    layoutStyle.setMarginTop(margin_.top().toLayoutValue());
    layoutStyle.setMarginRight(margin_.right().toLayoutValue());
    layoutStyle.setMarginBottom(margin_.bottom().toLayoutValue());
    layoutStyle.setMarginLeft(margin_.left().toLayoutValue());

    // Padding
    layoutStyle.setPaddingTop(padding_.top().toLayoutValue());
    layoutStyle.setPaddingRight(padding_.right().toLayoutValue());
    layoutStyle.setPaddingBottom(padding_.bottom().toLayoutValue());
    layoutStyle.setPaddingLeft(padding_.left().toLayoutValue());

    // Border
    layoutStyle.setBorderTop(border_width_.top().toLayoutValue());
    layoutStyle.setBorderRight(border_width_.right().toLayoutValue());
    layoutStyle.setBorderBottom(border_width_.bottom().toLayoutValue());
    layoutStyle.setBorderLeft(border_width_.left().toLayoutValue());

    // Flexbox
    layoutStyle.setAlignItems(align_items_.toLayoutValue());
    layoutStyle.setAlignSelf(align_self_.toLayoutValue());
    layoutStyle.setAlignContent(align_content_.toLayoutValue());
    layoutStyle.setJustifyContent(justify_content_.toLayoutValue());
    layoutStyle.setJustifySelf(justify_self_.toLayoutValue());
    layoutStyle.setJustifyItems(justify_items_.toLayoutValue());
    layoutStyle.setRowGap(row_gap_.toLayoutValue());
    layoutStyle.setColumnGap(column_gap_.toLayoutValue());
    layoutStyle.setFlexDirection(flex_direction_.toLayoutValue());
    layoutStyle.setFlexWrap(flex_wrap_.toLayoutValue());
    layoutStyle.setFlexGrow(flex_grow_.value);
    layoutStyle.setFlexShrink(flex_shrink_.value);

    // Grid
#define LAYOUT_USE_PROPERTY_STRING(PROP, NAME) \
  if (hasProperty(PROP))                       \
    layoutStyle.set##NAME(getPropertyValue(PROP));

    LAYOUT_USE_PROPERTY_STRING("grid-template-rows", GridTemplateRows)
    LAYOUT_USE_PROPERTY_STRING("grid-template-columns", GridTemplateColumns)
    LAYOUT_USE_PROPERTY_STRING("grid-auto-rows", GridAutoRows)
    LAYOUT_USE_PROPERTY_STRING("grid-auto-columns", GridAutoColumns)
    LAYOUT_USE_PROPERTY_STRING("grid-auto-flow", GridAutoFlow)
    LAYOUT_USE_PROPERTY_STRING("grid-row-start", GridRowStart)
    LAYOUT_USE_PROPERTY_STRING("grid-row-end", GridRowEnd)
    LAYOUT_USE_PROPERTY_STRING("grid-column-start", GridColumnStart)
    LAYOUT_USE_PROPERTY_STRING("grid-column-end", GridColumnEnd)
#undef LAYOUT_USE_PROPERTY_STRING

    return layoutStyle;
  }

  void ComputedStyle::resetProperties(optional<ComputedStyle> other, values::computed::Context &context)
  {
    clear();

    if (other.has_value())
    {
      for (const auto &[propertyName, value] : other.value())
      {
        setPropertyInternal(propertyName, value);
        computeProperty(propertyName, value, context);
      }
    }
  }

  size_t ComputedStyle::inheritProperties(const ComputedStyle &other, values::computed::Context &context)
  {
    size_t inherited = 0;
    for (const auto &[propertyName, value] : other)
    {
      if (IsInheritedProperty(propertyName))
      {
        auto it = find(propertyName);
        if (it != end())
        {
          if (it->second == value)
            continue;         // No need to update if the value is the same.
          it->second = value; // Update the existing property.
        }
        else
        {
          setPropertyInternal(propertyName, value);
        }

        // Compute the property if it is inherited.
        computeProperty(propertyName, value, context);
        inherited += 1;
      }
    }
    return inherited;
  }

  bool ComputedStyle::update(values::computed::Context &context)
  {
    resetProperties(context.resetStyle(), context);
    auto inherited_style = context.inheritedStyle();
    if (inherited_style.has_value())
      inheritProperties(inherited_style.value(), context);

    // Compute shorthand properties such as `margin`, `padding`, `border`, etc.
    computeShorthandProperties(context);
    return true;
  }

  bool ComputedStyle::update(const CSSStyleDeclaration &from_style, values::computed::Context &context)
  {
    if (from_style.length() == 0)
      return false;

    for (int index = 0; index < from_style.length(); index++)
    {
      const auto propertyName = from_style.item(index);
      const auto value = from_style.getPropertyValue(propertyName);

      auto it = find(propertyName);
      if (it != end())
      {
        if (it->second == value)
          continue;         // No need to update if the value is the same.
        it->second = value; // Update the existing property.
      }
      else
      {
        setPropertyInternal(propertyName, value);
      }

      // Compute the property if it is inherited.
      computeProperty(propertyName, value, context);
    }

    // Compute shorthand properties such as `margin`, `padding`, `border`, etc.
    computeShorthandProperties(context);
    return true;
  }

  void ComputedStyle::setPropertyInternal(const std::string &name, const std::string &value)
  {
    insert({name, value});
  }

  void ComputedStyle::computeProperty(const std::string &name, const std::string &value,
                                      values::computed::Context &context)
  {
    using namespace crates::css2;

    // Box model properties
    if (name == "display")
      display_ = Parse::ParseSingleValue<values::computed::Display>(value);
    else if (name == "box-sizing")
      box_sizing_ = Parse::ParseSingleValue<values::computed::BoxSizing>(value);
    else if (name == "overflow-x")
      overflow_x_ = Parse::ParseSingleValue<values::computed::Overflow>(value);
    else if (name == "overflow-y")
      overflow_y_ = Parse::ParseSingleValue<values::computed::Overflow>(value);

    // Margin
    if (name == "margin-top")
      margin_.setTop(Parse::ParseSingleValue<values::specified::MarginSize>(value).toComputedValue(context));
    else if (name == "margin-right")
      margin_.setRight(Parse::ParseSingleValue<values::specified::MarginSize>(value).toComputedValue(context));
    else if (name == "margin-bottom")
      margin_.setBottom(Parse::ParseSingleValue<values::specified::MarginSize>(value).toComputedValue(context));
    else if (name == "margin-left")
      margin_.setLeft(Parse::ParseSingleValue<values::specified::MarginSize>(value).toComputedValue(context));

    // Padding
    if (name == "padding-top")
      padding_.setTop(Parse::ParseSingleValue<values::specified::LengthPercentage>(value).toComputedValue(context));
    else if (name == "padding-right")
      padding_.setRight(Parse::ParseSingleValue<values::specified::LengthPercentage>(value).toComputedValue(context));
    else if (name == "padding-bottom")
      padding_.setBottom(Parse::ParseSingleValue<values::specified::LengthPercentage>(value).toComputedValue(context));
    else if (name == "padding-left")
      padding_.setLeft(Parse::ParseSingleValue<values::specified::LengthPercentage>(value).toComputedValue(context));

    // Sizes
    else if (name == "width")
      width_ = Parse::ParseSingleValue<values::specified::Size>(value).toComputedValue(context);
    else if (name == "height")
      height_ = Parse::ParseSingleValue<values::specified::Size>(value).toComputedValue(context);
    else if (name == "min-width")
      min_width_ = Parse::ParseSingleValue<values::specified::Size>(value).toComputedValue(context);
    else if (name == "min-height")
      min_height_ = Parse::ParseSingleValue<values::specified::Size>(value).toComputedValue(context);

    // Font properties
    if (name == "font-family")
      fonts_ = parsing::parseFontFamily(value);
    else if (name == "font-size")
      font_size_ = Parse::ParseSingleValue<values::specified::FontSize>(value).toComputedValue(context);
    else if (name == "font-weight")
      font_weight_ = Parse::ParseSingleValue<values::specified::FontWeight>(value).toComputedValue(context);
    else if (name == "font-style")
      font_style_ = Parse::ParseSingleValue<values::specified::FontStyle>(value).toComputedValue(context);
    else if (name == "line-height")
      line_height_ = Parse::ParseSingleValue<values::specified::LineHeight>(value).toComputedValue(context);

    // Visibility properties
    // TODO: implement visibility properties

    // Text properties
    else if (name == "text-align")
      text_align_ = Parse::ParseSingleValue<values::computed::TextAlign>(value);
    else if (name == "direction")
      text_direction_ = Parse::ParseSingleValue<values::computed::Direction>(value);

    // Color properties
    else if (name == "color")
      color_ = Parse::ParseSingleValue<values::specified::Color>(value).toComputedValue(context);
    else if (name == "background-color")
      background_color_ = Parse::ParseSingleValue<values::specified::Color>(value).toComputedValue(context);

    // Flexbox
    else if (name == "flex-direction")
      flex_direction_ = Parse::ParseSingleValue<values::specified::FlexDirection>(value).toComputedValue(context);
    else if (name == "align-items")
      align_items_ = Parse::ParseSingleValue<values::specified::AlignItems>(value);
    else if (name == "align-self")
      align_self_ = Parse::ParseSingleValue<values::specified::AlignSelf>(value);
    else if (name == "align-content")
      align_content_ = Parse::ParseSingleValue<values::specified::AlignContent>(value);
    else if (name == "justify-content")
      justify_content_ = Parse::ParseSingleValue<values::specified::JustifyContent>(value);
    else if (name == "justify-self")
      justify_self_ = Parse::ParseSingleValue<values::specified::JustifySelf>(value);
    else if (name == "justify-items")
      justify_items_ = Parse::ParseSingleValue<values::specified::JustifyItems>(value);
  }

  void ComputedStyle::computeShorthandProperties(values::computed::Context &context)
  {
    // TODO: implement shorthand properties
  }
}
