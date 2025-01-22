#pragma once

#include <cctype>

namespace client_cssom::types
{
#define CSS_RELATIVE_UNITS_ON_ELEMENT_MAP(XX) \
  XX(CAP, "cap")                              \
  XX(CH, "ch")                                \
  XX(EM, "em")                                \
  XX(EX, "ex")                                \
  XX(IC, "ic")                                \
  XX(LH, "lh")

#define CSS_RELATIVE_UNITS_ON_ROOT_MAP(XX) \
  XX(RCAP, "rcap")                         \
  XX(RCH, "rch")                           \
  XX(REM, "rem")                           \
  XX(REX, "rex")                           \
  XX(RIC, "ric")                           \
  XX(RLH, "rlh")

#define CSS_RELATIVE_UNITS_ON_VIEWPORT_MAP(XX) \
  XX(VH, "vh")                                 \
  XX(VW, "vw")                                 \
  XX(VMIN_, "vmin")                            \
  XX(VMAX_, "vmax")                            \
  XX(VB, "vb")                                 \
  XX(VI, "vi")

#define CSS_ABSOLUTE_UNITS_MAP(XX) \
  XX(PX, "px")                     \
  XX(CM, "cm")                     \
  XX(MM, "mm")                     \
  XX(Q, "q")                       \
  XX(IN, "in")                     \
  XX(PC, "pc")                     \
  XX(PT, "pt")

#define CSS_UNITS_MAP(XX)                \
  CSS_RELATIVE_UNITS_ON_ELEMENT_MAP(XX)  \
  CSS_RELATIVE_UNITS_ON_ROOT_MAP(XX)     \
  CSS_RELATIVE_UNITS_ON_VIEWPORT_MAP(XX) \
  CSS_ABSOLUTE_UNITS_MAP(XX)

  enum class Unit
  {
#define XX(UNIT, _) k##UNIT,
    CSS_UNITS_MAP(XX)
#undef XX
  };

  inline std::string to_string(Unit unit)
  {
#define XX(UNIT, STR) \
  case Unit::k##UNIT: \
    return STR;

    switch (unit)
    {
      CSS_UNITS_MAP(XX)
    default:
      assert(false && "Invalid unit");
    }
#undef XX
  }

  inline bool isAbsoluteUnit(Unit unit)
  {
    switch (unit)
    {
#define XX(UNIT, _)   \
  case Unit::k##UNIT: \
    return true;
      CSS_ABSOLUTE_UNITS_MAP(XX)
#undef XX
    default:
      return false;
    }
  }

  inline bool isElementBasedRelativeUnit(Unit unit)
  {
    switch (unit)
    {
#define XX(UNIT, _)   \
  case Unit::k##UNIT: \
    return true;
      CSS_RELATIVE_UNITS_ON_ELEMENT_MAP(XX)
#undef XX
    default:
      return false;
    }
  }

  inline bool isRootBasedRelativeUnit(Unit unit)
  {
    switch (unit)
    {
#define XX(UNIT, _)   \
  case Unit::k##UNIT: \
    return true;
      CSS_RELATIVE_UNITS_ON_ROOT_MAP(XX)
#undef XX
    default:
      return false;
    }
  }

  inline bool isViewportBasedRelativeUnit(Unit unit)
  {
    switch (unit)
    {
#define XX(UNIT, _)   \
  case Unit::k##UNIT: \
    return true;
      CSS_RELATIVE_UNITS_ON_VIEWPORT_MAP(XX)
#undef XX
    default:
      return false;
    }
  }

  class FontBasedComputationContext
  {
  public:
    FontBasedComputationContext(float fontSize)
        : fontSize(fontSize)
    {
    }

  public:
    float fontSize;
    float lineHeight;
  };

  inline float computeFontBasedRelativeLengthInPixels(Unit unit, float value,
                                                      const FontBasedComputationContext &context)
  {
    assert(isElementBasedRelativeUnit(unit)); // Must be a relative unit.

    switch (unit)
    {
    case Unit::kEM:
      return value * context.fontSize;
    case Unit::kEX:
      return value * context.fontSize * 0.5f;
    case Unit::kCAP:
      return value * context.fontSize * 0.7f;
    case Unit::kCH:
      return value * context.fontSize * 0.5f;
    case Unit::kIC:
      return value * context.fontSize * 0.5f;
    case Unit::kLH:
      return value * context.lineHeight;
    default:
      assert(false && "Invalid relative unit");
    }
  }

  class Length
  {
  public:
    Length() : value_(0), unit_(Unit::kPX) {}
    Length(float value, Unit unit = Unit::kPX) : value_(value), unit_(unit) {}
    Length(const std::string &input)
    {
#define PARSE(UNIT, STR)                                                                 \
  if (input.ends_with(STR))                                                              \
  {                                                                                      \
    std::string valueStr = input.substr(0, input.size() - strlen(STR));                  \
    try                                                                                  \
    {                                                                                    \
      value_ = std::stof(valueStr);                                                      \
      unit_ = Unit::k##UNIT;                                                             \
    }                                                                                    \
    catch (const std::invalid_argument &e)                                               \
    {                                                                                    \
      value_ = 0;                                                                        \
      unit_ = Unit::kPX;                                                                 \
      std::cerr << "Failed to parse length(" << valueStr << "): " << input << std::endl; \
      std::cerr << "  " << e.what() << std::endl;                                        \
    }                                                                                    \
    return;                                                                              \
  }

#define XX(UNIT, STR) PARSE(UNIT, STR)
      CSS_UNITS_MAP(XX)
#undef XX
#undef PARSE
    }

  public:
    friend std::ostream &operator<<(std::ostream &os, const Length &length)
    {
      os << length.value_ << to_string(length.unit_);
      return os;
    }

  public:
    inline float value() const { return value_; }
    inline float &value() { return value_; }
    inline Unit unit() const { return unit_; }
    inline Unit &unit() { return unit_; }

  public:
    /**
     * @returns `true` if the length is an absolute unit, such as px, cm, mm, in, pc, or pt.
     */
    bool isAbsoluteLength() const { return isAbsoluteUnit(unit_); }
    /**
     * @returns `true` if the length is a relative unit based on the element's font size, such
     *          as em, ex, cap, ch, ic, or lh.
     */
    bool isElementBasedRelativeLength() const { return isElementBasedRelativeUnit(unit_); }
    /**
     * @returns `true` if the length is a relative unit based on the root element's font size, such
     *          as rem, rch, rcap, rex, ric, or rlh.
     */
    bool isRootBasedRelativeLength() const { return isRootBasedRelativeUnit(unit_); }
    /**
     * @returns `true` if the length is a relative unit based on the viewport size, such as vh, vw,
     *          vmin, vmax, vb, or vi.
     */
    bool isViewportBasedRelativeLength() const { return isViewportBasedRelativeUnit(unit_); }
    /**
     * It computes the absolute length in pixels, this function should be called only when the length is
     * an absolute unit.
     *
     * @returns The absolute length in pixels.
     */
    float computeAbsoluteLengthInPixels() const
    {
      assert(isAbsoluteLength()); // Must be an absolute unit.

      switch (unit_)
      {
      case Unit::kPX:
        return value_;
      case Unit::kPT:
        return value_ * 96.0f / 72.0f;
      case Unit::kPC:
        return value_ * 16.0f;
      case Unit::kIN:
        return value_ * 96.0f;
      case Unit::kCM:
        return value_ * 96.0f / 2.54f;
      case Unit::kMM:
        return value_ * 96.0f / 25.4f;
      case Unit::kQ:
        return value_ * 96.0f / 25.4f / 40.0f;
      default:
        return 0.0f;
      }
    }
    /**
     * It computes the relative length in pixels, this function should be called only when the length is
     * a relative unit based on the element's font size.
     *
     * @param context The context for the font-based computation.
     * @returns The absolute value in pixels.
     */
    float computeElementBasedLengthInPixels(const FontBasedComputationContext &context) const
    {
      return computeFontBasedRelativeLengthInPixels(unit_, value_, context);
    }

  private:
    float value_;
    Unit unit_;
  };

#undef CSS_UNITS_MAP

  class LengthPercentage : public Length
  {
  private:
    enum class _ValueType
    {
      kLength,
      kPercentage,
    };

  public:
    /**
     * Constructs a percentage value.
     *
     * @param percentage The percentage value between 0 and 100.
     */
    static LengthPercentage Percentage(float percentage)
    {
      assert(percentage >= 0 && percentage <= 100);
      LengthPercentage value;
      value.percentage_ = percentage;
      return value;
    }

  public:
    LengthPercentage()
        : Length(),
          percentage_(std::nullopt)
    {
    }
    LengthPercentage(float value, Unit unit)
        : Length(value, unit),
          percentage_(std::nullopt)
    {
    }
    LengthPercentage(const std::string &input)
    {
      bool needsParseLength = true;
      if (input.ends_with("%"))
      {
        try
        {
          percentage_ = std::stof(input.substr(0, input.size() - 1));
          needsParseLength = false;
        }
        catch (const std::invalid_argument &e)
        {
          std::cerr << "Failed to parse percentage: " << input << std::endl;
          std::cerr << "  " << e.what() << std::endl;
        }
      }

      // If the input doesn't end with '%' or failed to parse percentage, try to parse length.
      if (needsParseLength)
      {
        percentage_ = std::nullopt;
        Length::operator=(Length(input));
      }
    }

  public:
    friend std::ostream &operator<<(std::ostream &os,
                                    const LengthPercentage &value)
    {
      if (value.isPercentage())
        os << value.percentage() << "%";
      else
        os << static_cast<const Length &>(value);
      return os;
    }

  public:
    bool isPercentage() const { return percentage_.has_value(); }
    /**
     * @returns The percentage value between 0 and 100.
     */
    float percentage() const
    {
      assert(percentage_.has_value());
      return percentage_.value();
    }

  protected:
    std::optional<float> percentage_ = std::nullopt;
  };
}
