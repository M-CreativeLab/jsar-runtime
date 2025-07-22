#pragma once

#include <cstdint>
#include <string>
#include <client/cssom/style_traits.hpp>

namespace client_cssom::values::generics
{
  template <typename T>
  class GenericBackgroundBlendMode : public Parse,
                                     public ToCss,
                                     public ToComputedValue<T>
  {
    friend class Parse;

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

    T toComputedValue(computed::Context &) const override
    {
      return static_cast<const T &>(*this);
    }

    inline bool isNormal() const
    {
      return tag_ == kNormal;
    }

  protected:
    Tag tag_;
  };

  template <typename T>
  class GenericBackgroundClip : public Parse,
                                public ToCss,
                                public ToComputedValue<T>
  {
    friend class Parse;

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
      return true;
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

    T toComputedValue(computed::Context &) const override
    {
      return static_cast<const T &>(*this);
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