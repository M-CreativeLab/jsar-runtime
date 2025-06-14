#pragma once

#include <cstdint>
#include <variant>
#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/common.hpp>

namespace client_cssom::values::generics
{
  template <typename T>
  class GenericTimingFunction : public ToCss
  {
  protected:
    enum Tag : uint8_t
    {
      kKeyword,
      kCubicBezier,
      // TODO(yorkie): Implement steps() and other timing functions.
    };
    enum TimingKeyword : uint8_t
    {
      kLinear,
      kEase,
      kEaseIn,
      kEaseOut,
      kEaseInOut,
    };
    struct CubicBezierDescriptor
    {
      float x1, y1, x2, y2;
      CubicBezierDescriptor(float x1, float y1, float x2, float y2)
          : x1(x1)
          , y1(y1)
          , x2(x2)
          , y2(y2)
      {
      }
    };
    using TimingFunctionVariant = std::variant<TimingKeyword, CubicBezierDescriptor>;

  public:
    static T Linear()
    {
      return T(kLinear);
    }
    static T Ease()
    {
      return T(kEase);
    }
    static T EaseIn()
    {
      return T(kEaseIn);
    }
    static T EaseOut()
    {
      return T(kEaseOut);
    }
    static T EaseInOut()
    {
      return T(kEaseInOut);
    }
    static T CubicBezier(float x1, float y1, float x2, float y2)
    {
      return T(x1, y1, x2, y2);
    }

  protected:
    GenericTimingFunction()
        : tag_(kKeyword)
        , timing_function_(kLinear)
    {
    }
    GenericTimingFunction(TimingKeyword keyword)
        : tag_(kKeyword)
        , timing_function_(keyword)
    {
    }
    GenericTimingFunction(float x1, float y1, float x2, float y2)
        : tag_(kCubicBezier)
        , timing_function_(CubicBezierDescriptor(x1, y1, x2, y2))
    {
    }
    // TODO(yorkie): Implement steps() and other timing functions.

  public:
    const TimingFunctionVariant &timing_function() const
    {
      return timing_function_;
    }
    std::string toCss() const override
    {
      switch (tag_)
      {
      case kKeyword:
        switch (std::get<TimingKeyword>(timing_function_))
        {
        case kLinear:
          return "linear";
        case kEase:
          return "ease";
        case kEaseIn:
          return "ease-in";
        case kEaseOut:
          return "ease-out";
        case kEaseInOut:
          return "ease-in-out";
        }
        break;
      case kCubicBezier:
      {
        const auto &cubic = std::get<CubicBezierDescriptor>(timing_function_);
        return "cubic-bezier(" + std::to_string(cubic.x1) + ", " + std::to_string(cubic.y1) + ", " +
               std::to_string(cubic.x2) + ", " + std::to_string(cubic.y2) + ")";
      }
        // TODO(yorkie): Implement steps() and other timing functions.
      }
      return "";
    }

  protected:
    Tag tag_;
    TimingFunctionVariant timing_function_;
  };
}
