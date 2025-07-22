#pragma once

#include <cstdint>
#include <client/cssom/style_traits.hpp>

namespace client_cssom::values::specified
{
  class BackgroundBlendMode : public Parse,
                              public ToCss,
                              public ToComputedValue<BackgroundBlendMode>
  {
    friend class Parse;

  private:
    enum Tag : uint8_t
    {
      kNormal = 0,
      kMultiply,
      kScreen,
      kOverlay,
      kDarken,
      kLighten,
      kColorDodge,
      kColorBurn,
      kHardLight,
      kSoftLight,
      kDifference,
      kExclusion,
      kHue,
      kSaturation,
      kColor,
      kLuminosity,
    };

  public:
    static BackgroundBlendMode Normal()
    {
      return BackgroundBlendMode(kNormal);
    }
    static BackgroundBlendMode Multiply()
    {
      return BackgroundBlendMode(kMultiply);
    }
    static BackgroundBlendMode Screen()
    {
      return BackgroundBlendMode(kScreen);
    }
    static BackgroundBlendMode Overlay()
    {
      return BackgroundBlendMode(kOverlay);
    }
    static BackgroundBlendMode Darken()
    {
      return BackgroundBlendMode(kDarken);
    }
    static BackgroundBlendMode Lighten()
    {
      return BackgroundBlendMode(kLighten);
    }
    static BackgroundBlendMode ColorDodge()
    {
      return BackgroundBlendMode(kColorDodge);
    }
    static BackgroundBlendMode ColorBurn()
    {
      return BackgroundBlendMode(kColorBurn);
    }
    static BackgroundBlendMode HardLight()
    {
      return BackgroundBlendMode(kHardLight);
    }
    static BackgroundBlendMode SoftLight()
    {
      return BackgroundBlendMode(kSoftLight);
    }
    static BackgroundBlendMode Difference()
    {
      return BackgroundBlendMode(kDifference);
    }
    static BackgroundBlendMode Exclusion()
    {
      return BackgroundBlendMode(kExclusion);
    }
    static BackgroundBlendMode Hue()
    {
      return BackgroundBlendMode(kHue);
    }
    static BackgroundBlendMode Saturation()
    {
      return BackgroundBlendMode(kSaturation);
    }
    static BackgroundBlendMode Color()
    {
      return BackgroundBlendMode(kColor);
    }
    static BackgroundBlendMode Luminosity()
    {
      return BackgroundBlendMode(kLuminosity);
    }

  public:
    BackgroundBlendMode()
        : tag_(kNormal)
    {
    }

  private:
    BackgroundBlendMode(Tag tag)
        : tag_(tag)
    {
    }

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
      return true;
    }

  public:
    std::string toCss() const override
    {
      switch (tag_)
      {
      case kNormal:
        return "normal";
      case kMultiply:
        return "multiply";
      case kScreen:
        return "screen";
      case kOverlay:
        return "overlay";
      case kDarken:
        return "darken";
      case kLighten:
        return "lighten";
      case kColorDodge:
        return "color-dodge";
      case kColorBurn:
        return "color-burn";
      case kHardLight:
        return "hard-light";
      case kSoftLight:
        return "soft-light";
      case kDifference:
        return "difference";
      case kExclusion:
        return "exclusion";
      case kHue:
        return "hue";
      case kSaturation:
        return "saturation";
      case kColor:
        return "color";
      case kLuminosity:
        return "luminosity";
      }
      return "";
    }
    BackgroundBlendMode toComputedValue(computed::Context &) const override
    {
      return *this;
    }

    inline bool isNormal() const
    {
      return tag_ == kNormal;
    }

  private:
    Tag tag_;
  };
}