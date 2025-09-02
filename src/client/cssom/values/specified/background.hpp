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
      // Tokenize the input by splitting on spaces
      std::vector<std::string> tokens = tokenize(input);

      if (tokens.empty())
        return false;

      // Handle different syntax variations
      if (tokens.size() == 1)
        return parseOneValue(tokens[0]);
      else if (tokens.size() == 2)
        return parseTwoValues(tokens[0], tokens[1]);
      else if (tokens.size() == 3)
        return parseThreeValues(tokens[0], tokens[1], tokens[2]);
      else if (tokens.size() == 4)
        return parseFourValues(tokens[0], tokens[1], tokens[2], tokens[3]);
      else
        return false;
    }

  private:
    std::vector<std::string> tokenize(const std::string &input)
    {
      std::vector<std::string> tokens;
      std::string current_token;

      for (char c : input)
      {
        if (c == ' ' || c == '\t')
        {
          if (!current_token.empty())
          {
            tokens.push_back(current_token);
            current_token.clear();
          }
        }
        else
        {
          current_token += c;
        }
      }

      if (!current_token.empty())
        tokens.push_back(current_token);

      return tokens;
    }

    bool isKeyword(const std::string &token)
    {
      return token == "center" || token == "left" || token == "right" ||
             token == "top" || token == "bottom";
    }

    bool isHorizontalKeyword(const std::string &token)
    {
      return token == "left" || token == "right" || token == "center";
    }

    bool isVerticalKeyword(const std::string &token)
    {
      return token == "top" || token == "bottom" || token == "center";
    }

    generics::BackgroundPositionKeyword stringToKeyword(const std::string &token)
    {
      if (token == "center")
        return generics::BackgroundPositionKeyword::kCenterKeyword;
      if (token == "left")
        return generics::BackgroundPositionKeyword::kLeftKeyword;
      if (token == "right")
        return generics::BackgroundPositionKeyword::kRightKeyword;
      if (token == "top")
        return generics::BackgroundPositionKeyword::kTopKeyword;
      if (token == "bottom")
        return generics::BackgroundPositionKeyword::kBottomKeyword;
      return generics::BackgroundPositionKeyword::kNone;
    }

    bool isLengthOrPercentage(const std::string &token)
    {
      return token.find("px") != std::string::npos ||
             token.find("%") != std::string::npos ||
             (std::isdigit(token[0]) || token[0] == '-' || token[0] == '+');
    }

    float parseValue(const std::string &token)
    {
      if (token.find("px") != std::string::npos)
        return std::stof(token.substr(0, token.find("px")));
      else if (token.find("%") != std::string::npos)
        return std::stof(token.substr(0, token.find("%")));
      else
        return std::stof(token);
    }

    bool parseOneValue(const std::string &token)
    {
      if (token == "center")
      {
        tag_ = kCenter;
        return true;
      }
      else if (token == "left")
      {
        tag_ = kLeft;
        return true;
      }
      else if (token == "right")
      {
        tag_ = kRight;
        return true;
      }
      else if (token == "top")
      {
        tag_ = kTop;
        return true;
      }
      else if (token == "bottom")
      {
        tag_ = kBottom;
        return true;
      }
      else if (token.find('%') != std::string::npos)
      {
        tag_ = kPercentage;
        x_ = parseValue(token);
        return true;
      }
      else if (isLengthOrPercentage(token))
      {
        tag_ = kLength;
        x_ = parseValue(token);
        return true;
      }
      return false;
    }

    bool parseTwoValues(const std::string &token1, const std::string &token2)
    {
      // Two values: horizontal vertical
      float x_val = 0, y_val = 0;

      // Parse first value (horizontal)
      if (token1 == "left")
        x_val = 0;
      else if (token1 == "center")
        x_val = 50; // 50% equivalent
      else if (token1 == "right")
        x_val = 100; // 100% equivalent
      else if (isLengthOrPercentage(token1))
        x_val = parseValue(token1);
      else
        return false;

      // Parse second value (vertical)
      if (token2 == "top")
        y_val = 0;
      else if (token2 == "center")
        y_val = 50; // 50% equivalent
      else if (token2 == "bottom")
        y_val = 100; // 100% equivalent
      else if (isLengthOrPercentage(token2))
        y_val = parseValue(token2);
      else
        return false;

      tag_ = kTwoValues;
      x_ = x_val;
      y_ = y_val;
      return true;
    }

    bool parseThreeValues(const std::string &token1, const std::string &token2, const std::string &token3)
    {
      // Three values: keyword offset keyword
      if (!isKeyword(token1) || !isLengthOrPercentage(token2) || !isKeyword(token3))
        return false;

      // Validate that we have one horizontal and one vertical keyword
      bool first_horizontal = isHorizontalKeyword(token1);
      bool third_vertical = isVerticalKeyword(token3);
      bool first_vertical = isVerticalKeyword(token1);
      bool third_horizontal = isHorizontalKeyword(token3);

      if ((first_horizontal && third_vertical) || (first_vertical && third_horizontal))
      {
        tag_ = kThreeValues;
        if (first_horizontal)
        {
          h_keyword_ = stringToKeyword(token1);
          h_offset_ = parseValue(token2);
          v_keyword_ = stringToKeyword(token3);
          v_offset_ = 0.0f;
        }
        else
        {
          v_keyword_ = stringToKeyword(token1);
          v_offset_ = parseValue(token2);
          h_keyword_ = stringToKeyword(token3);
          h_offset_ = 0.0f;
        }
        return true;
      }

      return false;
    }

    bool parseFourValues(const std::string &token1, const std::string &token2, const std::string &token3, const std::string &token4)
    {
      // Four values: keyword offset keyword offset
      if (!isKeyword(token1) || !isLengthOrPercentage(token2) ||
          !isKeyword(token3) || !isLengthOrPercentage(token4))
        return false;

      // Validate that we have one horizontal and one vertical keyword
      bool first_horizontal = isHorizontalKeyword(token1);
      bool third_vertical = isVerticalKeyword(token3);
      bool first_vertical = isVerticalKeyword(token1);
      bool third_horizontal = isHorizontalKeyword(token3);

      if ((first_horizontal && third_vertical) || (first_vertical && third_horizontal))
      {
        tag_ = kFourValues;
        if (first_horizontal)
        {
          h_keyword_ = stringToKeyword(token1);
          h_offset_ = parseValue(token2);
          v_keyword_ = stringToKeyword(token3);
          v_offset_ = parseValue(token4);
        }
        else
        {
          v_keyword_ = stringToKeyword(token1);
          v_offset_ = parseValue(token2);
          h_keyword_ = stringToKeyword(token3);
          h_offset_ = parseValue(token4);
        }
        return true;
      }

      return false;
    }

  public:
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
      else if (isThreeValues())
        return computed::BackgroundPosition::ThreeValues(h_keyword_, h_offset_, v_keyword_);
      else if (isFourValues())
        return computed::BackgroundPosition::FourValues(h_keyword_, h_offset_, v_keyword_, v_offset_);

      // Default to Center if none match
      return computed::BackgroundPosition::Center();
    }
  };
}
