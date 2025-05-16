#pragma once

#include <algorithm>
#include <cmath>
#include <client/cssom/style_traits.hpp>

namespace client_cssom::values::computed
{
  class Percentage : public ToCss
  {
  public:
    static Percentage Handred()
    {
      return Percentage(1.0f);
    }

  public:
    Percentage(float value)
        : value_(value)
    {
    }
    Percentage(const Percentage &) = default;
    Percentage &operator=(const Percentage &) = default;
    Percentage(Percentage &&) = default;
    Percentage &operator=(Percentage &&) = default;
    ~Percentage() = default;

    bool operator==(const Percentage &other) const { return value_ == other.value_; }
    bool operator==(float other) const { return value_ == other; }
    bool operator<(const Percentage &other) const { return value_ < other.value_; }
    bool operator<(float other) const { return value_ < other; }
    bool operator>(const Percentage &other) const { return value_ > other.value_; }
    bool operator>(float other) const { return value_ > other; }

  public:
    std::string toCss() const override
    {
      return std::to_string(value_ * 100.0f) + "%";
    }

    // Returns the value of the percentage in the range of [0.0f, 1.0f].
    inline float value() const { return value_; }
    inline Percentage abs() const
    {
      return Percentage(std::abs(value_));
    }
    inline Percentage clampToNonNegative() const
    {
      return Percentage(std::max(0.0f, value_));
    }

    // Compute the percentage value with the base.
    inline float computeWithBase(float base) const
    {
      return std::round(value_ * base);
    }

  private:
    float value_; // 0.0f ~ 1.0f
  };
}
