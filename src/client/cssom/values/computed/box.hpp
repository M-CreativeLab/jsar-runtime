#pragma once

#include <client/cssom/values/generics/rect.hpp>
#include <client/cssom/values/specified/box.hpp>
#include <client/cssom/values/computed/length_percentage.hpp>
#include <client/cssom/values/computed/length.hpp>

namespace client_cssom::values::computed
{
  using Display = specified::Display;
  using BoxSizing = specified::BoxSizing;
  using Overflow = specified::Overflow;

  class Padding : public generics::Rect<LengthPercentage>
  {
  public:
    Padding(generics::Rect<LengthPercentage> rect)
        : generics::Rect<LengthPercentage>(rect)
    {
    }
  };

  class Margin : public generics::Rect<MarginSize>
  {
  public:
    Margin(generics::Rect<MarginSize> rect)
        : generics::Rect<MarginSize>(rect)
    {
    }
  };
}
