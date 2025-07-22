#pragma once

#include <client/cssom/values/generics/background.hpp>

namespace client_cssom::values::specified
{
  class BackgroundBlendMode : public generics::GenericBackgroundBlendMode<BackgroundBlendMode>
  {
    friend class Parse;

  public:
    using generics::GenericBackgroundBlendMode<BackgroundBlendMode>::GenericBackgroundBlendMode;

    bool parse(const std::string &input) override
    {
      if (input == "normal")
        tag_ = kNormal;
      else if (input == "multiply")
        tag_ = kMultiply;
      else if (input == "screen")
        tag_ = kScreen;
      else if (input == "overlay")
        tag_ = kOverlay;
      else if (input == "darken")
        tag_ = kDarken;
      else if (input == "lighten")
        tag_ = kLighten;
      else if (input == "color-dodge")
        tag_ = kColorDodge;
      else if (input == "color-burn")
        tag_ = kColorBurn;
      else if (input == "hard-light")
        tag_ = kHardLight;
      else if (input == "soft-light")
        tag_ = kSoftLight;
      else if (input == "difference")
        tag_ = kDifference;
      else if (input == "exclusion")
        tag_ = kExclusion;
      else if (input == "hue")
        tag_ = kHue;
      else if (input == "saturation")
        tag_ = kSaturation;
      else if (input == "color")
        tag_ = kColor;
      else if (input == "luminosity")
        tag_ = kLuminosity;
      else
        return false;
      return true;
    }
  };

  class BackgroundClip : public generics::GenericBackgroundClip<BackgroundClip>
  {
    friend class Parse;

  public:
    using generics::GenericBackgroundClip<BackgroundClip>::GenericBackgroundClip;

    bool parse(const std::string &input) override
    {
      if (input == "border-box")
        tag_ = kBorderBox;
      else if (input == "padding-box")
        tag_ = kPaddingBox;
      else if (input == "content-box")
        tag_ = kContentBox;
      else if (input == "text")
        tag_ = kText;
      else
        return false;
      return true;
    }
  };
}