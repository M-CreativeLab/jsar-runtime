#pragma once

#include <algorithm>
#include <assert.h>
#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/common.hpp>
#include <client/cssom/values/generics/common.hpp>
#include <client/cssom/values/computed/context.hpp>

namespace client_cssom::values::specified
{
  class AllowedNumbericType
  {
  private:
    enum Tag
    {
      kAll,
      kNonNegative,
      kAtLeastOne,
      kZeroToOne
    };

  public:
    static AllowedNumbericType Default() { return AllowedNumbericType(); }
    static AllowedNumbericType All() { return AllowedNumbericType(kAll); }
    static AllowedNumbericType NonNegative() { return AllowedNumbericType(kNonNegative); }
    static AllowedNumbericType AtLeastOne() { return AllowedNumbericType(kAtLeastOne); }
    static AllowedNumbericType ZeroToOne() { return AllowedNumbericType(kZeroToOne); }

  public:
    AllowedNumbericType() : tag_(kAll) {}

  private:
    AllowedNumbericType(Tag tag) : tag_(tag) {}

  public:
    // Clamp the value following the rules of this numeric type.
    float clamp(float v) const
    {
      switch (tag_)
      {
      case kAll:
        return v;
      case kNonNegative:
        return std::max(0.0f, v);
      case kAtLeastOne:
        return std::max(1.0f, v);
      case kZeroToOne:
        return std::clamp(v, 0.0f, 1.0f);
      }
      assert(false && "Invalid tag.");
    }

  private:
    Tag tag_;
  };

  // `<number>`
  class Number : public Parse,
                 public ToCss,
                 public ToComputedValue<CSSFloat>
  {
    friend class Parse;

  public:
    Number(float value = 0.0f, std::optional<AllowedNumbericType> clamping_mode = std::nullopt)
        : value_(value),
          calc_clamping_mode_(clamping_mode)
    {
    }

  private:
    bool parse(const std::string &input) override
    {
      value_ = std::stof(input);
      // TODO(yorkie): support calc function.
      return true;
    }

  public:
    std::string toCss() const override { return std::to_string(value_); }
    CSSFloat toComputedValue(computed::Context &) const override
    {
      if (calc_clamping_mode_.has_value())
        return calc_clamping_mode_->clamp(value_);
      return value_;
    }
    inline bool wasCalc() const { return calc_clamping_mode_.has_value(); }

  private:
    float value_;
    std::optional<AllowedNumbericType> calc_clamping_mode_;
  };

  using NonNegativeNumber = generics::NonNegative<Number>;
}
