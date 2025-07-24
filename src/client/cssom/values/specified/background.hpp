#pragma once

#include <client/cssom/values/generics/background.hpp>
#include <client/cssom/values/computed/background.hpp>
#include <client/cssom/values/specified/length.hpp>
#include <sstream>
#include <vector>

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

  private:
    LengthPercentage width_;
    std::optional<LengthPercentage> height_; // For two-value syntax

  public:
    using generics::GenericBackgroundSize<BackgroundSize>::GenericBackgroundSize;

    BackgroundSize() : generics::GenericBackgroundSize<BackgroundSize>() {}

    // Constructor for single length/percentage value
    BackgroundSize(const LengthPercentage &width)
        : generics::GenericBackgroundSize<BackgroundSize>(kLengthPercentage)
        , width_(width)
    {
    }

    // Constructor for two-value syntax
    BackgroundSize(const LengthPercentage &width, const LengthPercentage &height)
        : generics::GenericBackgroundSize<BackgroundSize>(kLengthPercentagePair)
        , width_(width)
        , height_(height)
    {
    }

    // Helper function to parse individual values
    static std::vector<std::string> splitValues(const std::string &input)
    {
      std::vector<std::string> values;
      std::istringstream iss(input);
      std::string value;
      while (iss >> value)
      {
        values.push_back(value);
      }
      return values;
    }

    bool parse(const std::string &input) override
    {
      if (input == "auto")
      {
        tag_ = kAuto;
        return true;
      }
      else if (input == "cover")
      {
        tag_ = kCover;
        return true;
      }
      else if (input == "contain")
      {
        tag_ = kContain;
        return true;
      }
      else
      {
        // Try to parse as length/percentage values
        auto values = splitValues(input);
        
        if (values.size() == 1)
        {
          // Single value - could be length, percentage, or auto
          if (values[0] == "auto" || LengthPercentage::IsLengthOrPercentage(values[0]))
          {
            if (values[0] == "auto")
            {
              width_ = LengthPercentage(); // auto represented as 0px
            }
            else
            {
              LengthPercentage parsedValue;
              if (parsedValue.parse(values[0]))
              {
                width_ = parsedValue;
              }
              else
              {
                return false;
              }
            }
            tag_ = kLengthPercentage;
            height_ = std::nullopt;
            return true;
          }
        }
        else if (values.size() == 2)
        {
          // Two-value syntax: width height
          LengthPercentage parsedWidth, parsedHeight;
          
          // Parse width
          if (values[0] == "auto")
          {
            parsedWidth = LengthPercentage(); // auto as 0px
          }
          else if (LengthPercentage::IsLengthOrPercentage(values[0]))
          {
            if (!parsedWidth.parse(values[0]))
              return false;
          }
          else
          {
            return false;
          }
          
          // Parse height
          if (values[1] == "auto")
          {
            parsedHeight = LengthPercentage(); // auto as 0px
          }
          else if (LengthPercentage::IsLengthOrPercentage(values[1]))
          {
            if (!parsedHeight.parse(values[1]))
              return false;
          }
          else
          {
            return false;
          }
          
          width_ = parsedWidth;
          height_ = parsedHeight;
          tag_ = kLengthPercentagePair;
          return true;
        }
        
        return false;
      }
    }

    std::string toCss() const override
    {
      switch (tag_)
      {
      case kAuto:
        return "auto";
      case kCover:
        return "cover";
      case kContain:
        return "contain";
      case kLengthPercentage:
        return width_.toCss();
      case kLengthPercentagePair:
        return width_.toCss() + " " + (height_ ? height_->toCss() : "auto");
      }
      return "";
    }

    computed::BackgroundSize toComputedValue(computed::Context &context) const override
    {
      if (isAuto())
        return computed::BackgroundSize::Auto();
      else if (isCover())
        return computed::BackgroundSize::Cover();
      else if (isContain())
        return computed::BackgroundSize::Contain();
      else if (isLengthPercentage() || isLengthPercentagePair())
      {
        // For now, create a computed value that holds the length/percentage data
        return computed::BackgroundSize::LengthPercentage(width_.toComputedValue(context), 
                                                          height_ ? std::optional<computed::LengthPercentage>(height_->toComputedValue(context)) : std::nullopt);
      }

      // Default to Auto if none match
      return computed::BackgroundSize::Auto();
    }

    // Getters for the length/percentage values
    const LengthPercentage& getWidth() const { return width_; }
    const std::optional<LengthPercentage>& getHeight() const { return height_; }
  };

  class BackgroundPosition : public generics::GenericBackgroundPosition<BackgroundPosition>,
                             public Parse,
                             public ToComputedValue<computed::BackgroundPosition>
  {
    friend class Parse;

  private:
    LengthPercentage x_;
    std::optional<LengthPercentage> y_; // For two-value syntax
    
  public:
    using generics::GenericBackgroundPosition<BackgroundPosition>::GenericBackgroundPosition;

    BackgroundPosition() : generics::GenericBackgroundPosition<BackgroundPosition>() {}

    // Constructor for single length/percentage value
    BackgroundPosition(const LengthPercentage &x)
        : generics::GenericBackgroundPosition<BackgroundPosition>(kLengthPercentage)
        , x_(x)
    {
    }

    // Constructor for two-value syntax
    BackgroundPosition(const LengthPercentage &x, const LengthPercentage &y)
        : generics::GenericBackgroundPosition<BackgroundPosition>(kLengthPercentagePair)
        , x_(x)
        , y_(y)
    {
    }

    // Convert keyword to percentage equivalent
    static LengthPercentage keywordToPercentage(const std::string &keyword)
    {
      LengthPercentage percentage;
      if (keyword == "left" || keyword == "top")
        percentage.parse("0%");
      else if (keyword == "center")
        percentage.parse("50%");
      else if (keyword == "right" || keyword == "bottom")
        percentage.parse("100%");
      return percentage;
    }

    bool parse(const std::string &input) override
    {
      // Handle basic keyword values
      if (input == "left")
      {
        tag_ = kLeft;
        return true;
      }
      else if (input == "center")
      {
        tag_ = kCenter;
        return true;
      }
      else if (input == "right")
      {
        tag_ = kRight;
        return true;
      }
      else if (input == "top")
      {
        tag_ = kTop;
        return true;
      }
      else if (input == "bottom")
      {
        tag_ = kBottom;
        return true;
      }
      else
      {
        // Try to parse as length/percentage values
        auto values = BackgroundSize::splitValues(input);
        
        if (values.size() == 1)
        {
          // Single value - could be length, percentage, or keyword
          if (values[0] == "left" || values[0] == "center" || values[0] == "right" ||
              values[0] == "top" || values[0] == "bottom")
          {
            x_ = keywordToPercentage(values[0]);
          }
          else if (LengthPercentage::IsLengthOrPercentage(values[0]))
          {
            LengthPercentage parsedValue;
            if (parsedValue.parse(values[0]))
            {
              x_ = parsedValue;
            }
            else
            {
              return false;
            }
          }
          else
          {
            return false;
          }
          tag_ = kLengthPercentage;
          y_ = std::nullopt;
          return true;
        }
        else if (values.size() == 2)
        {
          // Two-value syntax: x y
          LengthPercentage parsedX, parsedY;
          
          // Parse X value
          if (values[0] == "left" || values[0] == "center" || values[0] == "right")
          {
            parsedX = keywordToPercentage(values[0]);
          }
          else if (LengthPercentage::IsLengthOrPercentage(values[0]))
          {
            if (!parsedX.parse(values[0]))
              return false;
          }
          else
          {
            return false;
          }
          
          // Parse Y value
          if (values[1] == "top" || values[1] == "center" || values[1] == "bottom")
          {
            parsedY = keywordToPercentage(values[1]);
          }
          else if (LengthPercentage::IsLengthOrPercentage(values[1]))
          {
            if (!parsedY.parse(values[1]))
              return false;
          }
          else
          {
            return false;
          }
          
          x_ = parsedX;
          y_ = parsedY;
          tag_ = kLengthPercentagePair;
          return true;
        }
        
        return false;
      }
    }

    std::string toCss() const override
    {
      switch (tag_)
      {
      case kLeft:
        return "left";
      case kCenter:
        return "center";
      case kRight:
        return "right";
      case kTop:
        return "top";
      case kBottom:
        return "bottom";
      case kLengthPercentage:
        return x_.toCss();
      case kLengthPercentagePair:
        return x_.toCss() + " " + (y_ ? y_->toCss() : "center");
      }
      return "";
    }

    computed::BackgroundPosition toComputedValue(computed::Context &context) const override
    {
      if (isLeft())
        return computed::BackgroundPosition::Left();
      else if (isCenter())
        return computed::BackgroundPosition::Center();
      else if (isRight())
        return computed::BackgroundPosition::Right();
      else if (isTop())
        return computed::BackgroundPosition::Top();
      else if (isBottom())
        return computed::BackgroundPosition::Bottom();
      else if (isLengthPercentage() || isLengthPercentagePair())
      {
        // For now, create a computed value that holds the length/percentage data
        return computed::BackgroundPosition::LengthPercentage(x_.toComputedValue(context), 
                                                              y_ ? std::optional<computed::LengthPercentage>(y_->toComputedValue(context)) : std::nullopt);
      }

      // Default to Center if none match
      return computed::BackgroundPosition::Center();
    }

    // Getters for the length/percentage values
    const LengthPercentage& getX() const { return x_; }
    const std::optional<LengthPercentage>& getY() const { return y_; }
  };
}
