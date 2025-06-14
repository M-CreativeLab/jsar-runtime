#pragma once

#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/generics/time.hpp>
#include <client/cssom/values/computed/time.hpp>

namespace client_cssom::values::specified
{

  class Time : public generics::GenericTime,
               public Parse,
               public ToComputedValue<computed::Time>
  {
    friend class Parse;
    using GenericTime::GenericTime;

  private:
    enum TimeUnit : uint8_t
    {
      kSeconds,
      kMilliseconds,
    };

  public:
    static constexpr const char *UNIT_SECONDS = "s";
    static constexpr const char *UNIT_MILLISECONDS = "ms";

  public:
    inline const char *unit() const
    {
      switch (unit_)
      {
      case kSeconds:
        return UNIT_SECONDS;
      case kMilliseconds:
        return UNIT_MILLISECONDS;
      }
      assert(false && "Invalid time unit.");
      return "";
    }
    inline bool isSeconds() const
    {
      return unit_ == kSeconds;
    }
    inline bool isMilliseconds() const
    {
      return unit_ == kMilliseconds;
    }

  private:
    bool parse(const std::string &input) override
    {
      if (input.back() == 's')
      {
        unit_ = kSeconds;
        seconds_ = Parse::ParseSingleValue<CSSFloat>(input.substr(0, input.size() - 1));
        return true;
      }
      else if (input.back() == 'm' && input.size() > 1 && input[input.size() - 2] == 's')
      {
        unit_ = kMilliseconds;
        CSSFloat milliseconds = Parse::ParseSingleValue<CSSFloat>(input.substr(0, input.size() - 2));
        seconds_ = milliseconds / 1000.0f; // Convert milliseconds to seconds.
        return true;
      }
      else
      {
        return false; // Invalid time format.
      }
    }

  public:
    computed::Time toComputedValue(values::computed::Context &context) const override
    {
      return computed::Time(seconds_);
    }

  private:
    TimeUnit unit_;
  };
}
