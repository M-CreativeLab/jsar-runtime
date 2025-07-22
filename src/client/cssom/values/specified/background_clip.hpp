#pragma once

#include <client/cssom/values/generics/background.hpp>

namespace client_cssom::values::specified
{
  class BackgroundClip : public generics::GenericBackgroundClip<BackgroundClip>
  {
    using generics::GenericBackgroundClip<BackgroundClip>::GenericBackgroundClip;
  };
}