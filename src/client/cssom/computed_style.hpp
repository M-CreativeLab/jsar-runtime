#pragma once

#include <map>
#include <unordered_set>
#include <string>
#include <vector>
#include <crates/bindings.hpp>
#include <client/cssom/values/generics/rect.hpp>
#include <client/cssom/values/computed/classes.hpp>
#include <client/dom/node.hpp>

#include "./css_style_declaration.hpp"

namespace client_cssom
{
  enum class Visibility
  {
    kVisible,
    kHidden,
    kCollapse,
  };

  enum class PointerEvents
  {
    kAuto,
    kNone,
  };

  class ComputedStyle : std::map<std::string, std::string>
  {
  public:
    enum Difference
    {
      kEqual,
      kNonInherited,
      kPseudoElementStyle,
      kIndependentInherited,
      kInherited,
      kDescendantAffecting,
    };
    static Difference ComputeDifference(const ComputedStyle &old_style, const ComputedStyle &new_style);
    static bool IsInheritedProperty(const std::string property)
    {
      static const std::unordered_set<std::string> inherited_properties = {
          "font-size",
          "font-weight",
          "font-style",
          "line-height",
          "color",
          "visibility",
          "text-align",
          "direction",
          "text-decoration",
          "text-transform",
          "letter-spacing",
          "word-spacing",
          "white-space",
          "text-indent"};
      return inherited_properties.find(property) != inherited_properties.end();
    }

    // Create a `ComputedStyle` from a `CSSStyleDeclaration` and a target node to create context.
    static ComputedStyle Make(const CSSStyleDeclaration &style, std::shared_ptr<dom::Node> target_node);

  public:
    ComputedStyle() = default;
    ComputedStyle(const ComputedStyle &) = default;
    ComputedStyle(const CSSStyleDeclaration &, std::optional<values::computed::Context> context);

    operator crates::layout2::LayoutStyle() const;
    friend std::ostream &operator<<(std::ostream &os, const ComputedStyle &style)
    {
      if (style.empty())
      {
        os << "ComputedStyle {}";
      }
      else
      {
        os << "ComputedStyle {" << std::endl;
        for (const auto &[key, value] : style)
          os << "  " << key << ": " << value << std::endl;
        os << "}";
      }
      return os;
    }

  public:
    inline bool hasProperty(const std::string &name) const { return find(name) != end(); }
    inline std::string getPropertyValue(const std::string &name) const
    {
      const auto &it = find(name);
      if (it != end())
        return it->second;
      return "";
    }

    void resetProperties(std::optional<ComputedStyle>, values::computed::Context &);
    size_t inheritProperties(const ComputedStyle &, values::computed::Context &);

    // Update the computed style from a context.
    bool update(values::computed::Context &);
    // Update the computed style from the given `CSSStyleDeclaration`, and a context to compute the values.
    bool update(const CSSStyleDeclaration &, std::optional<values::computed::Context>);

    // Properties
    inline const values::computed::Display &display() const { return display_; }
    inline const values::computed::BoxSizing &boxSizing() const { return box_sizing_; }
    inline const values::computed::Overflow &overflowX() const { return overflow_x_; }
    inline const values::computed::Overflow &overflowY() const { return overflow_y_; }
    inline const values::computed::Margin &margin() const { return margin_; }
    inline const values::computed::Padding &padding() const { return padding_; }

    inline const values::computed::Size &width() const { return width_; }
    inline const values::computed::Size &height() const { return height_; }
    inline const values::computed::Size &minWidth() const { return min_width_; }
    inline const values::computed::Size &minHeight() const { return min_height_; }
    inline const values::computed::MaxSize &maxWidth() const { return max_width_; }
    inline const values::computed::MaxSize &maxHeight() const { return max_height_; }

    inline const values::computed::BorderWidth &borderWidth() const { return border_width_; }
    inline const values::computed::BorderColor &borderColor() const { return border_color_; }
    inline const values::computed::BorderStyle &borderStyle() const { return border_style_; }
    inline const values::computed::BorderRadius &borderRadius() const { return border_radius_; }
    inline const values::computed::BorderCornerRadius &borderTopLeftRadius() const
    {
      return border_radius_.topLeft();
    }
    inline const values::computed::BorderCornerRadius &borderTopRightRadius() const
    {
      return border_radius_.topRight();
    }
    inline const values::computed::BorderCornerRadius &borderBottomLeftRadius() const
    {
      return border_radius_.bottomLeft();
    }
    inline const values::computed::BorderCornerRadius &borderBottomRightRadius() const
    {
      return border_radius_.bottomRight();
    }

    // Returns `true` if any of the border radius values are non-zero.
    inline const bool hasBorderRadius() const
    {
      if (!borderTopLeftRadius().isZero())
        return true;
      if (!borderTopRightRadius().isZero())
        return true;
      if (!borderBottomLeftRadius().isZero())
        return true;
      if (!borderBottomRightRadius().isZero())
        return true;
      return false;
    }

    inline const values::computed::PositionType &positionType() const { return position_type_; }

    inline Visibility visibility() const { return visibility_.value_or(Visibility::kVisible); }
    inline PointerEvents pointerEvents() const { return pointer_events_.value_or(PointerEvents::kAuto); }

    inline const std::vector<std::string> &fonts() const { return fonts_; }
    inline const values::computed::FontSize &fontSize() const { return font_size_; }
    inline const values::computed::FontWeight &fontWeight() const { return font_weight_; }
    inline const values::computed::FontStyle &fontStyle() const { return font_style_; }
    inline const values::computed::LineHeight &lineHeight() const { return line_height_; }

    inline const values::computed::TextAlign &textAlign() const { return text_align_; }
    inline const values::computed::Direction &textDirection() const { return text_direction_; }

    inline const values::computed::Color &color() const { return color_; }
    inline bool hasColor() const { return bitfields_.HasColor(); }
    inline const values::computed::Color &backgroundColor() const { return background_color_; }
    inline bool hasBackgroundColor() const { return bitfields_.HasBackgroundColor(); }

    // Visibility utility functions.
    inline bool visibleToHitTesting() const
    {
      return visibility_ == Visibility::kVisible &&
             pointer_events_ != PointerEvents::kNone;
    }

    // 3D Transforms
    inline const bool hasTransform() const { return bitfields_.HasTransform(); }
    inline const values::computed::Transform &transform() const { return transform_; }
    inline const size_t applyTransformTo(glm::mat4 &matrix) const { return transform_.applyTo(matrix); }

    // Transitions and animations
    inline const std::vector<values::computed::TransitionProperty> &transitionProperties() const
    {
      return transition_properties_;
    }
    inline const std::vector<values::computed::Time> &transitionDurations() const { return transition_durations_; }
    inline const std::vector<values::computed::Time> &transitionDelays() const { return transition_delays_; }
    inline const std::vector<values::computed::TimingFunction> &transitionTimingFunctions() const
    {
      return transition_timing_functions_;
    }

    struct TransitionProperty
    {
      values::computed::TransitionProperty property;
      values::computed::Time duration;
      values::computed::Time delay;
      values::computed::TimingFunction timing_function;
    };
    // Returns the transition property at the given index, otherwise returns `std::nullopt`.
    std::optional<TransitionProperty> getTransitionProperty(uint32_t index) const;
    inline const size_t getTransitionPropertiesCount() const { return transition_properties_.size(); }
    inline const bool hasTransitionProperties() const { return !transition_properties_.empty(); }

  private:
    void setPropertyInternal(const std::string &name, const std::string &value);
    void computeProperty(const std::string &name, const std::string &value, values::computed::Context &);
    void computeShorthandProperties(values::computed::Context &);

  private:
    // Box model
    values::computed::Display display_ = values::computed::Display::Block();
    values::computed::BoxSizing box_sizing_ = values::computed::BoxSizing::ContentBox();
    values::computed::Overflow overflow_x_ = values::computed::Overflow::Visible();
    values::computed::Overflow overflow_y_ = values::computed::Overflow::Visible();
    values::computed::Margin margin_ = values::computed::Margin::Default();
    values::computed::Padding padding_ = values::computed::Padding::Default();

    // Sizes
    values::computed::Size width_ = values::computed::Size::Auto();
    values::computed::Size height_ = values::computed::Size::Auto();
    values::computed::Size min_width_ = values::computed::Size::Auto();
    values::computed::Size min_height_ = values::computed::Size::Auto();
    values::computed::MaxSize max_width_ = values::computed::MaxSize::None();
    values::computed::MaxSize max_height_ = values::computed::MaxSize::None();

    // Border
    values::computed::BorderWidth border_width_ = values::computed::BorderWidth::Default();
    values::computed::BorderColor border_color_ = values::computed::BorderColor::Default();
    values::computed::BorderStyle border_style_ = values::computed::BorderStyle::Default();
    values::computed::BorderRadius border_radius_ = values::computed::BorderRadius::Zero();

    // Positional
    values::computed::PositionType position_type_ = values::computed::PositionType::Static();
    values::computed::Inset inset_ = values::computed::Inset::Default();

    // Alignment
    values::computed::AlignContent align_content_ = values::computed::AlignContent::Normal();
    values::computed::JustifyContent justify_content_ = values::computed::JustifyContent::Normal();
    values::computed::AlignSelf align_self_ = values::computed::AlignSelf::Auto();
    values::computed::AlignItems align_items_ = values::computed::AlignItems::Normal();
    values::computed::JustifySelf justify_self_ = values::computed::JustifySelf::Auto();
    values::computed::JustifyItems justify_items_ = values::computed::JustifyItems::Legacy();

    // Flexbox
    values::computed::LengthPercentage row_gap_;
    values::computed::LengthPercentage column_gap_;
    values::computed::FlexDirection flex_direction_;
    values::computed::FlexWrap flex_wrap_;
    values::CSSFloat flex_grow_ = 0.0f;
    values::CSSFloat flex_shrink_ = 1.0f;

    // Grid
    // TODO(yorkie): add grid properties when needed.

    // Visibility and UI
    std::optional<Visibility> visibility_ = Visibility::kVisible;
    std::optional<PointerEvents> pointer_events_ = PointerEvents::kAuto;

    // Font
    std::vector<std::string> fonts_;
    values::computed::FontSize font_size_;
    values::computed::FontWeight font_weight_;
    values::computed::FontStyle font_style_;
    values::computed::LineHeight line_height_ = values::computed::LineHeight::Normal();

    // Text
    values::computed::TextAlign text_align_;
    values::computed::Direction text_direction_;

    // Colors
    values::computed::Color color_ = values::computed::Color::Black();
    values::computed::Color background_color_ = values::computed::Color::Transparent();

    // 3D Transforms
    values::computed::Transform transform_;

    // Transitions and animations
    std::vector<values::computed::TransitionProperty> transition_properties_;
    std::vector<values::computed::Time> transition_durations_;
    std::vector<values::computed::Time> transition_delays_;
    std::vector<values::computed::TimingFunction> transition_timing_functions_;

  private: // Bitfields for computed style properties.
#define ADD_BOOLEAN_BITFIELD(PRIVATE_NAME, PUBLIC_NAME)               \
public:                                                               \
  bool PUBLIC_NAME() const { return PRIVATE_NAME; }                   \
  void Set##PUBLIC_NAME(bool new_value) { PRIVATE_NAME = new_value; } \
                                                                      \
private:                                                              \
  unsigned PRIVATE_NAME : 1

    class ComputedStyleBitfields
    {
    public:
      explicit ComputedStyleBitfields()
          : has_transform_(false)
      {
      }

      ADD_BOOLEAN_BITFIELD(has_display_, HasDisplay);
      ADD_BOOLEAN_BITFIELD(has_color_, HasColor);
      ADD_BOOLEAN_BITFIELD(has_background_color_, HasBackgroundColor);
      ADD_BOOLEAN_BITFIELD(has_box_sizing_, HasBoxSizing);
      ADD_BOOLEAN_BITFIELD(has_overflow_x_, HasOverflowX);
      ADD_BOOLEAN_BITFIELD(has_overflow_y_, HasOverflowY);
      ADD_BOOLEAN_BITFIELD(has_transform_, HasTransform);
    };
#undef ADD_BOOLEAN_BITFIELD

    ComputedStyleBitfields bitfields_;
  };
}
