#pragma once

#include <client/cssom/values/common.hpp>
#include <client/cssom/values/generics/common.hpp>

namespace client_cssom::values::computed
{
  using NonNegativeNumber = generics::NonNegative<CSSFloat>;

  // A computed value for `min-width`, `min-height`, `width` or `height` property.
  class CSSPixelLength
  {
  public:
    CSSPixelLength(float px)
        : value(px)
    {
    }

    bool operator==(const CSSPixelLength &other) const { return value == other.value; }
    bool operator==(float other) const { return value == other; }
    bool operator<(const CSSPixelLength &other) const { return value < other.value; }
    bool operator<(float other) const { return value < other; }
    bool operator>(const CSSPixelLength &other) const { return value > other.value; }
    bool operator>(float other) const { return value > other; }

  public:
    inline float px() const { return value; }

    // Returns the absolute value of the CSSPixelLength.
    inline CSSPixelLength abs() const
    {
      return CSSPixelLength(std::abs(value));
    }

    inline CSSPixelLength min(const CSSPixelLength &other) const
    {
      return CSSPixelLength(std::min(value, other.value));
    }
    inline CSSPixelLength max(const CSSPixelLength &other) const
    {
      return CSSPixelLength(std::max(value, other.value));
    }

  public:
    float value;
  };
}
