#pragma once

#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/computed/common.hpp>
#include <client/cssom/values/generics/position.hpp>

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
}
