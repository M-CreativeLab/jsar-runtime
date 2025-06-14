#pragma once

#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/generics/easing.hpp>
#include <client/cssom/values/computed/easing.hpp>

namespace client_cssom::values::specified
{
  class TimingFunction : public generics::GenericTimingFunction<TimingFunction>,
                         public Parse,
                         public ToComputedValue<computed::TimingFunction>
  {
    friend class Parse;
    using generics::GenericTimingFunction<TimingFunction>::GenericTimingFunction;

  private:
    bool parse(const std::string &input) override
    {
      if (input == "linear")
      {
        tag_ = kKeyword;
        timing_function_ = kLinear;
        return true;
      }
      else if (input == "ease")
      {
        tag_ = kKeyword;
        timing_function_ = kEase;
        return true;
      }
      else if (input == "ease-in")
      {
        tag_ = kKeyword;
        timing_function_ = kEaseIn;
        return true;
      }
      else if (input == "ease-out")
      {
        tag_ = kKeyword;
        timing_function_ = kEaseOut;
        return true;
      }
      else if (input == "ease-in-out")
      {
        tag_ = kKeyword;
        timing_function_ = kEaseInOut;
        return true;
      }
      return false; // Invalid timing function format.
    }

  public:
    computed::TimingFunction toComputedValue(computed::Context &context) const override
    {
      switch (tag_)
      {
      case kKeyword:
        switch (std::get<TimingKeyword>(timing_function_))
        {
        case kLinear:
          return computed::TimingFunction::Linear();
        case kEase:
          return computed::TimingFunction::Ease();
        case kEaseIn:
          return computed::TimingFunction::EaseIn();
        case kEaseOut:
          return computed::TimingFunction::EaseOut();
        case kEaseInOut:
          return computed::TimingFunction::EaseInOut();
        }
      case kCubicBezier:
      {
        const auto &cubic = std::get<CubicBezierDescriptor>(timing_function_);
        return computed::TimingFunction::CubicBezier(cubic.x1, cubic.y1, cubic.x2, cubic.y2);
      }
      }
      return computed::TimingFunction(); // Should not reach here.
    }
  };
}
