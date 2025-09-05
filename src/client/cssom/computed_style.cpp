#include <sstream>
#include <vector>
#include <numeric>
#include <crates/bindings.hpp>

#include "./computed_style.hpp"
#include "./style_traits.hpp"
#include "./values/computed/context.hpp"
#include "./values/specified/animation.hpp"
#include "./values/specified/length.hpp"
#include "./values/specified/border.hpp"
#include "./values/specified/easing.hpp"
#include "./values/specified/filter.hpp"
#include "./values/specified/flex.hpp"
#include "./values/specified/font.hpp"
#include "./values/specified/image.hpp"
#include "./values/specified/color.hpp"
#include "./values/specified/position.hpp"
#include "./values/specified/time.hpp"
#include "./values/specified/transform.hpp"
#include "./values/specified/background.hpp"
#include "./parsers/css_variable_parser.hpp"
#include "./variable_reference_tracker.hpp"

namespace client_cssom
{
  using namespace std;

  ComputedStyle::Difference ComputedStyle::ComputeDifference(const ComputedStyle &old_style,
                                                             const ComputedStyle &new_style)
  {
    // Fast check for empty styles.
    if (old_style.empty() && new_style.empty())
      return kEqual;

    // Fast check for sizes, namely if the sizes of the styles are different.
    if (old_style.size() != new_style.size())
      return kNonInherited;

    // Compare each properties in the styles.
    return old_style == new_style ? kEqual : kNonInherited;
  }

  ComputedStyle ComputedStyle::Make(const CSSStyleDeclaration &style, shared_ptr<dom::Node> target_node)
  {
    return ComputedStyle(style, values::computed::Context::From(target_node));
  }

  ComputedStyle::ComputedStyle(const CSSStyleDeclaration &style, optional<values::computed::Context> context)
      : map<string, string>()
  {
    // Set up variable change callback for recomputation
    variable_tracker_.setVariableChangeCallback(
      [this](const std::string &property_name, const std::string &new_value, const values::computed::Context &context)
      {
        auto property_it = find(property_name);
        if (property_it != end())
        {
          values::computed::Context mutable_context = context;
          computeProperty(property_name, property_it->second, mutable_context);
        }
      });

    update(style, context);
  }

  ComputedStyle::operator crates::layout2::LayoutStyle() const
  {
    using namespace crates::layout2;
    LayoutStyle layoutStyle;

    layoutStyle.setDisplay(display_.toLayoutValue());
    layoutStyle.setBoxSizing(box_sizing_.toLayoutValue());
    layoutStyle.setOverflowX(overflow_x_.toLayoutValue());
    layoutStyle.setOverflowY(overflow_y_.toLayoutValue());

    // Position
    layoutStyle.setPosition(position_type_.toLayoutValue());
    layoutStyle.setTop(inset_.top().toLayoutValue());
    layoutStyle.setRight(inset_.right().toLayoutValue());
    layoutStyle.setBottom(inset_.bottom().toLayoutValue());
    layoutStyle.setLeft(inset_.left().toLayoutValue());

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
      // First pass: Set all custom properties
      unordered_map<string, string> cssProperties;
      for (const auto &[name, value] : other.value())
      {
        auto it = find(name);
        if (it != end())
        {
          if (it->second == value)
            continue;         // No need to update if the value is the same.
          it->second = value; // Update the existing property.
        }
        else
        {
          setPropertyInternal(name, value);
        }

        if (name.length() >= 2 && name.substr(0, 2) == "--")
        {
          setCustomProperty(name, value);
        }
        else
        {
          cssProperties[name] = value;
        }
      }

      // Second pass: Process regular properties
      for (const auto &[name, value] : cssProperties)
        computeProperty(name, value, context);
    }
  }

  size_t ComputedStyle::inheritProperties(const ComputedStyle &other, values::computed::Context &context)
  {
    size_t inherited = 0;

    // First pass: Inherit custom properties
    unordered_map<string, string> inheritedProperties;
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
        inherited += 1;

        if (propertyName.length() >= 2 && propertyName.substr(0, 2) == "--")
        {
          setCustomProperty(propertyName, value);
        }
        else
        {
          inheritedProperties[propertyName] = value;
        }
      }
    }

    // Second pass: Inherit regular properties
    for (const auto &[name, value] : inheritedProperties)
      computeProperty(name, value, context);
    return inherited;
  }

  bool ComputedStyle::update(values::computed::Context &context)
  {
    resetProperties(context.resetStyle(), context);
    auto inherited_style = context.inheritedStyle();
    if (inherited_style.has_value())
    {
      inheritProperties(inherited_style.value(), context);
    }

    // Compute shorthand properties such as `margin`, `padding`, `border`, etc.
    computeShorthandProperties(context);
    updateBaseComputedStyle();
    return true;
  }

  bool ComputedStyle::update(const CSSStyleDeclaration &from_style, optional<values::computed::Context> context)
  {
    if (from_style.length() == 0)
      return false;

    // Two-pass approach: first process custom properties, then regular properties
    // This ensures custom properties are available when resolving variables in regular properties

    // First pass: Process all custom properties (CSS variables)
    unordered_map<string, string> css_properties;
    for (int index = 0; index < from_style.length(); index++)
    {
      string name = from_style.item(index);
      string value = from_style.getPropertyValue(name);

      auto it = find(name);
      if (it != end())
      {
        if (it->second == value)
          continue;         // No need to update if the value is the same.
        it->second = value; // Update the existing property.
      }
      else
      {
        setPropertyInternal(name, value);
      }

      // Only process custom properties in the first pass
      if (name.length() >= 2 && name.substr(0, 2) == "--")
      {
        setCustomProperty(name, value);
      }
      else
      {
        css_properties[name] = value;
      }
    }

    // Second pass: Process all regular properties (non-custom properties)
    if (context.has_value())
    {
      for (const auto &[name, value] : css_properties)
      {
        computeProperty(name, value, context.value());
      }
      computeShorthandProperties(context.value());
    }

    // Compute shorthand properties such as `margin`, `padding`, `border`, etc.
    if (context.has_value())
      computeShorthandProperties(context.value());
    updateBaseComputedStyle();
    return true;
  }

  optional<ComputedStyle::TransitionProperty> ComputedStyle::getTransitionProperty(uint32_t index) const
  {
    if (index >= transition_properties_.size())
      return nullopt;

    const auto &property = transition_properties_[index];
    if (index < transition_durations_.size() && index < transition_delays_.size() &&
        index < transition_timing_functions_.size())
    {
      return TransitionProperty{
        property,
        transition_durations_[index],
        transition_delays_[index],
        transition_timing_functions_[index]};
    }
    return nullopt;
  }

  // CSS Variables (Custom Properties) Implementation

  void ComputedStyle::setCustomProperty(const string &name, const string &value)
  {
    custom_properties_[name] = value;
  }

  string ComputedStyle::getCustomProperty(const string &name) const
  {
    auto it = custom_properties_.find(name);
    return (it != custom_properties_.end()) ? it->second : "";
  }

  bool ComputedStyle::hasCustomProperty(const string &name) const
  {
    return custom_properties_.find(name) != custom_properties_.end();
  }

  void ComputedStyle::inheritCustomProperties(const ComputedStyle &parentStyle)
  {
    // CSS custom properties inherit by default
    for (const auto &prop : parentStyle.custom_properties_)
    {
      // Only inherit if not already set
      if (custom_properties_.find(prop.first) == custom_properties_.end())
      {
        custom_properties_[prop.first] = prop.second;
      }
    }
  }

  string ComputedStyle::resolveVariables(const string &value, const values::computed::Context &context) const
  {
    css_variable_parser::CSSVariableParser parser(value);

    // Create a variable resolver function that looks up variables in this style and inherited styles
    auto resolver = [this, &context](const string &variable_name) -> optional<string>
    {
      // First try local custom properties
      if (hasCustomProperty(variable_name))
        return getCustomProperty(variable_name);

      // Then try parent style (inheritance)
      auto inherited_style = context.inheritedStyle();
      if (inherited_style.has_value() && inherited_style->hasCustomProperty(variable_name))
        return inherited_style->getCustomProperty(variable_name);

      // Variable not found
      return nullopt;
    };

    string result = parser.resolveVariables(resolver);

    // Check if parsing was successful
    if (!parser.isValid())
    {
      // If parsing failed, return the original value
      return value;
    }

    return result;
  }

  void ComputedStyle::setPropertyInternal(const string &name, const string &value)
  {
    insert({name, value});
  }

  void ComputedStyle::computeProperty(const string &name, const string &value, values::computed::Context &context)
  {
    using namespace crates::css2;

    // Handle CSS custom properties (variables)
    if (name.length() >= 2 && name.substr(0, 2) == "--")
    {
      setCustomProperty(name, value);
      return;
    }

    // Clear existing variable dependencies for this property
    variable_tracker_.clearDependencies(name);

    // Track variable dependencies before resolving
    css_variable_parser::CSSVariableParser parser(value);
    auto variableReferences = parser.getVariableReferences();
    for (const auto &varRef : variableReferences)
    {
      variable_tracker_.trackDependency(varRef.variable_name, name);
    }

    // Resolve variables in the value for regular properties
    string resolvedValue = resolveVariables(value, context);

    // Box model properties
    if (name == "display")
    {
      display_ = Parse::ParseSingleValue<values::computed::Display>(resolvedValue);
      bitfields_.SetHasDisplay(true);
    }
    else if (name == "box-sizing")
    {
      box_sizing_ = Parse::ParseSingleValue<values::computed::BoxSizing>(resolvedValue);
      bitfields_.SetHasBoxSizing(true);
    }
    else if (name == "z-index")
    {
      z_index_ = stoi(resolvedValue);
      bitfields_.SetHasZIndex(true);
    }
    else if (name == "position")
    {
      position_type_ = Parse::ParseSingleValue<values::specified::PositionType>(resolvedValue).toComputedValue(context);
    }
    else if (name == "top")
    {
      inset_.setTop(Parse::ParseSingleValue<values::specified::InsetSize>(resolvedValue).toComputedValue(context));
    }
    else if (name == "right")
    {
      inset_.setRight(Parse::ParseSingleValue<values::specified::InsetSize>(resolvedValue).toComputedValue(context));
    }
    else if (name == "bottom")
    {
      inset_.setBottom(Parse::ParseSingleValue<values::specified::InsetSize>(resolvedValue).toComputedValue(context));
    }
    else if (name == "left")
    {
      inset_.setLeft(Parse::ParseSingleValue<values::specified::InsetSize>(resolvedValue).toComputedValue(context));
    }
    else if (name == "overflow-x")
    {
      overflow_x_ = Parse::ParseSingleValue<values::computed::Overflow>(resolvedValue);
      bitfields_.SetHasOverflowX(true);
    }
    else if (name == "overflow-y")
    {
      overflow_y_ = Parse::ParseSingleValue<values::computed::Overflow>(resolvedValue);
      bitfields_.SetHasOverflowY(true);
    }

    // Margin
    if (name == "margin-top")
      margin_.setTop(Parse::ParseSingleValue<values::specified::MarginSize>(resolvedValue).toComputedValue(context));
    else if (name == "margin-right")
      margin_.setRight(Parse::ParseSingleValue<values::specified::MarginSize>(resolvedValue).toComputedValue(context));
    else if (name == "margin-bottom")
      margin_.setBottom(Parse::ParseSingleValue<values::specified::MarginSize>(resolvedValue).toComputedValue(context));
    else if (name == "margin-left")
      margin_.setLeft(Parse::ParseSingleValue<values::specified::MarginSize>(resolvedValue).toComputedValue(context));

    // Padding
    if (name == "padding-top")
      padding_.setTop(Parse::ParseSingleValue<values::specified::LengthPercentage>(resolvedValue).toComputedValue(context));
    else if (name == "padding-right")
      padding_.setRight(Parse::ParseSingleValue<values::specified::LengthPercentage>(resolvedValue).toComputedValue(context));
    else if (name == "padding-bottom")
      padding_.setBottom(Parse::ParseSingleValue<values::specified::LengthPercentage>(resolvedValue).toComputedValue(context));
    else if (name == "padding-left")
      padding_.setLeft(Parse::ParseSingleValue<values::specified::LengthPercentage>(resolvedValue).toComputedValue(context));

    // Sizes
    else if (name == "width")
      width_ = Parse::ParseSingleValue<values::specified::Size>(resolvedValue).toComputedValue(context);
    else if (name == "height")
      height_ = Parse::ParseSingleValue<values::specified::Size>(resolvedValue).toComputedValue(context);
    else if (name == "min-width")
      min_width_ = Parse::ParseSingleValue<values::specified::Size>(resolvedValue).toComputedValue(context);
    else if (name == "min-height")
      min_height_ = Parse::ParseSingleValue<values::specified::Size>(resolvedValue).toComputedValue(context);

    /**
     * Border properties
     */
    // border-width
    else if (name == "border-top-width")
      border_width_.top() = Parse::ParseSingleValue<values::specified::BorderSideWidth>(resolvedValue)
                              .toComputedValue(context);
    else if (name == "border-right-width")
      border_width_.right() = Parse::ParseSingleValue<values::specified::BorderSideWidth>(resolvedValue)
                                .toComputedValue(context);
    else if (name == "border-bottom-width")
      border_width_.bottom() = Parse::ParseSingleValue<values::specified::BorderSideWidth>(resolvedValue)
                                 .toComputedValue(context);
    else if (name == "border-left-width")
      border_width_.left() = Parse::ParseSingleValue<values::specified::BorderSideWidth>(resolvedValue)
                               .toComputedValue(context);
    // border-style
    else if (name == "border-top-style")
      border_style_.top() = Parse::ParseSingleValue<values::specified::BorderSideStyle>(resolvedValue)
                              .toComputedValue(context);
    else if (name == "border-right-style")
      border_style_.right() = Parse::ParseSingleValue<values::specified::BorderSideStyle>(resolvedValue)
                                .toComputedValue(context);
    else if (name == "border-bottom-style")
      border_style_.bottom() = Parse::ParseSingleValue<values::specified::BorderSideStyle>(resolvedValue)
                                 .toComputedValue(context);
    else if (name == "border-left-style")
      border_style_.left() = Parse::ParseSingleValue<values::specified::BorderSideStyle>(resolvedValue)
                               .toComputedValue(context);
    // border-color
    else if (name == "border-top-color")
      border_color_.top() = Parse::ParseSingleValue<values::specified::Color>(resolvedValue)
                              .toComputedValue(context);
    else if (name == "border-right-color")
      border_color_.right() = Parse::ParseSingleValue<values::specified::Color>(resolvedValue)
                                .toComputedValue(context);
    else if (name == "border-bottom-color")
      border_color_.bottom() = Parse::ParseSingleValue<values::specified::Color>(resolvedValue)
                                 .toComputedValue(context);
    else if (name == "border-left-color")
      border_color_.left() = Parse::ParseSingleValue<values::specified::Color>(resolvedValue)
                               .toComputedValue(context);
    // border-radius
    else if (name == "border-top-left-radius")
      border_radius_.topLeft() = Parse::ParseSingleValue<values::specified::BorderCornerRadius>(resolvedValue)
                                   .toComputedValue(context);
    else if (name == "border-top-right-radius")
      border_radius_.topRight() = Parse::ParseSingleValue<values::specified::BorderCornerRadius>(resolvedValue)
                                    .toComputedValue(context);
    else if (name == "border-bottom-left-radius")
      border_radius_.bottomLeft() = Parse::ParseSingleValue<values::specified::BorderCornerRadius>(resolvedValue)
                                      .toComputedValue(context);
    else if (name == "border-bottom-right-radius")
      border_radius_.bottomRight() = Parse::ParseSingleValue<values::specified::BorderCornerRadius>(resolvedValue)
                                       .toComputedValue(context);

    // Font properties
    if (name == "font-family")
      fonts_ = parsing::parseFontFamily(resolvedValue);
    else if (name == "font-size")
      font_size_ = Parse::ParseSingleValue<values::specified::FontSize>(resolvedValue).toComputedValue(context);
    else if (name == "font-weight")
      font_weight_ = Parse::ParseSingleValue<values::specified::FontWeight>(resolvedValue).toComputedValue(context);
    else if (name == "font-style")
      font_style_ = Parse::ParseSingleValue<values::specified::FontStyle>(resolvedValue).toComputedValue(context);
    else if (name == "line-height")
      line_height_ = Parse::ParseSingleValue<values::specified::LineHeight>(resolvedValue).toComputedValue(context);

    // Visibility properties
    // TODO: implement visibility properties

    // Text properties
    else if (name == "text-align")
      text_align_ = Parse::ParseSingleValue<values::computed::TextAlign>(resolvedValue);
    else if (name == "direction")
      text_direction_ = Parse::ParseSingleValue<values::computed::Direction>(resolvedValue);

    // Color properties
    else if (name == "color")
    {
      color_ = Parse::ParseSingleValue<values::specified::Color>(resolvedValue).toComputedValue(context);
      bitfields_.SetHasColor(true);
    }
    else if (name == "background-color")
    {
      background_color_ = Parse::ParseSingleValue<values::specified::Color>(resolvedValue).toComputedValue(context);
      bitfields_.SetHasBackgroundColor(true);
    }
    else if (name == "background-image")
    {
      background_image_ = Parse::ParseSingleValue<values::specified::Image>(resolvedValue).toComputedValue(context);
      bitfields_.SetHasBackgroundImage(true);
    }
    else if (name == "background-blend-mode")
    {
      background_blend_mode_ = Parse::ParseSingleValue<values::specified::BackgroundBlendMode>(resolvedValue)
                                 .toComputedValue(context);
    }
    else if (name == "background-clip")
    {
      background_clip_ = Parse::ParseSingleValue<values::specified::BackgroundClip>(resolvedValue).toComputedValue(context);
    }
    else if (name == "background-origin")
    {
      background_origin_ = Parse::ParseSingleValue<values::specified::BackgroundOrigin>(resolvedValue).toComputedValue(context);
    }
    else if (name == "background-repeat")
    {
      background_repeat_ = Parse::ParseSingleValue<values::specified::BackgroundRepeat>(resolvedValue).toComputedValue(context);
    }
    else if (name == "background-size")
    {
      background_size_ = Parse::ParseSingleValue<values::specified::BackgroundSize>(resolvedValue).toComputedValue(context);
    }

    // Flexbox
    else if (name == "flex-direction")
      flex_direction_ = Parse::ParseSingleValue<values::specified::FlexDirection>(resolvedValue).toComputedValue(context);
    else if (name == "align-items")
      align_items_ = Parse::ParseSingleValue<values::specified::AlignItems>(resolvedValue);
    else if (name == "align-self")
      align_self_ = Parse::ParseSingleValue<values::specified::AlignSelf>(resolvedValue);
    else if (name == "align-content")
      align_content_ = Parse::ParseSingleValue<values::specified::AlignContent>(resolvedValue);
    else if (name == "justify-content")
      justify_content_ = Parse::ParseSingleValue<values::specified::JustifyContent>(resolvedValue);
    else if (name == "justify-self")
      justify_self_ = Parse::ParseSingleValue<values::specified::JustifySelf>(resolvedValue);
    else if (name == "justify-items")
      justify_items_ = Parse::ParseSingleValue<values::specified::JustifyItems>(resolvedValue);
    else if (name == "row-gap")
      row_gap_ = Parse::ParseSingleValue<values::specified::LengthPercentage>(resolvedValue).toComputedValue(context);
    else if (name == "column-gap")
      column_gap_ = Parse::ParseSingleValue<values::specified::LengthPercentage>(resolvedValue).toComputedValue(context);

    /**
     * Transform properties
     */
    else if (name == "transform")
    {
      transform_ = Parse::ParseSingleValue<values::specified::Transform>(resolvedValue).toComputedValue(context);
      bitfields_.SetHasTransform(transform_.empty() == false);
    }

    /**
     * Visual Effects properties
     */
    else if (name == "filter")
    {
      filter_ = Parse::ParseSingleValue<values::specified::Filter>(resolvedValue).toComputedValue(context);
    }
    else if (name == "backdrop-filter")
    {
      backdrop_filter_ = Parse::ParseSingleValue<values::specified::Filter>(resolvedValue).toComputedValue(context);
    }

    /**
     * Transitions and animations
     */
    else if (name == "transition-property")
    {
      transition_properties_ = Parse::ParseSingleValue<values::specified::TransitionPropertySet>(resolvedValue)
                                 .toComputedValue(context);
    }
    else if (name == "transition-duration")
    {
      transition_durations_ = Parse::ParseValuesArray<values::specified::Time>(resolvedValue)
                                .toComputedValues<values::computed::Time>(context);
    }
    else if (name == "transition-delay")
    {
      transition_delays_ = Parse::ParseValuesArray<values::specified::Time>(resolvedValue)
                             .toComputedValues<values::computed::Time>(context);
    }
    else if (name == "transition-timing-function")
    {
      transition_timing_functions_ = Parse::ParseValuesArray<values::specified::TimingFunction>(resolvedValue)
                                       .toComputedValues<values::computed::TimingFunction>(context);
    }
  }

  void ComputedStyle::computeShorthandProperties(values::computed::Context &context)
  {
    bool hasBackgroundPositionX = hasProperty("background-position-x");
    bool hasBackgroundPositionY = hasProperty("background-position-y");

    if (hasBackgroundPositionX || hasBackgroundPositionY)
    {
      string shorthandValue;
      vector<string> parts;
      if (hasBackgroundPositionX)
        parts.push_back(getPropertyValue("background-position-x"));
      if (hasBackgroundPositionY)
        parts.push_back(getPropertyValue("background-position-y"));
      shorthandValue = accumulate(parts.begin(), parts.end(), string(), [](const string &a, const string &b)
                                  { return a.empty() ? b : (a + " " + b); });

      string resolvedValue = resolveVariables(shorthandValue, context);
      background_position_ = Parse::ParseSingleValue<values::specified::BackgroundPosition>(resolvedValue)
                               .toComputedValue(context);
    }
  }

  void ComputedStyle::updateBaseComputedStyle()
  {
    // TODO: implement updateBaseComputedStyle
  }
}
