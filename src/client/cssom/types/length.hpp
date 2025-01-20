#pragma once

#include <cctype>

namespace client_cssom::types
{
#define CSS_UNITS_MAP(XX) \
  XX(Cap)                 \
  XX(Ch)                  \
  XX(Em)                  \
  XX(Ex)                  \
  XX(Ic)                  \
  XX(Rem)                 \
  XX(Rex)                 \
  XX(Vh)                  \
  XX(Vw)                  \
  XX(Vmin)                \
  XX(Vmax)                \
  XX(Px)                  \
  XX(Cm)                  \
  XX(Mm)                  \
  XX(Q)                   \
  XX(In)                  \
  XX(Pc)                  \
  XX(Pt)

  enum class Unit
  {
#define XX(unit) k##unit,
    CSS_UNITS_MAP(XX)
#undef XX
  };

  inline std::string to_string(Unit unit)
  {
#define XX(unit)      \
  case Unit::k##unit: \
    return ToLowerCase(#unit);

    switch (unit)
    {
      CSS_UNITS_MAP(XX)
    default:
      return "Unknown";
    }
#undef XX
  }

  class Length
  {
  public:
    Length() : value_(0), unit_(Unit::kPx) {}
    Length(float value, Unit unit = Unit::kPx) : value_(value), unit_(unit) {}
    Length(const std::string &input)
    {
#define PARSE(unit)                                                                      \
  if (input.ends_with(ToLowerCase(std::string(#unit))))                                  \
  {                                                                                      \
    std::string valueStr = input.substr(0, input.size() - strlen(#unit));                \
    try                                                                                  \
    {                                                                                    \
      value_ = std::stof(valueStr);                                                      \
      unit_ = Unit::k##unit;                                                             \
    }                                                                                    \
    catch (const std::invalid_argument &e)                                               \
    {                                                                                    \
      value_ = 0;                                                                        \
      unit_ = Unit::kPx;                                                                 \
      std::cerr << "Failed to parse length(" << valueStr << "): " << input << std::endl; \
      std::cerr << "  " << e.what() << std::endl;                                        \
    }                                                                                    \
    return;                                                                              \
  }

#define XX(unit) PARSE(unit)
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
    float getValueAsPixels() const
    {
      switch (unit_)
      {
      case Unit::kPx:
        return value_;
      case Unit::kPt:
        return value_ * 96.0f / 72.0f;
      case Unit::kPc:
        return value_ * 16.0f;
      case Unit::kIn:
        return value_ * 96.0f;
      case Unit::kCm:
        return value_ * 96.0f / 2.54f;
      case Unit::kMm:
        return value_ * 96.0f / 25.4f;
      case Unit::kQ:
        return value_ * 96.0f / 25.4f / 40.0f;
      default:
        return 0.0f;
      }
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
