#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <crates/bindings.hpp>
#include <client/cssom/values/generics/rect.hpp>
#include <client/cssom/values/computed/classes.hpp>

#include "./css_style_declaration.hpp"
#include "./types/length.hpp"

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

  class ComputedStyle : std::unordered_map<std::string, std::string>
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
    static Difference ComputeDifference(const ComputedStyle *old_style, const ComputedStyle *new_style);

  public:
    ComputedStyle() = default;
    ComputedStyle(const ComputedStyle &) = default;
    ComputedStyle(const CSSStyleDeclaration &);

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
    // Update the computed style from the given `CSSStyleDeclaration`, and a context to compute the values.
    bool update(const CSSStyleDeclaration &, values::computed::Context &);

    // Properties
    const values::computed::Display &display() const { return display_; }
    const values::computed::BoxSizing &boxSizing() const { return box_sizing_; }
    const values::computed::Overflow &overflowX() const { return overflow_x_; }
    const values::computed::Overflow &overflowY() const { return overflow_y_; }
    const values::computed::Margin &margin() const { return margin_; }
    const values::computed::Padding &padding() const { return padding_; }

    const values::computed::Size &width() const { return width_; }
    const values::computed::Size &height() const { return height_; }
    const values::computed::Size &minWidth() const { return min_width_; }
    const values::computed::Size &minHeight() const { return min_height_; }
    const values::computed::MaxSize &maxWidth() const { return max_width_; }
    const values::computed::MaxSize &maxHeight() const { return max_height_; }

    const values::computed::BorderWidth &borderWidth() const { return border_width_; }
    const values::computed::BorderColor &borderColor() const { return border_color_; }
    const values::computed::BorderStyle &borderStyle() const { return border_style_; }
    const values::computed::BorderRadius &borderRadius() const { return border_radius_; }

    const values::computed::PositionType &positionType() const { return position_type_; }

    Visibility visibility() const { return visibility_.value_or(Visibility::kVisible); }
    PointerEvents pointerEvents() const { return pointer_events_.value_or(PointerEvents::kAuto); }

    const std::vector<std::string> &fonts() const { return fonts_; }
    const values::computed::FontSize &fontSize() const { return font_size_; }
    const values::computed::FontWeight &fontWeight() const { return font_weight_; }
    const values::computed::FontStyle &fontStyle() const { return font_style_; }
    const values::computed::LineHeight &lineHeight() const { return line_height_; }

    const values::computed::TextAlign &textAlign() const { return text_align_; }
    const values::computed::Direction &textDirection() const { return text_direction_; }

    const values::computed::Color &color() const { return color_; }
    const values::computed::Color &backgroundColor() const { return background_color_; }

    // Visibility utility functions.
    bool visibleToHitTesting() const
    {
      return visibility_ == Visibility::kVisible &&
             pointer_events_ != PointerEvents::kNone;
    }

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
    // TODO

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
  };
}
