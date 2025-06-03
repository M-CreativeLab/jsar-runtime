#pragma once

#include <client/cssom/style_traits.hpp>

namespace client_cssom::values
{
  class CSSFloat : public ToCss,
                   public Parse
  {
    friend class Parse;

  public:
    CSSFloat() : value(0.0f) {}
    CSSFloat(float v) : value(v) {}

  public:
    CSSFloat operator+(const float other) const { return CSSFloat(value + other); }
    CSSFloat operator-(const float other) const { return CSSFloat(value - other); }
    CSSFloat operator*(const float other) const { return CSSFloat(value * other); }
    CSSFloat operator/(const float other) const
    {
      if (other == 0.0f)
        throw std::runtime_error("Division by zero in CSSFloat.");
      return CSSFloat(value / other);
    }

  public:
    inline bool isZero() const { return value == 0.0f; }
    std::string toCss() const override
    {
      return std::to_string(value);
    }

  private:
    bool parse(const std::string &input) override
    {
      try
      {
        value = std::stof(input);
        return true;
      }
      catch (const std::invalid_argument &)
      {
        return false; // Invalid float format.
      }
      catch (const std::out_of_range &)
      {
        return false; // Float value out of range.
      }
    }

  public:
    float value;
  };
}
