#pragma once

#include <client/cssom/values/common.hpp>
#include <client/cssom/style_traits.hpp>

namespace client_cssom::values::generics
{
  class GenericTime : public ToCss
  {
  public:
    GenericTime(CSSFloat seconds = 0.0f)
        : seconds_(seconds)
    {
    }

  public:
    CSSFloat seconds() const
    {
      return seconds_;
    }

    std::string toCss() const override
    {
      return seconds_.toCss() + "s";
    }

  protected:
    CSSFloat seconds_;
  };
}
