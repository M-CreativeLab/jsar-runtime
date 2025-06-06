#pragma once

#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/generics/position.hpp>
#include <client/cssom/values/computed/common.hpp>
#include <client/cssom/values/computed/position.hpp>
#include <client/cssom/values/specified/length.hpp>

namespace client_cssom::values::specified
{
  class PositionType : public generics::PositionType,
                       public Parse,
                       public ToCss,
                       public ToComputedValue<generics::PositionType>
  {
    friend class Parse;

  private:
    bool parse(const std::string &input) override
    {
      if (input == "static")
        tag_ = kStatic;
      else if (input == "relative")
        tag_ = kRelative;
      else if (input == "absolute")
        tag_ = kAbsolute;
      else if (input == "fixed")
        tag_ = kFixed;
      else if (input == "sticky")
        tag_ = kSticky;
      return true;
    }

  public:
    std::string toCss() const override
    {
      switch (tag_)
      {
      case kStatic:
        return "static";
      case kRelative:
        return "relative";
      case kAbsolute:
        return "absolute";
      case kFixed:
        return "fixed";
      case kSticky:
        return "sticky";
      }
      return "";
    }
    generics::PositionType toComputedValue(computed::Context &) const override
    {
      return *this;
    }
  };

  class InsetSize : public generics::GenericMargin<InsetSize, specified::LengthPercentage>,
                    public Parse,
                    public ToComputedValue<computed::InsetSize>
  {
    friend class Parse;
    using generics::GenericMargin<InsetSize, specified::LengthPercentage>::GenericMargin;

  private:
    bool parse(const std::string &input) override
    {
      if (input == "auto")
        setAuto();
      else if (LengthPercentage::IsLengthOrPercentage(input))
        setLengthPercentage(Parse::ParseSingleValue<specified::LengthPercentage>(input));
      return true;
    }

  public:
    computed::InsetSize toComputedValue(computed::Context &context) const override
    {
      if (isAuto())
        return computed::InsetSize::Auto();
      else if (isLengthPercentage())
        return computed::InsetSize::LengthPercentage(lengthPercent().toComputedValue(context));
      assert(false && "Invalid tag.");
    }
  };
}
