#pragma once

#include <client/cssom/values/generics/background.hpp>
#include <skia/include/core/SkBlendMode.h>

namespace client_cssom::values::specified
{
  class BackgroundBlendMode;
  class BackgroundClip;
  class BackgroundOrigin;
  class BackgroundRepeat;
}

namespace client_cssom::values::computed
{
  class BackgroundBlendMode : public generics::GenericBackgroundBlendMode<BackgroundBlendMode>
  {
    using generics::GenericBackgroundBlendMode<BackgroundBlendMode>::GenericBackgroundBlendMode;

  public:
    operator SkBlendMode() const
    {
      switch (tag_)
      {
      case BackgroundBlendMode::kNormal:
        return SkBlendMode::kSrcOver;
      case BackgroundBlendMode::kMultiply:
        return SkBlendMode::kMultiply;
      case BackgroundBlendMode::kScreen:
        return SkBlendMode::kScreen;
      case BackgroundBlendMode::kOverlay:
        return SkBlendMode::kOverlay;
      case BackgroundBlendMode::kDarken:
        return SkBlendMode::kDarken;
      case BackgroundBlendMode::kLighten:
        return SkBlendMode::kLighten;
      case BackgroundBlendMode::kColorDodge:
        return SkBlendMode::kColorDodge;
      case BackgroundBlendMode::kColorBurn:
        return SkBlendMode::kColorBurn;
      case BackgroundBlendMode::kHardLight:
        return SkBlendMode::kHardLight;
      case BackgroundBlendMode::kSoftLight:
        return SkBlendMode::kSoftLight;
      case BackgroundBlendMode::kDifference:
        return SkBlendMode::kDifference;
      case BackgroundBlendMode::kExclusion:
        return SkBlendMode::kExclusion;
      case BackgroundBlendMode::kHue:
        return SkBlendMode::kHue;
      case BackgroundBlendMode::kSaturation:
        return SkBlendMode::kSaturation;
      case BackgroundBlendMode::kColor:
        return SkBlendMode::kColor;
      case BackgroundBlendMode::kLuminosity:
        return SkBlendMode::kLuminosity;
      default:
        return SkBlendMode::kSrcOver; // Default to SrcOver if none match
      }
    }
  };

  class BackgroundClip : public generics::GenericBackgroundClip<BackgroundClip>
  {
    using generics::GenericBackgroundClip<BackgroundClip>::GenericBackgroundClip;
  };

  class BackgroundOrigin : public generics::GenericBackgroundOrigin<BackgroundOrigin>
  {
    using generics::GenericBackgroundOrigin<BackgroundOrigin>::GenericBackgroundOrigin;
  };

  class BackgroundRepeat : public generics::GenericBackgroundRepeat<BackgroundRepeat>
  {
    using generics::GenericBackgroundRepeat<BackgroundRepeat>::GenericBackgroundRepeat;
  };
}
