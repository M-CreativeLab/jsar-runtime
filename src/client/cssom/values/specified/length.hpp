#pragma once

#include <algorithm>
#include <variant>
#include <assert.h>
#include <crates/bindings.hpp>
#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/generics/common.hpp>
#include <client/cssom/values/specified/calc.hpp>
#include <client/cssom/values/computed/length_percentage.hpp>
#include <client/cssom/values/computed/percentage.hpp>

namespace client_cssom::values::specified
{
  // Number of pixels per inch
  constexpr float PX_PER_IN = 96.0f;
  // Number of pixels per centimeter
  constexpr float PX_PER_CM = PX_PER_IN / 2.54f;
  // Number of pixels per millimeter
  constexpr float PX_PER_MM = PX_PER_IN / 25.4f;
  // Number of pixels per quarter
  constexpr float PX_PER_Q = PX_PER_MM / 4.0f;
  // Number of pixels per point
  constexpr float PX_PER_PT = PX_PER_IN / 72.0f;
  // Number of pixels per pica
  constexpr float PX_PER_PC = PX_PER_PT * 12.0f;

  class NoCalcLength;
  class AbsoluteLength
  {
    friend class NoCalcLength;

  private:
    enum Tag
    {
      kPx,
      kIn,
      kCm,
      kMm,
      kQ,
      kPt,
      kPc,
    };

    static constexpr const char *UNIT_PX = "px";
    static constexpr const char *UNIT_IN = "in";
    static constexpr const char *UNIT_CM = "cm";
    static constexpr const char *UNIT_MM = "mm";
    static constexpr const char *UNIT_Q = "q";
    static constexpr const char *UNIT_PT = "pt";
    static constexpr const char *UNIT_PC = "pc";

  public:
    static AbsoluteLength Px(float value) { return AbsoluteLength(kPx, value); }
    static AbsoluteLength In(float value) { return AbsoluteLength(kIn, value); }
    static AbsoluteLength Cm(float value) { return AbsoluteLength(kCm, value); }
    static AbsoluteLength Mm(float value) { return AbsoluteLength(kMm, value); }
    static AbsoluteLength Q(float value) { return AbsoluteLength(kQ, value); }
    static AbsoluteLength Pt(float value) { return AbsoluteLength(kPt, value); }
    static AbsoluteLength Pc(float value) { return AbsoluteLength(kPc, value); }

  private:
    AbsoluteLength(Tag tag, float value)
        : tag_(tag),
          unitless_value_(value)
    {
    }

  public:
    inline float unitlessValue() const { return unitless_value_; }
    inline const char *unit() const
    {
      switch (tag_)
      {
      case kPx:
        return UNIT_PX;
      case kIn:
        return UNIT_IN;
      case kCm:
        return UNIT_CM;
      case kMm:
        return UNIT_MM;
      case kQ:
        return UNIT_Q;
      case kPt:
        return UNIT_PT;
      case kPc:
        return UNIT_PC;
      }
      assert(false && "Invalid tag.");
    }

    inline float toPx() const
    {
      switch (tag_)
      {
      case kPx:
        return unitless_value_;
      case kIn:
        return unitless_value_ * PX_PER_IN;
      case kCm:
        return unitless_value_ * PX_PER_CM;
      case kMm:
        return unitless_value_ * PX_PER_MM;
      case kQ:
        return unitless_value_ * PX_PER_Q;
      case kPt:
        return unitless_value_ * PX_PER_PT;
      case kPc:
        return unitless_value_ * PX_PER_PC;
      }
      assert(false && "Invalid tag.");
    }

  private:
    Tag tag_;
    float unitless_value_;
  };

  class FontRelativeLength
  {
    friend class NoCalcLength;

  private:
    enum Tag
    {
      kEm,
      kEx,
      kCh,
      kCap,
      kIc,
      kRem,
      kLh,
      kRlh,
    };

    static constexpr const char *UNIT_EM = "em";
    static constexpr const char *UNIT_EX = "ex";
    static constexpr const char *UNIT_CH = "ch";
    static constexpr const char *UNIT_CAP = "cap";
    static constexpr const char *UNIT_IC = "ic";
    static constexpr const char *UNIT_REM = "rem";
    static constexpr const char *UNIT_LH = "lh";
    static constexpr const char *UNIT_RLH = "rlh";

  public:
    static FontRelativeLength Em(float value) { return FontRelativeLength(kEm, value); }
    static FontRelativeLength Ex(float value) { return FontRelativeLength(kEx, value); }
    static FontRelativeLength Ch(float value) { return FontRelativeLength(kCh, value); }
    static FontRelativeLength Cap(float value) { return FontRelativeLength(kCap, value); }
    static FontRelativeLength Ic(float value) { return FontRelativeLength(kIc, value); }
    static FontRelativeLength Rem(float value) { return FontRelativeLength(kRem, value); }
    static FontRelativeLength Lh(float value) { return FontRelativeLength(kLh, value); }
    static FontRelativeLength Rlh(float value) { return FontRelativeLength(kRlh, value); }

  private:
    FontRelativeLength(Tag tag, float value)
        : tag_(tag),
          unitless_value_(value)
    {
    }

  public:
    inline float unitlessValue() const { return unitless_value_; }
    const char *unit() const
    {
      switch (tag_)
      {
      case kEm:
        return UNIT_EM;
      case kEx:
        return UNIT_EX;
      case kCh:
        return UNIT_CH;
      case kCap:
        return UNIT_CAP;
      case kIc:
        return UNIT_IC;
      case kRem:
        return UNIT_REM;
      case kLh:
        return UNIT_LH;
      case kRlh:
        return UNIT_RLH;
      }
      assert(false && "Invalid tag.");
    }

    float toComputedValue(computed::Context &context) const
    {
      switch (tag_)
      {
      case kEm:
      case kCap:
        return unitless_value_ * context.baseFontSize();
      case kRem:
        return unitless_value_ * context.rootFontSize();
      case kEx:
        // Assuming 'ex' is half of the font size.
        return unitless_value_ * context.baseFontSize() * 0.5f;
      case kLh:
        return unitless_value_ * context.baseLineHeight();
      case kRlh:
        return unitless_value_ * context.rootLineHeight();
      default:
        // Returns the root font-size if not supported
        return context.rootFontSize();
      }
    }

  private:
    Tag tag_;
    float unitless_value_;
  };

  class ViewportPercentageLength
  {
    friend class NoCalcLength;

  private:
    enum Tag
    {
      kVw,
      kVh,
      kVmin,
      kVmax,
      kVb,
      kVi,
    };

    static constexpr const char *UNIT_VW = "vw";
    static constexpr const char *UNIT_VH = "vh";
    static constexpr const char *UNIT_VMIN = "vmin";
    static constexpr const char *UNIT_VMAX = "vmax";
    static constexpr const char *UNIT_VB = "vb";
    static constexpr const char *UNIT_VI = "vi";

  public:
    static ViewportPercentageLength Vw(float value) { return ViewportPercentageLength(kVw, value); }
    static ViewportPercentageLength Vh(float value) { return ViewportPercentageLength(kVh, value); }
    static ViewportPercentageLength Vmin(float value) { return ViewportPercentageLength(kVmin, value); }
    static ViewportPercentageLength Vmax(float value) { return ViewportPercentageLength(kVmax, value); }
    static ViewportPercentageLength Vb(float value) { return ViewportPercentageLength(kVb, value); }
    static ViewportPercentageLength Vi(float value) { return ViewportPercentageLength(kVi, value); }

  private:
    ViewportPercentageLength(Tag tag, float value)
        : tag_(tag),
          unitless_value_(value)
    {
    }

  public:
    inline float unitlessValue() const { return unitless_value_; }
    const char *unit() const
    {
      switch (tag_)
      {
      case kVw:
        return UNIT_VW;
      case kVh:
        return UNIT_VH;
      case kVmin:
        return UNIT_VMIN;
      case kVmax:
        return UNIT_VMAX;
      case kVb:
        return UNIT_VB;
      case kVi:
        return UNIT_VI;
      }
      assert(false && "Invalid tag.");
    }
    float toComputedValue(computed::Context &context) const
    {
      glm::uvec4 base_viewport = context.baseViewport();
      switch (tag_)
      {
      case kVw:
        return unitless_value_ * base_viewport.x / 100.0f;
      case kVh:
        return unitless_value_ * base_viewport.y / 100.0f;
      case kVmin:
        return unitless_value_ * std::min(base_viewport.x, base_viewport.y) / 100.0f;
      case kVmax:
        return unitless_value_ * std::max(base_viewport.x, base_viewport.y) / 100.0f;
      default:
        return 0.0f;
      }
    }

  private:
    Tag tag_;
    float unitless_value_;
  };

  class ContainerRelativeLength
  {
    friend class NoCalcLength;

  private:
    enum Tag
    {
      kCqw,
      kCqh,
      kCqi,
      kCqb,
      kCqmin,
      kCqmax,
    };

    static constexpr const char *UNIT_CQW = "cqw";
    static constexpr const char *UNIT_CQH = "cqh";
    static constexpr const char *UNIT_CQI = "cqi";
    static constexpr const char *UNIT_CQB = "cqb";
    static constexpr const char *UNIT_CQMIN = "cqmin";
    static constexpr const char *UNIT_CQMAX = "cqmax";

  public:
    static ContainerRelativeLength Cqw(float value) { return ContainerRelativeLength(kCqw, value); }
    static ContainerRelativeLength Cqh(float value) { return ContainerRelativeLength(kCqh, value); }
    static ContainerRelativeLength Cqi(float value) { return ContainerRelativeLength(kCqi, value); }
    static ContainerRelativeLength Cqb(float value) { return ContainerRelativeLength(kCqb, value); }
    static ContainerRelativeLength Cqmin(float value) { return ContainerRelativeLength(kCqmin, value); }
    static ContainerRelativeLength Cqmax(float value) { return ContainerRelativeLength(kCqmax, value); }

  private:
    ContainerRelativeLength(Tag tag, float value)
        : tag_(tag),
          unitless_value_(value)
    {
    }

  public:
    inline float unitlessValue() const { return unitless_value_; }
    const char *unit() const
    {
      switch (tag_)
      {
      case kCqw:
        return UNIT_CQW;
      case kCqh:
        return UNIT_CQH;
      case kCqi:
        return UNIT_CQI;
      case kCqb:
        return UNIT_CQB;
      case kCqmin:
        return UNIT_CQMIN;
      case kCqmax:
        return UNIT_CQMAX;
      }
      assert(false && "Invalid tag.");
    }

  private:
    Tag tag_;
    float unitless_value_;
  };

  class NoCalcLength : public Parse,
                       public ToCss,
                       public ToComputedValue<float>
  {
    friend class Parse;

  private:
    enum Tag
    {
      kAbsolute,
      kFontRelative,
      kViewportPercentage,
      kContainerRelative,
    };

    using LengthVariant = std::variant<AbsoluteLength,
                                       FontRelativeLength,
                                       ViewportPercentageLength,
                                       ContainerRelativeLength>;

  public:
    static NoCalcLength FromPx(float value)
    {
      return NoCalcLength(AbsoluteLength::Px(value));
    }
    static bool MaybeDimension(const std::string &input)
    {
      if (input.empty())
        return false;

      // Check if the first character is a digit or a dot.
      if (!isdigit(input[0]) && input[0] != '.')
        return false;

      return input.ends_with(AbsoluteLength::UNIT_PX) ||
             input.ends_with(AbsoluteLength::UNIT_IN) ||
             input.ends_with(AbsoluteLength::UNIT_CM) ||
             input.ends_with(AbsoluteLength::UNIT_MM) ||
             input.ends_with(AbsoluteLength::UNIT_Q) ||
             input.ends_with(AbsoluteLength::UNIT_PT) ||
             input.ends_with(AbsoluteLength::UNIT_PC) ||
             input.ends_with(FontRelativeLength::UNIT_EM) ||
             input.ends_with(FontRelativeLength::UNIT_EX) ||
             input.ends_with(FontRelativeLength::UNIT_CH) ||
             input.ends_with(FontRelativeLength::UNIT_CAP) ||
             input.ends_with(FontRelativeLength::UNIT_IC) ||
             input.ends_with(FontRelativeLength::UNIT_REM) ||
             input.ends_with(FontRelativeLength::UNIT_LH) ||
             input.ends_with(FontRelativeLength::UNIT_RLH) ||
             input.ends_with(ViewportPercentageLength::UNIT_VW) ||
             input.ends_with(ViewportPercentageLength::UNIT_VH) ||
             input.ends_with(ViewportPercentageLength::UNIT_VMIN) ||
             input.ends_with(ViewportPercentageLength::UNIT_VMAX) ||
             input.ends_with(ViewportPercentageLength::UNIT_VB) ||
             input.ends_with(ViewportPercentageLength::UNIT_VI);
    }

  public:
    NoCalcLength()
        : tag_(kAbsolute),
          length_(AbsoluteLength::Px(0))
    {
    }

  private:
    NoCalcLength(AbsoluteLength absolute_length)
        : tag_(kAbsolute),
          length_(absolute_length)
    {
    }
    NoCalcLength(FontRelativeLength font_relative_length)
        : tag_(kFontRelative),
          length_(font_relative_length)
    {
    }
    NoCalcLength(ViewportPercentageLength viewport_percentage_length)
        : tag_(kViewportPercentage),
          length_(viewport_percentage_length)
    {
    }
    NoCalcLength(ContainerRelativeLength container_relative_length)
        : tag_(kContainerRelative),
          length_(container_relative_length)
    {
    }

  private:
    bool parse(const std::string &input) override
    {
      // Check if the first character is a digit or a dot.
      if (!isdigit(input[0]) && input[0] != '.')
        return false;

      size_t num_end = 0;
      bool has_decimal = false;

      while (num_end < input.size())
      {
        char c = input[num_end];
        if (isdigit(static_cast<unsigned char>(c)))
        {
          num_end++;
        }
        else if (c == '.' && !has_decimal)
        {
          has_decimal = true;
          num_end++;
        }
        else
          break;
      }

      // Failure case: no digits or only a dot.
      if (num_end == 0 || (num_end == 1 && input[0] == '.'))
        return false;

      // Parse the number.
      double num;
      std::istringstream iss(input.substr(0, num_end));
      iss >> num;

      std::string unit = input.substr(num_end);
      updateFrom(num, unit.c_str());
      return true;
    }

    void updateFrom(float value, const char *unit)
    {
      assert(unit != nullptr);

      // Absolute lengths
      if (strcmp(unit, AbsoluteLength::UNIT_PX) == 0)
      {
        tag_ = kAbsolute;
        length_ = AbsoluteLength::Px(value);
        return;
      }
      if (strcmp(unit, AbsoluteLength::UNIT_IN) == 0)
      {
        tag_ = kAbsolute;
        length_ = AbsoluteLength::In(value);
        return;
      }
      if (strcmp(unit, AbsoluteLength::UNIT_CM) == 0)
      {
        tag_ = kAbsolute;
        length_ = AbsoluteLength::Cm(value);
        return;
      }
      if (strcmp(unit, AbsoluteLength::UNIT_MM) == 0)
      {
        tag_ = kAbsolute;
        length_ = AbsoluteLength::Mm(value);
        return;
      }
      if (strcmp(unit, AbsoluteLength::UNIT_Q) == 0)
      {
        tag_ = kAbsolute;
        length_ = AbsoluteLength::Q(value);
        return;
      }
      if (strcmp(unit, AbsoluteLength::UNIT_PT) == 0)
      {
        tag_ = kAbsolute;
        length_ = AbsoluteLength::Pt(value);
        return;
      }
      if (strcmp(unit, AbsoluteLength::UNIT_PC) == 0)
      {
        tag_ = kAbsolute;
        length_ = AbsoluteLength::Pc(value);
        return;
      }

      // Font-relative lengths
      if (strcmp(unit, FontRelativeLength::UNIT_EM) == 0)
      {
        tag_ = kFontRelative;
        length_ = FontRelativeLength::Em(value);
        return;
      }
      if (strcmp(unit, FontRelativeLength::UNIT_EX) == 0)
      {
        tag_ = kFontRelative;
        length_ = FontRelativeLength::Ex(value);
        return;
      }
      if (strcmp(unit, FontRelativeLength::UNIT_CH) == 0)
      {
        tag_ = kFontRelative;
        length_ = FontRelativeLength::Ch(value);
        return;
      }
      if (strcmp(unit, FontRelativeLength::UNIT_CAP) == 0)
      {
        tag_ = kFontRelative;
        length_ = FontRelativeLength::Cap(value);
        return;
      }
      if (strcmp(unit, FontRelativeLength::UNIT_IC) == 0)
      {
        tag_ = kFontRelative;
        length_ = FontRelativeLength::Ic(value);
        return;
      }
      if (strcmp(unit, FontRelativeLength::UNIT_REM) == 0)
      {
        tag_ = kFontRelative;
        length_ = FontRelativeLength::Rem(value);
        return;
      }
      if (strcmp(unit, FontRelativeLength::UNIT_LH) == 0)
      {
        tag_ = kFontRelative;
        length_ = FontRelativeLength::Lh(value);
        return;
      }
      if (strcmp(unit, FontRelativeLength::UNIT_RLH) == 0)
      {
        tag_ = kFontRelative;
        length_ = FontRelativeLength::Rlh(value);
        return;
      }

      // Viewport percentage lengths
      if (strcmp(unit, ViewportPercentageLength::UNIT_VW) == 0)
      {
        tag_ = kViewportPercentage;
        length_ = ViewportPercentageLength::Vw(value);
        return;
      }
      if (strcmp(unit, ViewportPercentageLength::UNIT_VH) == 0)
      {
        tag_ = kViewportPercentage;
        length_ = ViewportPercentageLength::Vh(value);
        return;
      }
      if (strcmp(unit, ViewportPercentageLength::UNIT_VMIN) == 0)
      {
        tag_ = kViewportPercentage;
        length_ = ViewportPercentageLength::Vmin(value);
        return;
      }
      if (strcmp(unit, ViewportPercentageLength::UNIT_VMAX) == 0)
      {
        tag_ = kViewportPercentage;
        length_ = ViewportPercentageLength::Vmax(value);
        return;
      }
      if (strcmp(unit, ViewportPercentageLength::UNIT_VB) == 0)
      {
        tag_ = kViewportPercentage;
        length_ = ViewportPercentageLength::Vb(value);
        return;
      }
      if (strcmp(unit, ViewportPercentageLength::UNIT_VI) == 0)
      {
        tag_ = kViewportPercentage;
        length_ = ViewportPercentageLength::Vi(value);
        return;
      }

      // Container relative lengths
      if (strcmp(unit, ContainerRelativeLength::UNIT_CQW) == 0)
      {
        tag_ = kContainerRelative;
        length_ = ContainerRelativeLength::Cqw(value);
        return;
      }
      if (strcmp(unit, ContainerRelativeLength::UNIT_CQH) == 0)
      {
        tag_ = kContainerRelative;
        length_ = ContainerRelativeLength::Cqh(value);
        return;
      }
      if (strcmp(unit, ContainerRelativeLength::UNIT_CQI) == 0)
      {
        tag_ = kContainerRelative;
        length_ = ContainerRelativeLength::Cqi(value);
        return;
      }
      if (strcmp(unit, ContainerRelativeLength::UNIT_CQB) == 0)
      {
        tag_ = kContainerRelative;
        length_ = ContainerRelativeLength::Cqb(value);
        return;
      }
      if (strcmp(unit, ContainerRelativeLength::UNIT_CQMIN) == 0)
      {
        tag_ = kContainerRelative;
        length_ = ContainerRelativeLength::Cqmin(value);
        return;
      }
      if (strcmp(unit, ContainerRelativeLength::UNIT_CQMAX) == 0)
      {
        tag_ = kContainerRelative;
        length_ = ContainerRelativeLength::Cqmax(value);
        return;
      }

      // Unreachable
      cerr << "Failed to parse length: " << value << unit << endl;
      assert(false && "Invalid unit.");
    }

  public:
    std::string toCss() const override
    {
      return std::to_string(unitlessValue()) + std::string(unit());
    }
    float toComputedValue(computed::Context &context) const override
    {
      if (tag_ == kAbsolute)
      {
        const auto &absolute_length = std::get<AbsoluteLength>(length_);
        return absolute_length.toPx();
      }
      else if (tag_ == kFontRelative)
      {
        const auto &font_relative_length = std::get<FontRelativeLength>(length_);
        return font_relative_length.toComputedValue(context);
      }
      else if (tag_ == kViewportPercentage)
      {
        const auto &viewport_percentage_length = std::get<ViewportPercentageLength>(length_);
        return viewport_percentage_length.toComputedValue(context);
      }
      else if (tag_ == kContainerRelative)
      {
        assert(false && "Container relative length is not implemented yet.");
      }

      // Unreachable
      assert(false && "Invalid tag.");
    }

    float unitlessValue() const
    {
      switch (tag_)
      {
      case kAbsolute:
        return std::get<AbsoluteLength>(length_).unitlessValue();
      case kFontRelative:
        return std::get<FontRelativeLength>(length_).unitlessValue();
      case kViewportPercentage:
        return std::get<ViewportPercentageLength>(length_).unitlessValue();
      case kContainerRelative:
        return std::get<ContainerRelativeLength>(length_).unitlessValue();
      }
      assert(false && "Invalid tag.");
    }
    // Returns the unit as a string.
    const char *unit() const
    {
      switch (tag_)
      {
      case kAbsolute:
        return std::get<AbsoluteLength>(length_).unit();
      case kFontRelative:
        return std::get<FontRelativeLength>(length_).unit();
      case kViewportPercentage:
        return std::get<ViewportPercentageLength>(length_).unit();
      case kContainerRelative:
        return std::get<ContainerRelativeLength>(length_).unit();
      }
      assert(false && "Invalid tag.");
    }

    inline bool isZero() const { return unitlessValue() == 0.0f; }
    inline bool isInfinite() const { return unitlessValue() == std::numeric_limits<float>::infinity(); }
    inline bool isNan() const { return unitlessValue() != unitlessValue(); }
    inline bool isNegative() const { return unitlessValue() < 0.0f; }

  private:
    Tag tag_;
    LengthVariant length_;
  };

  class LengthPercentage : public Parse,
                           public ToCss,
                           public ToComputedValue<computed::LengthPercentage>
  {
    friend class Parse;

  private:
    enum Tag : uint8_t
    {
      kLength,
      kPercentage,
      kCalc,
    };
    using ValueVariant = std::variant<NoCalcLength, computed::Percentage, CalcLengthPercentage>;

  public:
    // Creates a LengthPercentage from a inner length percentage value.
    static LengthPercentage From(crates::css2::values::specified::LengthPercentage inner_length_percentage)
    {
      if (inner_length_percentage.isNoCalcLength())
      {
        const auto &no_calc_length = inner_length_percentage.getNoCalcLength();
        return Parse::ParseSingleValue<NoCalcLength>(no_calc_length.toCss());
      }
      else if (inner_length_percentage.isPercentage())
      {
        const auto &percentage = inner_length_percentage.getPercentage();
        return LengthPercentage(computed::Percentage(percentage.value));
      }
      else
      {
        // TODO(yorkie): support calc length.
        return LengthPercentage(0.0f);
      }
    }
    // Returns if the input string is a valid length or percentage.
    static bool IsLengthOrPercentage(const std::string &input)
    {
      return input.ends_with("%") || NoCalcLength::MaybeDimension(input);
    }

  public:
    LengthPercentage(float value = 0.0f)
        : tag_(kLength),
          value_(NoCalcLength::FromPx(value))
    {
    }

  private:
    LengthPercentage(NoCalcLength length)
        : tag_(kLength),
          value_(length)
    {
    }
    LengthPercentage(computed::Percentage percentage)
        : tag_(kPercentage),
          value_(percentage)
    {
    }

  private:
    bool parse(const std::string &input) override
    {
      if (input.empty())
        return true;

      if (input.ends_with("%"))
      {
        float percent = std::stof(input.substr(0, input.length() - 1));
        tag_ = kPercentage;
        value_ = computed::Percentage(percent / 100.0f);
        return true;
      }
      else if (input.starts_with("calc(") && input.ends_with(")"))
      {
        tag_ = kCalc;
        value_ = CalcLengthPercentage();
        return true;
      }
      else
      {
        tag_ = kLength;
        value_ = Parse::ParseSingleValue<NoCalcLength>(input);
        return true;
      }
    }

  public:
    std::string toCss() const override
    {
      switch (tag_)
      {
      case kLength:
        return std::get<NoCalcLength>(value_).toCss();
      case kPercentage:
        return std::get<computed::Percentage>(value_).toCss();
      case kCalc:
        // TODO(yorkie): support calc length.
        return "calc()";
      }
    }
    computed::LengthPercentage toComputedValue(computed::Context &context) const override
    {
      if (tag_ == kLength)
      {
        auto length = std::get<NoCalcLength>(value_).toComputedValue(context);
        return computed::LengthPercentage::Length(length);
      }
      else if (tag_ == kPercentage)
      {
        auto percent = std::get<computed::Percentage>(value_).value();
        return computed::LengthPercentage::Percentage(percent);
      }
      else
      {
        // TODO(yorkie): support calc length.
        return computed::LengthPercentage::Length(0.0f);
      }
    }

  private:
    Tag tag_;
    ValueVariant value_;
  };

  using NonNegativeLengthPercentage = generics::NonNegative<LengthPercentage>;

  class Size final : public generics::GenericSize<Size, NonNegativeLengthPercentage>,
                     public Parse,
                     public ToComputedValue<computed::Size>
  {
    friend class Parse;
    using generics::GenericSize<Size, NonNegativeLengthPercentage>::GenericSize;

  public:
    Size() : generics::GenericSize<Size, NonNegativeLengthPercentage>()
    {
    }

  private:
    bool parse(const std::string &input) override
    {
      if (input == "auto")
        setAuto();
      if (LengthPercentage::IsLengthOrPercentage(input))
        setLengthPercentage(Parse::ParseSingleValue<NonNegativeLengthPercentage>(input));
      return true;
    }

  public:
    computed::Size toComputedValue(computed::Context &context) const override
    {
      if (isAuto())
        return computed::Size::Auto();
      else if (isLengthPercentage())
        return computed::Size::LengthPercentage(lengthPercent().toComputedValue(context));
      assert(false && "Invalid tag.");
    }
  };

  class MarginSize : public generics::GenericMargin<MarginSize, specified::LengthPercentage>,
                     public Parse,
                     public ToComputedValue<computed::MarginSize>
  {
    friend class Parse;
    using generics::GenericMargin<MarginSize, specified::LengthPercentage>::GenericMargin;

  private:
    bool parse(const std::string &input) override
    {
      if (input == "auto")
        setAuto();
      else if (LengthPercentage::IsLengthOrPercentage(input))
        setLengthPercentage(Parse::ParseSingleValue<specified::LengthPercentage>(input));
      return true;
    }

  public:
    computed::MarginSize toComputedValue(computed::Context &context) const override
    {
      if (isAuto())
        return computed::MarginSize::Auto();
      else if (isLengthPercentage())
        return computed::MarginSize::LengthPercentage(lengthPercent().toComputedValue(context));
      assert(false && "Invalid tag.");
    }
  };
}
