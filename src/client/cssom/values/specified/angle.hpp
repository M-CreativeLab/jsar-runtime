#pragma once

#include <cmath>
#include <assert.h>
#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/computed/angle.hpp>

namespace client_cssom::values::specified
{
  class Angle;
  class AngleDimension
  {
    friend class Angle;

  private:
    enum Tag
    {
      kDeg,
      kGrad,
      kRad,
      kTurn,
    };

    static constexpr const char *UNIT_DEG = "deg";
    static constexpr const char *UNIT_GRAD = "grad";
    static constexpr const char *UNIT_RAD = "rad";
    static constexpr const char *UNIT_TURN = "turn";

  public:
    static AngleDimension Deg(float value) { return AngleDimension(kDeg, value); }
    static AngleDimension Grad(float value) { return AngleDimension(kGrad, value); }
    static AngleDimension Rad(float value) { return AngleDimension(kRad, value); }
    static AngleDimension Turn(float value) { return AngleDimension(kTurn, value); }

  private:
    AngleDimension(Tag tag, float value = 0.0f)
        : tag_(tag),
          unitless_value_(value)
    {
    }

  public:
    inline bool isDeg() const { return tag_ == kDeg; }
    inline bool isGrad() const { return tag_ == kGrad; }
    inline bool isRad() const { return tag_ == kRad; }
    inline bool isTurn() const { return tag_ == kTurn; }
    inline bool isZero() const { return unitless_value_ == 0.0f; }

    inline float unitlessValue() const { return unitless_value_; }
    inline const char *unit() const
    {
      switch (tag_)
      {
      case kDeg:
        return UNIT_DEG;
      case kGrad:
        return UNIT_GRAD;
      case kRad:
        return UNIT_RAD;
      case kTurn:
        return UNIT_TURN;
      }
      assert(false && "Invalid tag.");
    }

    // Returns the amount of degrees this angle represents.
    float toDegrees() const
    {
      const float DEG_PER_RAD = 180.0f / M_PI;
      const float DEG_PER_TURN = 360.0f;
      const float DEG_PER_GRAD = 360.0f / 400.0f;

      switch (tag_)
      {
      case kDeg:
        return unitless_value_;
      case kRad:
        return unitless_value_ * DEG_PER_RAD;
      case kTurn:
        return unitless_value_ * DEG_PER_TURN;
      case kGrad:
        return unitless_value_ * DEG_PER_GRAD;
      }
      assert(false && "Invalid tag.");
    }

  private:
    Tag tag_;
    float unitless_value_;
  };

  class Angle : public Parse,
                public ToCss,
                public ToComputedValue<computed::Angle>
  {
    friend class Parse;

  public:
    static Angle Zero() { return Angle(AngleDimension::Deg(0.0f), false); }

  public:
    Angle() = default;

  private:
    Angle(AngleDimension value, bool was_calculated)
        : value_(value),
          was_calculated_(was_calculated)
    {
    }

  private:
    bool parse(const std::string &input) override
    {
      if (!isdigit(input[0]))
        return false;

      if (input.ends_with(AngleDimension::UNIT_DEG))
      {
        value_ = AngleDimension::Deg(std::stof(input.substr(0, input.length() - 3)));
        return true;
      }
      if (input.ends_with(AngleDimension::UNIT_GRAD))
      {
        value_ = AngleDimension::Grad(std::stof(input.substr(0, input.length() - 5)));
        return true;
      }
      if (input.ends_with(AngleDimension::UNIT_RAD))
      {
        value_ = AngleDimension::Rad(std::stof(input.substr(0, input.length() - 4)));
        return true;
      }
      if (input.ends_with(AngleDimension::UNIT_TURN))
      {
        value_ = AngleDimension::Turn(std::stof(input.substr(0, input.length() - 5)));
        return true;
      }
      return false;
    }

  public:
    inline bool isZero() const { return value_.isZero(); }

    std::string toCss() const override
    {
      return std::to_string(value_.unitlessValue()) + value_.unit();
    }
    computed::Angle toComputedValue(computed::Context &) const override
    {
      float degrees = value_.toDegrees();
      return computed::Angle::FromDegrees(std::isinf(degrees) ? 0.0f : degrees);
    }

  private:
    AngleDimension value_ = AngleDimension::Deg(0.0f);
    bool was_calculated_ = false;
  };
}
