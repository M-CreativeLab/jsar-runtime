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

  template <typename T>
  class GenericBackgroundOrigin : public ToCss
  {
  protected:
    enum Tag : uint8_t
    {
      kPaddingBox = 0,
      kBorderBox,
      kContentBox,
    };

  public:
    static T PaddingBox()
    {
      return T(kPaddingBox);
    }
    static T BorderBox()
    {
      return T(kBorderBox);
    }
    static T ContentBox()
    {
      return T(kContentBox);
    }

  public:
    GenericBackgroundOrigin()
        : tag_(kPaddingBox)
    {
    }

  protected:
    GenericBackgroundOrigin(Tag tag)
        : tag_(tag)
    {
    }

  public:
    std::string toCss() const override
    {
      switch (tag_)
      {
      case kPaddingBox:
        return "padding-box";
      case kBorderBox:
        return "border-box";
      case kContentBox:
        return "content-box";
      }
      return "";
    }

    inline bool isPaddingBox() const
    {
      return tag_ == kPaddingBox;
    }
    inline bool isBorderBox() const
    {
      return tag_ == kBorderBox;
    }
    inline bool isContentBox() const
    {
      return tag_ == kContentBox;
    }

  protected:
    Tag tag_;
  };

  template <typename T>
  class GenericBackgroundRepeat : public ToCss
  {
  protected:
    enum Tag : uint8_t
    {
      kRepeat = 0,
      kRepeatX,
      kRepeatY,
      kNoRepeat,
      kSpace,
      kRound,
    };

  public:
    static T Repeat()
    {
      return T(kRepeat);
    }
    static T RepeatX()
    {
      return T(kRepeatX);
    }
    static T RepeatY()
    {
      return T(kRepeatY);
    }
    static T NoRepeat()
    {
      return T(kNoRepeat);
    }
    static T Space()
    {
      return T(kSpace);
    }
    static T Round()
    {
      return T(kRound);
    }

  public:
    GenericBackgroundRepeat()
        : tag_(kRepeat)
    {
    }

  protected:
    GenericBackgroundRepeat(Tag tag)
        : tag_(tag)
    {
    }

  public:
    std::string toCss() const override
    {
      switch (tag_)
      {
      case kRepeat:
        return "repeat";
      case kRepeatX:
        return "repeat-x";
      case kRepeatY:
        return "repeat-y";
      case kNoRepeat:
        return "no-repeat";
      case kSpace:
        return "space";
      case kRound:
        return "round";
      }
      return "";
    }

    inline bool isRepeat() const
    {
      return tag_ == kRepeat;
    }
    inline bool isRepeatX() const
    {
      return tag_ == kRepeatX;
    }
    inline bool isRepeatY() const
    {
      return tag_ == kRepeatY;
    }
    inline bool isNoRepeat() const
    {
      return tag_ == kNoRepeat;
    }
    inline bool isSpace() const
    {
      return tag_ == kSpace;
    }
    inline bool isRound() const
    {
      return tag_ == kRound;
    }

  protected:
    Tag tag_;
  };

  // bg-size = [ <length-percentage> | auto ]{1,2} | cover | contain
  template <typename T>
  class GenericBackgroundSize : public ToCss
  {
  protected:
    enum Tag : uint8_t
    {
      // [ <length-percentage> | auto ]{1,2}
      kSingleValue = 0,  // Single <length-percentage> or auto
      kTwoValues,        // Two <length-percentage> or auto values
      // Keywords
      kCover,
      kContain,
    };

  public:
    static T Cover()
    {
      return T(kCover);
    }
    static T Contain()
    {
      return T(kContain);
    }

  public:
    GenericBackgroundSize()
        : tag_(kSingleValue)  // Default to single auto value
    {
    }

  protected:
    GenericBackgroundSize(Tag tag)
        : tag_(tag)
    {
    }

  public:
    std::string toCss() const override
    {
      switch (tag_)
      {
      case kCover:
        return "cover";
      case kContain:
        return "contain";
      case kSingleValue:
      case kTwoValues:
        return ""; // Will be overridden by derived classes
      }
      return "";
    }

    inline bool isCover() const
    {
      return tag_ == kCover;
    }
    inline bool isContain() const
    {
      return tag_ == kContain;
    }
    inline bool isSingleValue() const
    {
      return tag_ == kSingleValue;
    }
    inline bool isTwoValues() const
    {
      return tag_ == kTwoValues;
    }

  protected:
    Tag tag_;
  };

  // bg-position = [
  //   [ left | center | right | top | bottom | <length-percentage> ]
  // |
  //   [ left | center | right | <length-percentage> ]
  //   [ top | center | bottom | <length-percentage> ]
  // |
  //   [ center | [ left | right ] <length-percentage>? ] &&
  //   [ center | [ top | bottom ] <length-percentage>? ]
  // ]
  template <typename T>
  class GenericBackgroundPosition : public ToCss
  {
  protected:
    enum Tag : uint8_t
    {
      // Single value: [ left | center | right | top | bottom | <length-percentage> ]
      kSingleKeyword = 0,       // Single keyword (left, center, right, top, bottom)
      kSingleLengthPercentage,  // Single <length-percentage>
      
      // Two value: [ left | center | right | <length-percentage> ] [ top | center | bottom | <length-percentage> ]
      kTwoValues,               // Two values (keyword/length-percentage combinations)
      
      // Four value edge offsets: [ center | [ left | right ] <length-percentage>? ] && [ center | [ top | bottom ] <length-percentage>? ]
      kEdgeOffsets,             // Edge offsets like "right 10px bottom 20px"
    };

  public:
    static T Left()
    {
      return T(kSingleKeyword);
    }
    static T Center()
    {
      return T(kSingleKeyword);
    }
    static T Right()
    {
      return T(kSingleKeyword);
    }
    static T Top()
    {
      return T(kSingleKeyword);
    }
    static T Bottom()
    {
      return T(kSingleKeyword);
    }

  public:
    GenericBackgroundPosition()
        : tag_(kSingleKeyword)  // Default to center
    {
    }

  protected:
    GenericBackgroundPosition(Tag tag)
        : tag_(tag)
    {
    }

  public:
    std::string toCss() const override
    {
      switch (tag_)
      {
      case kSingleKeyword:
      case kSingleLengthPercentage:
      case kTwoValues:
      case kEdgeOffsets:
        return ""; // Will be overridden by derived classes
      }
      return "";
    }

    inline bool isSingleKeyword() const
    {
      return tag_ == kSingleKeyword;
    }
    inline bool isSingleLengthPercentage() const
    {
      return tag_ == kSingleLengthPercentage;
    }
    inline bool isTwoValues() const
    {
      return tag_ == kTwoValues;
    }
    inline bool isEdgeOffsets() const
    {
      return tag_ == kEdgeOffsets;
    }

  protected:
    Tag tag_;
  };
}
