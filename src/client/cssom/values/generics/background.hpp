#pragma once

#include <cstdint>
#include <string>
#include <client/cssom/style_traits.hpp>

namespace client_cssom::values::generics
{
  template <typename T>
  class GenericBackgroundBlendMode : public ToCss
  {
  protected:
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
    static T Normal()
    {
      return T(kNormal);
    }
    static T Multiply()
    {
      return T(kMultiply);
    }
    static T Screen()
    {
      return T(kScreen);
    }
    static T Overlay()
    {
      return T(kOverlay);
    }
    static T Darken()
    {
      return T(kDarken);
    }
    static T Lighten()
    {
      return T(kLighten);
    }
    static T ColorDodge()
    {
      return T(kColorDodge);
    }
    static T ColorBurn()
    {
      return T(kColorBurn);
    }
    static T HardLight()
    {
      return T(kHardLight);
    }
    static T SoftLight()
    {
      return T(kSoftLight);
    }
    static T Difference()
    {
      return T(kDifference);
    }
    static T Exclusion()
    {
      return T(kExclusion);
    }
    static T Hue()
    {
      return T(kHue);
    }
    static T Saturation()
    {
      return T(kSaturation);
    }
    static T Color()
    {
      return T(kColor);
    }
    static T Luminosity()
    {
      return T(kLuminosity);
    }

  public:
    GenericBackgroundBlendMode()
        : tag_(kNormal)
    {
    }

  protected:
    GenericBackgroundBlendMode(Tag tag)
        : tag_(tag)
    {
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

    inline bool isNormal() const
    {
      return tag_ == kNormal;
    }
    inline bool isMultiply() const
    {
      return tag_ == kMultiply;
    }
    inline bool isScreen() const
    {
      return tag_ == kScreen;
    }
    inline bool isOverlay() const
    {
      return tag_ == kOverlay;
    }
    inline bool isDarken() const
    {
      return tag_ == kDarken;
    }
    inline bool isLighten() const
    {
      return tag_ == kLighten;
    }
    inline bool isColorDodge() const
    {
      return tag_ == kColorDodge;
    }
    inline bool isColorBurn() const
    {
      return tag_ == kColorBurn;
    }
    inline bool isHardLight() const
    {
      return tag_ == kHardLight;
    }
    inline bool isSoftLight() const
    {
      return tag_ == kSoftLight;
    }
    inline bool isDifference() const
    {
      return tag_ == kDifference;
    }
    inline bool isExclusion() const
    {
      return tag_ == kExclusion;
    }
    inline bool isHue() const
    {
      return tag_ == kHue;
    }
    inline bool isSaturation() const
    {
      return tag_ == kSaturation;
    }
    inline bool isColor() const
    {
      return tag_ == kColor;
    }
    inline bool isLuminosity() const
    {
      return tag_ == kLuminosity;
    }

  protected:
    Tag tag_;
  };

  template <typename T>
  class GenericBackgroundClip : public ToCss
  {
  protected:
    enum Tag : uint8_t
    {
      kBorderBox = 0,
      kPaddingBox,
      kContentBox,
      kText,
    };

  public:
    static T BorderBox()
    {
      return T(kBorderBox);
    }
    static T PaddingBox()
    {
      return T(kPaddingBox);
    }
    static T ContentBox()
    {
      return T(kContentBox);
    }
    static T Text()
    {
      return T(kText);
    }

  public:
    GenericBackgroundClip()
        : tag_(kBorderBox)
    {
    }

  protected:
    GenericBackgroundClip(Tag tag)
        : tag_(tag)
    {
    }

  public:
    std::string toCss() const override
    {
      switch (tag_)
      {
      case kBorderBox:
        return "border-box";
      case kPaddingBox:
        return "padding-box";
      case kContentBox:
        return "content-box";
      case kText:
        return "text";
      }
      return "";
    }

    inline bool isBorderBox() const
    {
      return tag_ == kBorderBox;
    }
    inline bool isPaddingBox() const
    {
      return tag_ == kPaddingBox;
    }
    inline bool isContentBox() const
    {
      return tag_ == kContentBox;
    }
    inline bool isText() const
    {
      return tag_ == kText;
    }

  protected:
    Tag tag_;
  };
}
