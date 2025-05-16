#pragma once

#include <client/cssom/values/computed/length_percentage.hpp>
#include <client/cssom/values/generics/position.hpp>
#include <client/cssom/values/specified/position.hpp>

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
}
