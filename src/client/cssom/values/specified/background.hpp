#pragma once

#include <string>
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

  class BackgroundOrigin : public generics::GenericBackgroundOrigin<BackgroundOrigin>,
                           public Parse,
                           public ToComputedValue<computed::BackgroundOrigin>
  {
    friend class Parse;

  public:
    using generics::GenericBackgroundOrigin<BackgroundOrigin>::GenericBackgroundOrigin;

    bool parse(const std::string &input) override
    {
      if (input == "padding-box")
        tag_ = kPaddingBox;
      else if (input == "border-box")
        tag_ = kBorderBox;
      else if (input == "content-box")
        tag_ = kContentBox;
      else
        return false;
      return true;
    }

    computed::BackgroundOrigin toComputedValue(computed::Context &) const override
    {
      if (isPaddingBox())
        return computed::BackgroundOrigin::PaddingBox();
      else if (isBorderBox())
        return computed::BackgroundOrigin::BorderBox();
      else if (isContentBox())
        return computed::BackgroundOrigin::ContentBox();

      // Default to PaddingBox if none match
      return computed::BackgroundOrigin::PaddingBox();
    }
  };

  class BackgroundRepeat : public generics::GenericBackgroundRepeat<BackgroundRepeat>,
                           public Parse,
                           public ToComputedValue<computed::BackgroundRepeat>
  {
    friend class Parse;

  public:
    using generics::GenericBackgroundRepeat<BackgroundRepeat>::GenericBackgroundRepeat;

    bool parse(const std::string &input) override
    {
      if (input == "repeat")
        tag_ = kRepeat;
      else if (input == "repeat-x")
        tag_ = kRepeatX;
      else if (input == "repeat-y")
        tag_ = kRepeatY;
      else if (input == "no-repeat")
        tag_ = kNoRepeat;
      else if (input == "space")
        tag_ = kSpace;
      else if (input == "round")
        tag_ = kRound;
      else
        return false;
      return true;
    }

    computed::BackgroundRepeat toComputedValue(computed::Context &) const override
    {
      if (isRepeat())
        return computed::BackgroundRepeat::Repeat();
      else if (isRepeatX())
        return computed::BackgroundRepeat::RepeatX();
      else if (isRepeatY())
        return computed::BackgroundRepeat::RepeatY();
      else if (isNoRepeat())
        return computed::BackgroundRepeat::NoRepeat();
      else if (isSpace())
        return computed::BackgroundRepeat::Space();
      else if (isRound())
        return computed::BackgroundRepeat::Round();

      // Default to Repeat if none match
      return computed::BackgroundRepeat::Repeat();
    }
  };

  class BackgroundSize : public generics::GenericBackgroundSize<BackgroundSize>,
                         public Parse,
                         public ToComputedValue<computed::BackgroundSize>
  {
    friend class Parse;

  public:
    using generics::GenericBackgroundSize<BackgroundSize>::GenericBackgroundSize;

    bool parse(const std::string &input) override
    {
      if (input == "auto")
        tag_ = kAuto;
      else if (input == "cover")
        tag_ = kCover;
      else if (input == "contain")
        tag_ = kContain;
      else if (input.find('%') != std::string::npos)
      {
        tag_ = kPercentage;
        width_ = std::stof(input.substr(0, input.find('%')));
      }
      else if (input.find("px") != std::string::npos)
      {
        tag_ = kLength;
        width_ = std::stof(input.substr(0, input.find("px")));
      }
      else
        return false;
      return true;
    }

    computed::BackgroundSize toComputedValue(computed::Context &) const override
    {
      if (isAuto())
        return computed::BackgroundSize::Auto();
      else if (isCover())
        return computed::BackgroundSize::Cover();
      else if (isContain())
        return computed::BackgroundSize::Contain();
      else if (isLength())
        return computed::BackgroundSize::Length(width_);
      else if (isPercentage())
        return computed::BackgroundSize::Percentage(width_);
      else if (isTwoValues())
        return computed::BackgroundSize::TwoValues(width_, height_);

      // Default to Auto if none match
      return computed::BackgroundSize::Auto();
    }
  };

  class BackgroundPosition : public generics::GenericBackgroundPosition<BackgroundPosition>,
                             public Parse,
                             public ToComputedValue<computed::BackgroundPosition>
  {
    friend class Parse;

  public:
    using generics::GenericBackgroundPosition<BackgroundPosition>::GenericBackgroundPosition;

    bool parse(const std::string &input) override
    {
      if (input == "center")
        tag_ = kCenter;
      else if (input == "left")
        tag_ = kLeft;
      else if (input == "right")
        tag_ = kRight;
      else if (input == "top")
        tag_ = kTop;
      else if (input == "bottom")
        tag_ = kBottom;
      else if (input.find('%') != std::string::npos)
      {
        tag_ = kPercentage;
        x_ = std::stof(input.substr(0, input.find('%')));
      }
      else if (input.find("px") != std::string::npos)
      {
        tag_ = kLength;
        x_ = std::stof(input.substr(0, input.find("px")));
      }
      else
        return false;
      return true;
    }

    computed::BackgroundPosition toComputedValue(computed::Context &) const override
    {
      if (isCenter())
        return computed::BackgroundPosition::Center();
      else if (isLeft())
        return computed::BackgroundPosition::Left();
      else if (isRight())
        return computed::BackgroundPosition::Right();
      else if (isTop())
        return computed::BackgroundPosition::Top();
      else if (isBottom())
        return computed::BackgroundPosition::Bottom();
      else if (isLength())
        return computed::BackgroundPosition::Length(x_);
      else if (isPercentage())
        return computed::BackgroundPosition::Percentage(x_);
      else if (isTwoValues())
        return computed::BackgroundPosition::TwoValues(x_, y_);

      // Default to Center if none match
      return computed::BackgroundPosition::Center();
    }
  };
}
