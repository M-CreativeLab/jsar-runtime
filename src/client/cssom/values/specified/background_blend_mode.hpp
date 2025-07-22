#pragma once

#include <client/cssom/values/generics/background.hpp>

namespace client_cssom::values::specified
{
  class BackgroundBlendMode : public generics::GenericBackgroundBlendMode<BackgroundBlendMode>
  {
    using generics::GenericBackgroundBlendMode<BackgroundBlendMode>::GenericBackgroundBlendMode;
  };
}