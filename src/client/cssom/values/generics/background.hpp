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

  template <typename T>
  class GenericBackgroundSize : public ToCss
  {
  protected:
    enum Tag : uint8_t
    {
      kAuto = 0,
      kLength,
      kPercentage,
      kCover,
      kContain,
      kTwoValues,
    };

  public:
    static T Auto()
    {
      return T(kAuto);
    }
    static T Length(float value)
    {
      return T(kLength, value);
    }
    static T Percentage(float value)
    {
      return T(kPercentage, value);
    }
    static T Cover()
    {
      return T(kCover);
    }
    static T Contain()
    {
      return T(kContain);
    }
    static T TwoValues(float width, float height)
    {
      return T(kTwoValues, width, height);
    }

  public:
    GenericBackgroundSize()
        : tag_(kAuto)
        , width_(0.0f)
        , height_(0.0f)
    {
    }

  protected:
    GenericBackgroundSize(Tag tag, float width = 0.0f, float height = 0.0f)
        : tag_(tag)
        , width_(width)
        , height_(height)
    {
    }

  public:
    std::string toCss() const override
    {
      switch (tag_)
      {
      case kAuto:
        return "auto";
      case kLength:
        return std::to_string(width_) + "px";
      case kPercentage:
        return std::to_string(width_) + "%";
      case kCover:
        return "cover";
      case kContain:
        return "contain";
      case kTwoValues:
        return std::to_string(width_) + "px " + std::to_string(height_) + "px";
      }
      return "";
    }

    inline bool isAuto() const
    {
      return tag_ == kAuto;
    }
    inline bool isLength() const
    {
      return tag_ == kLength;
    }
    inline bool isPercentage() const
    {
      return tag_ == kPercentage;
    }
    inline bool isCover() const
    {
      return tag_ == kCover;
    }
    inline bool isContain() const
    {
      return tag_ == kContain;
    }
    inline bool isTwoValues() const
    {
      return tag_ == kTwoValues;
    }

    inline float getWidth() const
    {
      return width_;
    }
    inline float getHeight() const
    {
      return height_;
    }

  protected:
    Tag tag_;
    float width_;
    float height_;
  };

  template <typename T>
  class GenericBackgroundPosition : public ToCss
  {
  protected:
    enum Tag : uint8_t
    {
      kCenter = 0,
      kLeft,
      kRight,
      kTop,
      kBottom,
      kLength,
      kPercentage,
      kTwoValues,
      kThreeValues,
      kFourValues,
    };

    enum Keyword : uint8_t
    {
      kNone = 0,
      kCenterKeyword,
      kLeftKeyword,
      kRightKeyword,
      kTopKeyword,
      kBottomKeyword,
    };

  public:
    static T Center()
    {
      return T(kCenter);
    }
    static T Left()
    {
      return T(kLeft);
    }
    static T Right()
    {
      return T(kRight);
    }
    static T Top()
    {
      return T(kTop);
    }
    static T Bottom()
    {
      return T(kBottom);
    }
    static T Length(float value)
    {
      return T(kLength, value);
    }
    static T Percentage(float value)
    {
      return T(kPercentage, value);
    }
    static T TwoValues(float x, float y)
    {
      return T(kTwoValues, x, y);
    }
    static T ThreeValues(Keyword hKeyword, float hOffset, Keyword vKeyword)
    {
      return T(kThreeValues, 0.0f, 0.0f, hKeyword, hOffset, vKeyword, 0.0f);
    }
    static T FourValues(Keyword hKeyword, float hOffset, Keyword vKeyword, float vOffset)
    {
      return T(kFourValues, 0.0f, 0.0f, hKeyword, hOffset, vKeyword, vOffset);
    }

  public:
    GenericBackgroundPosition()
        : tag_(kCenter)
        , x_(0.0f)
        , y_(0.0f)
        , h_keyword_(kNone)
        , h_offset_(0.0f)
        , v_keyword_(kNone)
        , v_offset_(0.0f)
    {
    }

  protected:
    GenericBackgroundPosition(Tag tag, float x = 0.0f, float y = 0.0f, Keyword h_keyword = kNone, float h_offset = 0.0f, Keyword v_keyword = kNone, float v_offset = 0.0f)
        : tag_(tag)
        , x_(x)
        , y_(y)
        , h_keyword_(h_keyword)
        , h_offset_(h_offset)
        , v_keyword_(v_keyword)
        , v_offset_(v_offset)
    {
    }

  public:
    std::string toCss() const override
    {
      switch (tag_)
      {
      case kCenter:
        return "center";
      case kLeft:
        return "left";
      case kRight:
        return "right";
      case kTop:
        return "top";
      case kBottom:
        return "bottom";
      case kLength:
        return std::to_string(x_) + "px";
      case kPercentage:
        return std::to_string(x_) + "%";
      case kTwoValues:
        return std::to_string(x_) + "px " + std::to_string(y_) + "px";
      case kThreeValues:
        return keywordToString(h_keyword_) + " " + std::to_string(h_offset_) + "px " + keywordToString(v_keyword_);
      case kFourValues:
        return keywordToString(h_keyword_) + " " + std::to_string(h_offset_) + "px " +
               keywordToString(v_keyword_) + " " + std::to_string(v_offset_) + "px";
      }
      return "";
    }

  private:
    std::string keywordToString(Keyword keyword) const
    {
      switch (keyword)
      {
      case kCenterKeyword:
        return "center";
      case kLeftKeyword:
        return "left";
      case kRightKeyword:
        return "right";
      case kTopKeyword:
        return "top";
      case kBottomKeyword:
        return "bottom";
      case kNone:
      default:
        return "";
      }
    }

    inline bool isCenter() const
    {
      return tag_ == kCenter;
    }
    inline bool isLeft() const
    {
      return tag_ == kLeft;
    }
    inline bool isRight() const
    {
      return tag_ == kRight;
    }
    inline bool isTop() const
    {
      return tag_ == kTop;
    }
    inline bool isBottom() const
    {
      return tag_ == kBottom;
    }
    inline bool isLength() const
    {
      return tag_ == kLength;
    }
    inline bool isPercentage() const
    {
      return tag_ == kPercentage;
    }
    inline bool isTwoValues() const
    {
      return tag_ == kTwoValues;
    }
    inline bool isThreeValues() const
    {
      return tag_ == kThreeValues;
    }
    inline bool isFourValues() const
    {
      return tag_ == kFourValues;
    }

    inline float getX() const
    {
      return x_;
    }
    inline float getY() const
    {
      return y_;
    }
    inline Keyword getHorizontalKeyword() const
    {
      return h_keyword_;
    }
    inline float getHorizontalOffset() const
    {
      return h_offset_;
    }
    inline Keyword getVerticalKeyword() const
    {
      return v_keyword_;
    }
    inline float getVerticalOffset() const
    {
      return v_offset_;
    }

  protected:
    Tag tag_;
    float x_;
    float y_;
    Keyword h_keyword_;
    float h_offset_;
    Keyword v_keyword_;
    float v_offset_;
  };
}
