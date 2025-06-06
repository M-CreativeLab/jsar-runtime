#pragma once

#include <client/cssom/values/generics/rect.hpp>
#include <client/cssom/values/generics/position.hpp>
#include <client/cssom/values/computed/length_percentage.hpp>

namespace client_cssom::values::computed
{
  using PositionType = generics::PositionType;

  class PositionComponent : public LengthPercentage,
                            public generics::PositionComponent
  {
    using LengthPercentage::LengthPercentage;

  public:
    inline bool isCenter() const
    {
      const auto &percentage = toPercentage();
      return percentage.has_value() && percentage->value() == 0.5f;
    }
  };

  using HorizontalPosition = PositionComponent;
  using VerticalPosition = PositionComponent;

  class Position : public generics::GenericPosition<HorizontalPosition, VerticalPosition>
  {
    // TODO(yorkie): add methods?
  };

  using PositionOrAuto = generics::GenericPositionOrAuto<Position>;

  class InsetSize : public generics::GenericInset<InsetSize, LengthPercentage>,
                    public ToLayoutValue<crates::layout2::styles::LengthPercentageAuto>
  {
    using generics::GenericInset<InsetSize, computed::LengthPercentage>::GenericInset;

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

  class Inset : public generics::Rect<InsetSize>
  {
  public:
    Inset(generics::Rect<InsetSize> rect)
        : generics::Rect<InsetSize>(rect)
    {
    }
  };
}
