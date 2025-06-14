#pragma once

#include <memory>
#include <variant>
#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/generics/calc.hpp>
#include <client/cssom/values/specified/common.hpp>
#include <client/cssom/values/specified/length.hpp>

namespace client_cssom::values::specified
{
  class Leaf
  {
  private:
    enum Tag
    {
      kLength,
      kAngle,
      kTime,
      kResolution,
      kColorComponent,
      kPercentage,
      kNumber,
    };

  private:
    Tag tag_;
  };

  using CalcNode = generics::GenericCalcNode<Leaf>;

  class CalcLengthPercentage
  {
  public:
    CalcLengthPercentage()
        : clamping_mode_(AllowedNumbericType::Default())
        , calc_node_(nullptr)
    {
    }
    CalcLengthPercentage(const CalcLengthPercentage &other)
        : clamping_mode_(other.clamping_mode_)
        , calc_node_(nullptr)
    {
    }

    CalcLengthPercentage operator=(const CalcLengthPercentage &other)
    {
      return *this;
    }

  private:
    AllowedNumbericType clamping_mode_;
    std::unique_ptr<CalcNode> calc_node_ = nullptr;
  };
}
