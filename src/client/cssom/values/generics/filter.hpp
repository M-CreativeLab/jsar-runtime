#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <client/cssom/style_traits.hpp>

namespace client_cssom::values::generics
{
  template <typename T>
  class GenericFilterFunction : public ToCss
  {
  protected:
    enum Tag : uint8_t
    {
      kNone = 0,
      kBlur,
      kBrightness,
      kContrast,
      kDropShadow,
      kGrayscale,
      kHueRotate,
      kInvert,
      kOpacity,
      kSaturate,
      kSepia,
    };

  public:
    static T None()
    {
      return T(kNone);
    }
    static T Blur()
    {
      return T(kBlur);
    }
    static T Brightness()
    {
      return T(kBrightness);
    }
    static T Contrast()
    {
      return T(kContrast);
    }
    static T DropShadow()
    {
      return T(kDropShadow);
    }
    static T Grayscale()
    {
      return T(kGrayscale);
    }
    static T HueRotate()
    {
      return T(kHueRotate);
    }
    static T Invert()
    {
      return T(kInvert);
    }
    static T Opacity()
    {
      return T(kOpacity);
    }
    static T Saturate()
    {
      return T(kSaturate);
    }
    static T Sepia()
    {
      return T(kSepia);
    }

  protected:
    GenericFilterFunction(Tag tag)
        : tag_(tag)
    {
    }

  public:
    GenericFilterFunction()
        : tag_(kNone)
    {
    }
    inline bool isNone() const
    {
      return tag_ == kNone;
    }
    inline bool isBlur() const
    {
      return tag_ == kBlur;
    }
    inline bool isBrightness() const
    {
      return tag_ == kBrightness;
    }
    inline bool isContrast() const
    {
      return tag_ == kContrast;
    }
    inline bool isDropShadow() const
    {
      return tag_ == kDropShadow;
    }
    inline bool isGrayscale() const
    {
      return tag_ == kGrayscale;
    }
    inline bool isHueRotate() const
    {
      return tag_ == kHueRotate;
    }
    inline bool isInvert() const
    {
      return tag_ == kInvert;
    }
    inline bool isOpacity() const
    {
      return tag_ == kOpacity;
    }
    inline bool isSaturate() const
    {
      return tag_ == kSaturate;
    }
    inline bool isSepia() const
    {
      return tag_ == kSepia;
    }

    std::string toCss() const override
    {
      switch (tag_)
      {
      case kNone:
        return "none";
      case kBlur:
        return "blur";
      case kBrightness:
        return "brightness";
      case kContrast:
        return "contrast";
      case kDropShadow:
        return "drop-shadow";
      case kGrayscale:
        return "grayscale";
      case kHueRotate:
        return "hue-rotate";
      case kInvert:
        return "invert";
      case kOpacity:
        return "opacity";
      case kSaturate:
        return "saturate";
      case kSepia:
        return "sepia";
      default:
        return "none";
      }
    }

  protected:
    Tag tag_;
  };

  template <typename T>
  class GenericFilter : public ToCss
  {
  public:
    GenericFilter()
        : is_none_(true)
    {
    }

    static T None()
    {
      T filter;
      filter.is_none_ = true;
      return filter;
    }

    inline bool isNone() const
    {
      return is_none_;
    }

    std::string toCss() const override
    {
      if (is_none_)
        return "none";

      // TODO: Implement proper CSS serialization for filter functions
      return "none";
    }

  protected:
    bool is_none_;
    // TODO: Add vector of filter functions when needed
  };
}