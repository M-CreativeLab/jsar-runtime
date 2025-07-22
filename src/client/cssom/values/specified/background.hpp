#pragma once

#include <client/cssom/values/generics/background.hpp>
#include <client/cssom/values/computed/background.hpp>

namespace client_cssom::values::specified
{
  class BackgroundBlendMode : public generics::GenericBackgroundBlendMode<BackgroundBlendMode>,
                              public Parse,
                              public ToComputedValue<computed::BackgroundBlendMode>
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

    computed::BackgroundBlendMode toComputedValue(computed::Context &) const override
    {
      if (isNormal())
        return computed::BackgroundBlendMode::Normal();
      else if (isMultiply())
        return computed::BackgroundBlendMode::Multiply();
      else if (isScreen())
        return computed::BackgroundBlendMode::Screen();
      else if (isOverlay())
        return computed::BackgroundBlendMode::Overlay();
      else if (isDarken())
        return computed::BackgroundBlendMode::Darken();
      else if (isLighten())
        return computed::BackgroundBlendMode::Lighten();
      else if (isColorDodge())
        return computed::BackgroundBlendMode::ColorDodge();
      else if (isColorBurn())
        return computed::BackgroundBlendMode::ColorBurn();
      else if (isHardLight())
        return computed::BackgroundBlendMode::HardLight();
      else if (isSoftLight())
        return computed::BackgroundBlendMode::SoftLight();
      else if (isDifference())
        return computed::BackgroundBlendMode::Difference();
      else if (isExclusion())
        return computed::BackgroundBlendMode::Exclusion();
      else if (isHue())
        return computed::BackgroundBlendMode::Hue();
      else if (isSaturation())
        return computed::BackgroundBlendMode::Saturation();
      else if (isColor())
        return computed::BackgroundBlendMode::Color();
      else if (isLuminosity())
        return computed::BackgroundBlendMode::Luminosity();

      // Default to Normal if none match
      return computed::BackgroundBlendMode::Normal();
    }
  };

  class BackgroundClip : public generics::GenericBackgroundClip<BackgroundClip>,
                         public Parse,
                         public ToComputedValue<computed::BackgroundClip>
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

    computed::BackgroundClip toComputedValue(computed::Context &) const override
    {
      if (isBorderBox())
        return computed::BackgroundClip::BorderBox();
      else if (isPaddingBox())
        return computed::BackgroundClip::PaddingBox();
      else if (isContentBox())
        return computed::BackgroundClip::ContentBox();
      else if (isText())
        return computed::BackgroundClip::Text();

      // Default to BorderBox if none match
      return computed::BackgroundClip::BorderBox();
    }
  };
}
