#pragma once

#include <cmath>
#include <client/cssom/style_traits.hpp>

namespace client_cssom::values::computed
{
  // Angle class for computed values.
  // The value is stored in degrees.
  class Angle : public ToCss
  {
    static constexpr float RAD_PER_DEG = M_PI / 180.0f;

  public:
    static Angle FromRadians(float radians)
    {
      return Angle(radians / RAD_PER_DEG);
    }
    static Angle FromDegrees(float degrees)
    {
      return Angle(degrees);
    }

  public:
    Angle(float degrees = 0.0f)
        : value_(degrees)
    {
    }

    std::string toCss() const override
    {
      return std::to_string(value_) + "deg";
    }
    float degrees() const
    {
      return value_;
    }
    float radians() const
    {
      return value_ * RAD_PER_DEG;
    }

  private:
    float value_;
  };
}
