#pragma once

#include <iostream>
#include <concepts>
#include "./holocron_layout.autogen.hpp"

namespace crates::layout2
{
  using namespace ::rust;

  template <typename T>
  class Point
  {
  public:
    Point(T x, T y) : x_(x), y_(y) {}

  public:
    T &x() { return x_; }
    T x() const { return x_; }
    T &y() { return y_; }
    T y() const { return y_; }

  private:
    T x_;
    T y_;
  };

  /**
   * A rectangle with top, right, bottom, and left values.
   *
   * @tparam T The type of the rectangle values.
   */
  template <typename T>
  class Rect
  {
  public:
    Rect(T top, T right, T bottom, T left)
        : top_(top), right_(right), bottom_(bottom), left_(left) {}

  public:
    T &top() { return top_; }
    T top() const { return top_; }
    T &right() { return right_; }
    T right() const { return right_; }
    T &bottom() { return bottom_; }
    T bottom() const { return bottom_; }
    T &left() { return left_; }
    T left() const { return left_; }

  public:
    friend std::ostream &operator<<(std::ostream &os, const Rect<T> &style)
    {
      os << "(" << style.top_ << ", " << style.right_ << ", " << style.bottom_ << ", " << style.left_ << ")";
      return os;
    }

  private:
    T top_;
    T right_;
    T bottom_;
    T left_;
  };

  inline Rect<float> NumberRect(holocron::layout::NumberRect rect)
  {
    return Rect<float>(rect.top, rect.right, rect.bottom, rect.left);
  }

  /**
   * A template class represents CSS keyword.
   *
   * @tparam T The type of the CSS keyword.
   * @tparam defaultValue The default value of the CSS keyword.
   */
  template <typename T, T defaultValue>
    requires std::is_enum_v<T>
  class CSSKeyword
  {
    friend class LayoutStyle;

  public:
    CSSKeyword(T handle = defaultValue) : handle_(handle)
    {
    }
    ~CSSKeyword() = default;

  public:
    operator T() const { return handle_; }

  protected:
    virtual std::optional<T> parse(const std::string &input) = 0;
    virtual std::string stringify() { return ""; }

  protected:
    T handle_;
  };

  namespace styles
  {
    using OverflowXY = holocron::layout::OverflowXY;

#define DISPLAY_MAP(XX) \
  XX(Block, "block")    \
  XX(Flex, "flex")      \
  XX(Grid, "grid")      \
  XX(None, "none")

#define BOX_SIZING_MAP(XX)      \
  XX(ContentBox, "content-box") \
  XX(BorderBox, "border-box")

#define OVERFLOW_MAP(XX) \
  XX(Visible, "visible") \
  XX(Hidden, "hidden")   \
  XX(Scroll, "scroll")   \
  XX(Clip, "clip")

#define POSITION_MAP(XX)   \
  XX(Relative, "relative") \
  XX(Absolute, "absolute")

#define BOX_ALIGNMENT_MAP(XX) \
  XX(Start, "start")          \
  XX(End, "end")              \
  XX(FlexStart, "flex-start") \
  XX(FlexEnd, "flex-end")     \
  XX(Center, "center")        \
  XX(Baseline, "baseline")    \
  XX(Stretch, "stretch")

#define CONTENT_SPACING_MAP(XX)     \
  XX(Start, "start")                \
  XX(End, "end")                    \
  XX(FlexStart, "flex-start")       \
  XX(FlexEnd, "flex-end")           \
  XX(Center, "center")              \
  XX(Stretch, "stretch")            \
  XX(SpaceBetween, "space-between") \
  XX(SpaceEvenly, "space-evenly")   \
  XX(SpaceAround, "space-around")

#define FLEX_DIRECTION_MAP(XX)  \
  XX(Row, "row")                \
  XX(Column, "column")          \
  XX(RowReverse, "row-reverse") \
  XX(ColumnReverse, "column-reverse")

#define FLEX_WRAP_MAP(XX) \
  XX(NoWrap, "nowrap")    \
  XX(Wrap, "wrap")        \
  XX(WrapReverse, "wrap-reverse")

    class Display : public CSSKeyword<holocron::layout::Display,
                                      holocron::layout::Display::Block>
    {
      using CSSKeyword::CSSKeyword;

    public:
      static Display Block() { return Display(holocron::layout::Display::Block); }
      static Display Flex() { return Display(holocron::layout::Display::Flex); }
      static Display Grid() { return Display(holocron::layout::Display::Grid); }
      static Display None() { return Display(holocron::layout::Display::None); }

    public:
      Display(const std::string &input)
      {
        handle_ = parse(input).value_or(holocron::layout::Display::Block);
      }

    private:
      std::optional<holocron::layout::Display> parse(const std::string &input) override
      {
#define XX(tag, str) \
  if (input == str)  \
    return holocron::layout::Display::tag;
        DISPLAY_MAP(XX)
#undef XX
        return std::nullopt;
      }

    public:
      friend std::ostream &operator<<(std::ostream &os, const Display &value)
      {
        switch (value.handle_)
        {
#define XX(tag, str)                   \
  case holocron::layout::Display::tag: \
    os << str;                         \
    break;
          DISPLAY_MAP(XX)
#undef XX
        }
        return os;
      }
    };

    class BoxSizing : public CSSKeyword<holocron::layout::BoxSizing,
                                        holocron::layout::BoxSizing::ContentBox>
    {
      using CSSKeyword::CSSKeyword;

    public:
      static BoxSizing ContentBox() { return BoxSizing(holocron::layout::BoxSizing::ContentBox); }
      static BoxSizing BorderBox() { return BoxSizing(holocron::layout::BoxSizing::BorderBox); }

    public:
      BoxSizing(const std::string &input)
      {
        handle_ = parse(input).value_or(holocron::layout::BoxSizing::ContentBox);
      }

    private:
      std::optional<holocron::layout::BoxSizing> parse(const std::string &input) override
      {
#define XX(tag, str) \
  if (input == str)  \
    return holocron::layout::BoxSizing::tag;
        BOX_SIZING_MAP(XX)
#undef XX
        return std::nullopt;
      }

    public:
      friend std::ostream &operator<<(std::ostream &os, const BoxSizing &value)
      {
        switch (value.handle_)
        {
#define XX(tag, str)                     \
  case holocron::layout::BoxSizing::tag: \
    os << str;                           \
    break;
          BOX_SIZING_MAP(XX)
#undef XX
        }
        return os;
      }
    };

    class Overflow : public CSSKeyword<holocron::layout::Overflow,
                                       holocron::layout::Overflow::Visible>
    {
      using CSSKeyword::CSSKeyword;

    public:
      static Overflow Visible() { return Overflow(holocron::layout::Overflow::Visible); }
      static Overflow Hidden() { return Overflow(holocron::layout::Overflow::Hidden); }
      static Overflow Scroll() { return Overflow(holocron::layout::Overflow::Scroll); }
      static Overflow Clip() { return Overflow(holocron::layout::Overflow::Clip); }

    public:
      Overflow(const std::string &input)
      {
        handle_ = parse(input).value_or(holocron::layout::Overflow::Visible);
      }

    private:
      std::optional<holocron::layout::Overflow> parse(const std::string &input) override
      {
#define XX(tag, str) \
  if (input == str)  \
    return holocron::layout::Overflow::tag;
        OVERFLOW_MAP(XX)
#undef XX
        return std::nullopt;
      }

    public:
      friend std::ostream &operator<<(std::ostream &os, const Overflow &value)
      {
        switch (value.handle_)
        {
#define XX(tag, str)                    \
  case holocron::layout::Overflow::tag: \
    os << str;                          \
    break;
          OVERFLOW_MAP(XX)
#undef XX
        }
        return os;
      }
    };

    class Position : public CSSKeyword<holocron::layout::Position,
                                       holocron::layout::Position::Relative>
    {
      using CSSKeyword::CSSKeyword;

    public:
      static Position Relative() { return Position(holocron::layout::Position::Relative); }
      static Position Absolute() { return Position(holocron::layout::Position::Absolute); }

    public:
      Position(const std::string &input)
      {
        handle_ = parse(input).value_or(holocron::layout::Position::Relative);
      }

    private:
      std::optional<holocron::layout::Position> parse(const std::string &input) override
      {
#define XX(tag, str) \
  if (input == str)  \
    return holocron::layout::Position::tag;
        POSITION_MAP(XX)
#undef XX
        return std::nullopt;
      }

    public:
      friend std::ostream &operator<<(std::ostream &os, const Position &value)
      {
        switch (value.handle_)
        {
#define XX(tag, str)                    \
  case holocron::layout::Position::tag: \
    os << str;                          \
    break;
          POSITION_MAP(XX)
#undef XX
        }
        return os;
      }
    };

    class FlexDirection : public CSSKeyword<holocron::layout::FlexDirection,
                                            holocron::layout::FlexDirection::Row>
    {
      using CSSKeyword::CSSKeyword;

    public:
      static FlexDirection Row() { return FlexDirection(holocron::layout::FlexDirection::Row); }
      static FlexDirection Column() { return FlexDirection(holocron::layout::FlexDirection::Column); }
      static FlexDirection RowReverse() { return FlexDirection(holocron::layout::FlexDirection::RowReverse); }
      static FlexDirection ColumnReverse() { return FlexDirection(holocron::layout::FlexDirection::ColumnReverse); }

    public:
      FlexDirection(const std::string &input)
      {
        handle_ = parse(input).value_or(holocron::layout::FlexDirection::Row);
      }

    private:
      std::optional<holocron::layout::FlexDirection> parse(const std::string &input) override
      {
#define XX(tag, str) \
  if (input == str)  \
    return holocron::layout::FlexDirection::tag;
        FLEX_DIRECTION_MAP(XX)
#undef XX
        return std::nullopt;
      }

    public:
      friend std::ostream &operator<<(std::ostream &os, const FlexDirection &value)
      {
        switch (value.handle_)
        {
#define XX(tag, str)                         \
  case holocron::layout::FlexDirection::tag: \
    os << str;                               \
    break;
          FLEX_DIRECTION_MAP(XX)
#undef XX
        }
        return os;
      }
    };

    class FlexWrap : public CSSKeyword<holocron::layout::FlexWrap,
                                       holocron::layout::FlexWrap::NoWrap>
    {
      using CSSKeyword::CSSKeyword;

    public:
      static FlexWrap NoWrap() { return FlexWrap(holocron::layout::FlexWrap::NoWrap); }
      static FlexWrap Wrap() { return FlexWrap(holocron::layout::FlexWrap::Wrap); }
      static FlexWrap WrapReverse() { return FlexWrap(holocron::layout::FlexWrap::WrapReverse); }

    public:
      FlexWrap(const std::string &input)
      {
        handle_ = parse(input).value_or(holocron::layout::FlexWrap::NoWrap);
      }

    private:
      std::optional<holocron::layout::FlexWrap> parse(const std::string &input) override
      {
#define XX(tag, str) \
  if (input == str)  \
    return holocron::layout::FlexWrap::tag;
        FLEX_WRAP_MAP(XX)
#undef XX
        return std::nullopt;
      }

    public:
      friend std::ostream &operator<<(std::ostream &os, const FlexWrap &value)
      {
        switch (value.handle_)
        {
#define XX(tag, str)                    \
  case holocron::layout::FlexWrap::tag: \
    os << str;                          \
    break;
          FLEX_WRAP_MAP(XX)
#undef XX
        }
        return os;
      }
    };

    template <typename T, typename I, I defaultValue = I::Stretch>
    class BoxAlignmentProperty : public CSSKeyword<I, defaultValue>
    {
      using CSSKeyword<I, defaultValue>::CSSKeyword;

    public:
#define XX(tag, _) \
  static T tag() { return T(I::tag); }
      BOX_ALIGNMENT_MAP(XX)
#undef XX

    public:
      BoxAlignmentProperty(const std::string &input)
      {
        this->handle_ = parse(input).value_or(defaultValue);
      }

    protected:
      std::optional<I> parse(const std::string &input) override
      {
#define XX(tag, str) \
  if (input == str)  \
    return I::tag;
        BOX_ALIGNMENT_MAP(XX)
#undef XX
        return std::nullopt;
      }
      std::string stringify() override
      {
        switch (this->handle_)
        {
#define XX(tag, str) \
  case I::tag:       \
    return str;
          BOX_ALIGNMENT_MAP(XX)
#undef XX
        default:
          return "";
        }
      }

    public:
      friend std::ostream &operator<<(std::ostream &os, const T &value)
      {
        return os << value.stringify();
      }
    };

    class AlignItems : public BoxAlignmentProperty<AlignItems, holocron::layout::AlignItems>
    {
      using BoxAlignmentProperty::BoxAlignmentProperty;
    };

    class AlignSelf : public BoxAlignmentProperty<AlignSelf, holocron::layout::AlignSelf, holocron::layout::AlignSelf::Auto>
    {
      using BoxAlignmentProperty::BoxAlignmentProperty;

    public:
      static AlignSelf Auto() { return AlignSelf(holocron::layout::AlignSelf::Auto); }

    protected:
      std::optional<holocron::layout::AlignSelf> parse(const std::string &input) override
      {
        if (input == "auto")
          return holocron::layout::AlignSelf::Auto;
        return BoxAlignmentProperty::parse(input);
      }
      std::string stringify() override
      {
        if (handle_ == holocron::layout::AlignSelf::Auto)
          return "auto";
        return BoxAlignmentProperty::stringify();
      }
    };

    class JustifyItems : public BoxAlignmentProperty<JustifyItems, holocron::layout::JustifyItems>
    {
      using BoxAlignmentProperty::BoxAlignmentProperty;
    };

    class JustifySelf : public BoxAlignmentProperty<JustifySelf, holocron::layout::JustifySelf>
    {
      using BoxAlignmentProperty::BoxAlignmentProperty;
    };

    template <typename T, typename I>
    class ContentSpacingProperty : public CSSKeyword<I, I::Stretch>
    {
      using CSSKeyword<I, I::Stretch>::CSSKeyword;

    public:
#define XX(tag, _) \
  static T tag() { return T(I::tag); }
      CONTENT_SPACING_MAP(XX)
#undef XX

    public:
      ContentSpacingProperty(const std::string &input)
      {
        this->handle_ = parse(input).value_or(I::Stretch);
      }

    private:
      std::optional<I> parse(const std::string &input) override
      {
#define XX(tag, str) \
  if (input == str)  \
    return I::tag;
        CONTENT_SPACING_MAP(XX)
#undef XX
        return std::nullopt;
      }

    public:
      friend std::ostream &operator<<(std::ostream &os, const T &value)
      {
        switch (value.handle_)
        {
#define XX(tag, str) \
  case I::tag:       \
    os << str;       \
    break;
          CONTENT_SPACING_MAP(XX)
#undef XX
        }
        return os;
      }
    };

    class AlignContent : public ContentSpacingProperty<AlignContent, holocron::layout::AlignContent>
    {
      using ContentSpacingProperty::ContentSpacingProperty;
    };

    class JustifyContent : public ContentSpacingProperty<JustifyContent, holocron::layout::JustifyContent>
    {
      using ContentSpacingProperty::ContentSpacingProperty;
    };

    class LengthPercentage
    {
    public:
      inline static LengthPercentage Length(float value)
      {
        auto inner = holocron::layout::LengthPercentage{
            .tag = holocron::layout::LengthPercentageTag::Length,
            .length = {value}};
        return LengthPercentage(inner);
      }
      inline static LengthPercentage Percentage(float percent)
      {
        auto inner = holocron::layout::LengthPercentage{
            .tag = holocron::layout::LengthPercentageTag::Percentage,
            .percentage = {percent}};
        return LengthPercentage(inner);
      }

    public:
      LengthPercentage(const std::string &input = "")
          : inner_(holocron::layout::LengthPercentage{
                .tag = holocron::layout::LengthPercentageTag::Length,
                .length = {0.0f}})
      {
        if (input.ends_with("%"))
        {
          auto value = std::stof(input.substr(0, input.size() - 1)) / 100.0f;
          inner_ = holocron::layout::LengthPercentage{
              .tag = holocron::layout::LengthPercentageTag::Percentage,
              .percentage = {value}};
        }
        else if (input.ends_with("px"))
        {
          auto value = std::stof(input.substr(0, input.size() - 2));
          inner_ = holocron::layout::LengthPercentage{
              .tag = holocron::layout::LengthPercentageTag::Length,
              .length = {value}};
        }
      }
      LengthPercentage(holocron::layout::LengthPercentage lengthPercentage)
          : inner_(lengthPercentage)
      {
      }

    public:
      inline bool isLength() const { return inner_.tag == holocron::layout::LengthPercentageTag::Length; }
      inline bool isPercent() const { return inner_.tag == holocron::layout::LengthPercentageTag::Percentage; }

    public:
      friend std::ostream &operator<<(std::ostream &os, const LengthPercentage &value)
      {
        os << static_cast<std::string>(value);
        return os;
      }
      operator holocron::layout::LengthPercentage() const { return inner_; }
      operator std::string() const
      {
        if (isLength())
          return std::to_string(inner_.length.value) + "px";
        else if (isPercent())
          return std::to_string(inner_.percentage.percent * 100.0f) + "%";
        else
          throw std::runtime_error("Unknown LengthPercentage value");
      }

    private:
      holocron::layout::LengthPercentage inner_;
    };

    template <typename T, typename I, typename S>
    class LengthPercentageAutoLike
    {
      friend class LayoutStyle;

    public:
      /**
       * A length dimension.
       *
       * @param value The length value.
       * @returns The length dimension.
       */
      inline static T Length(float value) { return T(I{.tag = S::Length, .length = {value}}); }
      /**
       * A percent dimension.
       *
       * @param value The percent value between 0 and 1.
       * @returns The percent dimension.
       */
      inline static T Percentage(float percent) { return T(I{.tag = S::Percentage, .percentage = {percent}}); }
      /**
       * An auto dimension without value.
       *
       * @returns The auto dimension.
       */
      inline static T Auto() { return T(); }

    public:
      LengthPercentageAutoLike(const std::string &input = "")
      {
        if (input.ends_with("%"))
        {
          auto value = std::stof(input.substr(0, input.size() - 1)) / 100.0f;
          inner_ = I{.tag = S::Percentage, .percentage = {value}};
        }
        else if (input.ends_with("px"))
        {
          auto value = std::stof(input.substr(0, input.size() - 2));
          inner_ = I{.tag = S::Length, .length = {value}};
        }
        else
        {
          if (input != "" && input != "auto") // Print error message if the input is not empty or "auto".
            std::cerr << "Switched to \"auto\", unknown Dimension or LengthPercentageAuto value: " << input << std::endl;
          inner_ = I{.tag = S::Auto};
        }
      }
      LengthPercentageAutoLike(I inner)
          : inner_(inner)
      {
      }

    public:
      inline bool isLength() const { return inner_.tag == S::Length; }
      inline bool isPercent() const { return inner_.tag == S::Percentage; }
      inline bool isAuto() const { return inner_.tag == S::Auto; }
      inline float value() const
      {
        if (isLength())
          return inner_.length.value;
        else if (isPercent())
          return inner_.percentage.percent;
        else
          return 0.0f;
      }

    public:
      friend std::ostream &operator<<(std::ostream &os, const T &value)
      {
        os << static_cast<std::string>(value);
        return os;
      }
      operator I() const { return inner_; }
      operator std::string() const
      {
        if (isLength())
          return std::to_string(inner_.length.value) + "px";
        else if (isPercent())
          return std::to_string(inner_.percentage.percent * 100.0f) + "%";
        else
          return "auto";
      }
      T operator/(float value) const
      {
        if (isLength())
          return Length(inner_.length.value / value);
        else if (isPercent())
          return Percent(inner_.percentage.percent / value);
        else
          return Auto();
      }
      T operator*(float value) const
      {
        if (isLength())
          return Length(inner_.length.value * value);
        else if (isPercent())
          return Percent(inner_.percentage.percent * value);
        else
          return Auto();
      }

    private:
      I inner_;
    };

    class LengthPercentageAuto
        : public LengthPercentageAutoLike<LengthPercentageAuto,
                                          holocron::layout::LengthPercentageAuto,
                                          holocron::layout::LengthPercentageAutoTag>
    {
      using LengthPercentageAutoLike::LengthPercentageAutoLike;

    public:
      inline static LengthPercentageAuto Length(float value) { return LengthPercentageAutoLike::Length(value); }
      inline static LengthPercentageAuto Percentage(float percent) { return LengthPercentageAutoLike::Percentage(percent); }
      inline static LengthPercentageAuto Auto() { return LengthPercentageAutoLike::Auto(); }
    };

    class Dimension
        : public LengthPercentageAutoLike<Dimension,
                                          holocron::layout::Dimension,
                                          holocron::layout::DimensionTag>
    {
      using LengthPercentageAutoLike::LengthPercentageAutoLike;

    public:
      inline static Dimension Length(float value) { return LengthPercentageAutoLike::Length(value); }
      inline static Dimension Percentage(float percent) { return LengthPercentageAutoLike::Percentage(percent); }
      inline static Dimension Auto() { return LengthPercentageAutoLike::Auto(); }
    };

#undef DISPLAY_MAP
#undef BOX_SIZING_MAP
#undef OVERFLOW_MAP
#undef POSITION_MAP
#undef BOX_ALIGNMENT_MAP
#undef CONTENT_SPACING_MAP
#undef FLEX_DIRECTION_MAP
#undef FLEX_WRAP_MAP
  }

  class LayoutStyle
  {
  public:
    LayoutStyle()
        : style_({// display
                  styles::Display::Block(),
                  // box sizing
                  styles::BoxSizing::ContentBox(),
                  // overflow
                  holocron::layout::OverflowXY{
                      .x = holocron::layout::Overflow::Visible,
                      .y = holocron::layout::Overflow::Visible,
                  },
                  // scrollbar_width
                  2.0f,
                  // position
                  styles::Position::Relative(),
                  // width
                  styles::Dimension::Auto(),
                  // height
                  styles::Dimension::Auto(),
                  // width range
                  holocron::layout::DimensionRange{
                      .min = styles::Dimension::Auto(),
                      .max = styles::Dimension::Auto(),
                  },
                  // height range
                  holocron::layout::DimensionRange{
                      .min = styles::Dimension::Auto(),
                      .max = styles::Dimension::Auto(),
                  },
                  // margin
                  holocron::layout::LengthPercentageAutoRect{
                      .top = styles::LengthPercentageAuto::Length(0.0f),
                      .right = styles::LengthPercentageAuto::Length(0.0f),
                      .bottom = styles::LengthPercentageAuto::Length(0.0f),
                      .left = styles::LengthPercentageAuto::Length(0.0f),
                  },
                  // padding
                  holocron::layout::LengthPercentageRect{
                      .top = styles::LengthPercentage::Length(0.0f),
                      .right = styles::LengthPercentage::Length(0.0f),
                      .bottom = styles::LengthPercentage::Length(0.0f),
                      .left = styles::LengthPercentage::Length(0.0f),
                  },
                  // border
                  holocron::layout::LengthPercentageRect{
                      .top = styles::LengthPercentage::Length(0.0f),
                      .right = styles::LengthPercentage::Length(0.0f),
                      .bottom = styles::LengthPercentage::Length(0.0f),
                      .left = styles::LengthPercentage::Length(0.0f),
                  },
                  // alignment and spacing
                  styles::AlignItems::Stretch(),
                  styles::AlignSelf::Auto(),
                  styles::AlignContent::Stretch(),
                  styles::JustifyItems::Stretch(),
                  styles::JustifySelf::Stretch(),
                  styles::JustifyContent::FlexStart(),
                  // gap
                  holocron::layout::LengthPercentageXY{
                      .x = styles::LengthPercentage::Length(0.0f),
                      .y = styles::LengthPercentage::Length(0.0f),
                  },
                  // flex properties
                  styles::FlexDirection::Row(),
                  styles::FlexWrap::NoWrap(),
                  // flex basis
                  styles::Dimension::Auto(),
                  // flex grow
                  0.0f,
                  // flex shrink
                  1.0f})
    {
    }
    LayoutStyle(holocron::layout::Style style) : style_(style)
    {
    }

  public:
    // Display property
    const styles::Display display() const { return style_.display; }
    void setDisplay(styles::Display value) { style_.display = value; }

    // BoxSizing property
    const styles::BoxSizing boxSizing() const { return style_.boxSizing; }
    void setBoxSizing(styles::BoxSizing value) { style_.boxSizing = value; }

    // Overflow property
    const Point<styles::Overflow> overflow() const
    {
      return Point<styles::Overflow>(style_.overflow.x, style_.overflow.y);
    }
    void setOverflowX(styles::Overflow value) { style_.overflow.x = value; }
    void setOverflowY(styles::Overflow value) { style_.overflow.y = value; }

    // Scrollbar width property
    const float scrollbarWidth() const { return style_.scrollbarWidth; }
    void setScrollbarWidth(float value) { style_.scrollbarWidth = value; }

    // Position property
    const styles::Position position() const { return style_.position; }
    void setPosition(styles::Position value) { style_.position = value; }

    // Size properties
    styles::Dimension width() const { return styles::Dimension(style_.width); }
    void setWidth(const styles::Dimension value) { style_.width = value; }
    styles::Dimension height() const { return styles::Dimension(style_.height); }
    void setHeight(const styles::Dimension value) { style_.height = value; }

    // Size range properties
    styles::Dimension minWidth() const { return styles::Dimension(style_.widthRange.min); }
    void setMinWidth(const styles::Dimension value) { style_.widthRange.min = value; }
    styles::Dimension maxWidth() const { return styles::Dimension(style_.widthRange.max); }
    void setMaxWidth(const styles::Dimension value) { style_.widthRange.max = value; }
    styles::Dimension minHeight() const { return styles::Dimension(style_.heightRange.min); }
    void setMinHeight(const styles::Dimension value) { style_.heightRange.min = value; }
    styles::Dimension maxHeight() const { return styles::Dimension(style_.heightRange.max); }
    void setMaxHeight(const styles::Dimension value) { style_.heightRange.max = value; }

    // Spacing properties
    const Rect<styles::LengthPercentageAuto> margin() const
    {
      return Rect<styles::LengthPercentageAuto>(
          styles::LengthPercentageAuto(style_.margin.top),
          styles::LengthPercentageAuto(style_.margin.right),
          styles::LengthPercentageAuto(style_.margin.bottom),
          styles::LengthPercentageAuto(style_.margin.left));
    }
    void setMarginTop(styles::LengthPercentageAuto value) { style_.margin.top = value; }
    void setMarginRight(styles::LengthPercentageAuto value) { style_.margin.right = value; }
    void setMarginBottom(styles::LengthPercentageAuto value) { style_.margin.bottom = value; }
    void setMarginLeft(styles::LengthPercentageAuto value) { style_.margin.left = value; }
    const Rect<styles::LengthPercentage> padding() const
    {
      return Rect<styles::LengthPercentage>(
          styles::LengthPercentage(style_.padding.top),
          styles::LengthPercentage(style_.padding.right),
          styles::LengthPercentage(style_.padding.bottom),
          styles::LengthPercentage(style_.padding.left));
    }
    void setPaddingTop(styles::LengthPercentage value) { style_.padding.top = value; }
    void setPaddingRight(styles::LengthPercentage value) { style_.padding.right = value; }
    void setPaddingBottom(styles::LengthPercentage value) { style_.padding.bottom = value; }
    void setPaddingLeft(styles::LengthPercentage value) { style_.padding.left = value; }
    const Rect<styles::LengthPercentage> border() const
    {
      return Rect<styles::LengthPercentage>(
          styles::LengthPercentage(style_.border.top),
          styles::LengthPercentage(style_.border.right),
          styles::LengthPercentage(style_.border.bottom),
          styles::LengthPercentage(style_.border.left));
    }
    void setBorderTop(styles::LengthPercentage value) { style_.border.top = value; }
    void setBorderRight(styles::LengthPercentage value) { style_.border.right = value; }
    void setBorderBottom(styles::LengthPercentage value) { style_.border.bottom = value; }
    void setBorderLeft(styles::LengthPercentage value) { style_.border.left = value; }

    // Flex properties
    styles::AlignItems alignItems() const { return style_.alignItems; }
    void setAlignItems(styles::AlignItems value) { style_.alignItems = value; }
    styles::AlignSelf alignSelf() const { return style_.alignSelf; }
    void setAlignSelf(styles::AlignSelf value) { style_.alignSelf = value; }
    styles::AlignContent alignContent() const { return style_.alignContent; }
    void setAlignContent(styles::AlignContent value) { style_.alignContent = value; }
    styles::JustifyItems justifyItems() const { return style_.justifyItems; }
    void setJustifyItems(styles::JustifyItems value) { style_.justifyItems = value; }
    styles::JustifySelf justifySelf() const { return style_.justifySelf; }
    void setJustifySelf(styles::JustifySelf value) { style_.justifySelf = value; }
    styles::JustifyContent justifyContent() const { return style_.justifyContent; }
    void setJustifyContent(styles::JustifyContent value) { style_.justifyContent = value; }
    styles::LengthPercentage rowGap() const { return styles::LengthPercentage(style_.gap.x); }
    void setRowGap(styles::LengthPercentage value) { style_.gap.x = value; }
    styles::LengthPercentage columnGap() const { return styles::LengthPercentage(style_.gap.y); }
    void setColumnGap(styles::LengthPercentage value) { style_.gap.y = value; }
    styles::FlexDirection flexDirection() const { return style_.flexDirection; }
    void setFlexDirection(styles::FlexDirection value) { style_.flexDirection = value; }
    styles::FlexWrap flexWrap() const { return style_.flexWrap; }
    void setFlexWrap(styles::FlexWrap value) { style_.flexWrap = value; }
    styles::Dimension flexBasis() const { return styles::Dimension(style_.flexBasis); }
    void setFlexBasis(styles::Dimension value) { style_.flexBasis = value; }
    const float flexGrow() const { return style_.flexGrow; }
    void setFlexGrow(float value) { style_.flexGrow = value; }
    const float flexShrink() const { return style_.flexShrink; }
    void setFlexShrink(float value) { style_.flexShrink = value; }

  public:
    friend std::ostream &operator<<(std::ostream &os, const LayoutStyle &style)
    {
      os << "LayoutStyle {" << std::endl;
      os << "    display: " << style.display() << "," << std::endl;
      os << "  boxSizing: " << style.boxSizing() << "," << std::endl;
      os << "  overflowX: " << style.overflow().x() << "," << std::endl;
      os << "  overflowY: " << style.overflow().y() << "," << std::endl;
      os << "   position: " << style.position() << "," << std::endl;
      os << "       size: " << "(" << style.width() << ", " << style.height() << ")," << std::endl;
      os << "  [min]size: " << "(" << style.minWidth() << ", " << style.minHeight() << ")," << std::endl;
      os << "  [max]size: " << "(" << style.maxWidth() << ", " << style.maxHeight() << ")," << std::endl;
      os << "     margin: " << style.margin() << "," << std::endl;
      os << "    padding: " << style.padding() << "," << std::endl;
      os << "}";
      return os;
    }
    operator holocron::layout::Style() const { return style_; }

  private:
    holocron::layout::Style style_;
  };

  /**
   * The layout allocator to create layout nodes.
   *
   * An allocator is used to store the layout tree and nodes, please make sure it's created before any layout
   * nodes.
   */
  class Allocator
  {
    friend class Node;

  public:
    Allocator() : handle(holocron::layout::createAllocator())
    {
    }

  private:
    Box<holocron::layout::TaffyTree> handle;
  };

  class Layout
  {
    friend class Node;

  public:
    Layout()
        : width_(0.0f),
          height_(0.0f),
          x_(0.0f),
          y_(0.0f),
          border_(0.0f, 0.0f, 0.0f, 0.0f),
          padding_(0.0f, 0.0f, 0.0f, 0.0f)
    {
    }

  private:
    Layout(holocron::layout::LayoutOutput output)
        : width_(output.width),
          height_(output.height),
          x_(output.x),
          y_(output.y),
          border_(NumberRect(output.border)),
          padding_(NumberRect(output.padding))
    {
    }

  public:
    /**
     * @returns The node width.
     */
    inline float width() const { return width_; }
    /**
     * @returns The node height.
     */
    inline float height() const { return height_; }
    /**
     * @returns The node x position.
     */
    inline float left() const { return x_; }
    /**
     * @returns The node y position.
     */
    inline float top() const { return y_; }
    /**
     * @returns The node border.
     */
    inline Rect<float> border() const { return border_; }
    /**
     * @returns The node padding.
     */
    inline Rect<float> padding() const { return padding_; }

  public:
    friend std::ostream &operator<<(std::ostream &os, const Layout &layout)
    {
      os << "Layout {" << std::endl;
      os << " width: " << layout.width() << "," << std::endl;
      os << " height: " << layout.height() << "," << std::endl;
      os << " left: " << layout.left() << "," << std::endl;
      os << " top: " << layout.top() << ", " << std::endl;
      os << " border: " << layout.border() << "," << std::endl;
      os << " padding: " << layout.padding() << "}" << std::endl;
      os << std::endl;
      return os;
    }

  protected:
    float width_;
    float height_;
    float x_;
    float y_;
    Rect<float> border_;
    Rect<float> padding_;
  };

  /**
   * The layout node to compute the layout.
   */
  class Node
  {
  public:
    /**
     * Create a new layout node with the specified allocator.
     *
     * @param allocator The layout allocator to use.
     */
    Node(Allocator &allocator)
        : node_(createNode(*allocator.handle))
    {
    }
    ~Node()
    {
      holocron::layout::removeNode(*node_);
    }

  public:
    /**
     * Add a child node to this layout node.
     *
     * @param child The child node to add.
     */
    inline void addChild(Node &child)
    {
      holocron::layout::addChild(*node_, *child.node_);
    }
    /**
     * Remove a child node from this layout node.
     *
     * @param child The child node to remove.
     */
    inline void removeChild(Node &child)
    {
      holocron::layout::removeChild(*node_, *child.node_);
    }
    /**
     * Insert a child node before another child node.
     * 
     * @param child The child node to insert.
     * @param beforeChild The child node to insert before.
     */
    inline void insertChild(Node &child, Node &beforeChild)
    {
      holocron::layout::insertChild(*node_, *child.node_, *beforeChild.node_);
    }
    /**
     * Replace a child node with a new child node.
     *
     * The old child node is not going to be deleted in this method, the destructor of the old child node will be
     * guaranteed to be this.
     *
     * @param oldChild The old child node to replace.
     * @param newChild The new child node to replace with.
     * @param copyChildren Whether to copy the children of the old child node to the new child node.
     */
    inline void replaceChild(Node &oldChild, Node &newChild, bool copyChildren)
    {
      holocron::layout::replaceChild(*node_, *oldChild.node_, *newChild.node_, copyChildren);
    }
    /**
     * @returns The child count of this layout node.
     */
    inline size_t childCount() { return holocron::layout::getChildCount(*node_); }
    /**
     * @returns The layout style of this node.
     */
    inline LayoutStyle style() { return holocron::layout::getNodeStyle(*node_); }
    /**
     * Set the layout style of this node.
     *
     * @param style The layout style to set.
     */
    inline void setStyle(LayoutStyle style)
    {
      holocron::layout::setNodeStyle(*node_, style);
    }
    /**
     * Manually mark this node as dirty to recompute the layout.
     */
    inline void markDirty() { holocron::layout::markNodeDirty(*node_); }
    /**
     * Get whether this node is dirty and need to recompute the layout.
     *
     * @returns Whether this node is dirty.
     */
    inline bool isDirty() { return holocron::layout::isNodeDirty(*node_); }
    /**
     * Compute the layout of this node (and its children) with the specified parent width and height. Call this
     * method will mark all nodes as not dirty.
     *
     * @param width The parent width.
     * @param height The parent height.
     */
    inline void computeLayout(float width, float height)
    {
      holocron::layout::computeLayout(*node_, width, height);
    }
    /**
     * Print this node for debugging.
     */
    inline void debugPrint()
    {
      std::cout << "Node (children=" << childCount() << ")" << std::endl;
      holocron::layout::printNode(*node_);
    }
    /**
     * @returns The layout output of this node.
     */
    inline Layout layout()
    {
      return Layout(holocron::layout::getLayoutOutput(*node_));
    }

  private:
    Box<holocron::layout::TaffyNode> node_;
  };
}
