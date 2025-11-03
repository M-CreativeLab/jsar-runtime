#pragma once

#include <client/cssom/values/generics/easing.hpp>

namespace endor
{
  namespace client_cssom::values::computed
  {
    class TimingFunction : public generics::GenericTimingFunction<TimingFunction>
    {
      using generics::GenericTimingFunction<TimingFunction>::GenericTimingFunction;
    };
  }
} // namespace endor
