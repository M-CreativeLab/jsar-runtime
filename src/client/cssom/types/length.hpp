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
}
