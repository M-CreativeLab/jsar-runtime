#pragma once

#include <algorithm>
#include <cmath>
#include <crates/bindings.hpp>
#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/generics/common.hpp>
#include <client/cssom/values/generics/length.hpp>
#include <client/cssom/values/computed/common.hpp>
#include <client/cssom/values/computed/length_percentage.hpp>

namespace client_cssom::values::computed
{
  // An alias of computed `<length>` value.
  using Length = CSSPixelLength;

  // Either a computed `<length>` or the `auto` keyword.
  using LengthOrAuto = generics::GenericLengthPercentageOrAuto<Length>;

  // The non-negative length percentage value.
  class NonNegativeLength : public generics::NonNegative<Length>
  {
  public:
    NonNegativeLength(float px)
        : generics::NonNegative<Length>(std::max(0.0f, px))
    {
    }

  public:
    inline NonNegativeLength clamp() const
    {
      return px() < 0.0f ? NonNegativeLength(0.0f) : *this;
    }
  };

  // A computed value for `min-width`, `min-height`, `width` or `height` property.
  class Size : public generics::GenericSize<Size, NonNegativeLengthPercentage>,
               public ToLayoutValue<crates::layout2::styles::Dimension>
  {
    using generics::GenericSize<Size, NonNegativeLengthPercentage>::GenericSize;

  public:
    // Returns the computed value as a `Dimension` object.
    crates::layout2::styles::Dimension toLayoutValue() const override
    {
      if (isAuto())
        return crates::layout2::styles::Dimension::Auto();

      if (isLengthPercentage())
      {
        const auto &lp = lengthPercent();
        if (lp.isLength())
          return crates::layout2::styles::Dimension::Length(lp.getLength().px());
        else if (lp.isPercentage())
          return crates::layout2::styles::Dimension::Percentage(lp.getPercentage().value());
        // TODO(yorkie): support calc() value?
      }
      return crates::layout2::styles::Dimension::Auto();
    }
  };

  // A computed value for `max-width` or `max-height` property.
  class MaxSize : public generics::GenericMaxSize<MaxSize, NonNegativeLengthPercentage>,
                  public ToLayoutValue<crates::layout2::styles::Dimension>
  {
    using generics::GenericMaxSize<MaxSize, NonNegativeLengthPercentage>::GenericMaxSize;

  public:
    // Returns the computed value as a `Dimension` object.
    crates::layout2::styles::Dimension toLayoutValue() const override
    {
      if (isNone())
        return crates::layout2::styles::Dimension::Auto();

      if (isLengthPercentage())
      {
        const auto &lp = lengthPercent();
        if (lp.isLength())
          return crates::layout2::styles::Dimension::Length(lp.getLength().px());
        else if (lp.isPercentage())
          return crates::layout2::styles::Dimension::Percentage(lp.getPercentage().value());
        // TODO(yorkie): support calc() value?
      }
      return crates::layout2::styles::Dimension::Auto();
    }
  };

  // A computed type for `margin` properties.
  class MarginSize : public generics::GenericMargin<MarginSize, LengthPercentage>,
                     public ToLayoutValue<crates::layout2::styles::LengthPercentageAuto>
  {
    using generics::GenericMargin<MarginSize, computed::LengthPercentage>::GenericMargin;

  public:
    // Returns the computed value as a `LengthPercentageAuto` object.
    crates::layout2::styles::LengthPercentageAuto toLayoutValue() const override
    {
      if (isAuto())
        return crates::layout2::styles::LengthPercentageAuto::Auto();

      if (isLengthPercentage())
      {
        const auto &lp = lengthPercent();
        if (lp.isLength())
          return crates::layout2::styles::LengthPercentageAuto::Length(lp.getLength().px());
        else if (lp.isPercentage())
          return crates::layout2::styles::LengthPercentageAuto::Percentage(lp.getPercentage().value());
        // TODO(yorkie): support calc() value?
      }
      return crates::layout2::styles::LengthPercentageAuto::Auto();
    }
  };
}
