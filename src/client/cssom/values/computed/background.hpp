#pragma once

#include <client/cssom/values/generics/background.hpp>
#include <client/cssom/values/computed/length_percentage.hpp>
#include <skia/include/core/SkBlendMode.h>
#include <optional>

namespace client_cssom::values::specified
{
  class BackgroundBlendMode;
  class BackgroundClip;
  class BackgroundOrigin;
  class BackgroundRepeat;
  class BackgroundSize;
  class BackgroundPosition;
}

namespace client_cssom::values::computed
{
  class BackgroundBlendMode : public generics::GenericBackgroundBlendMode<BackgroundBlendMode>
  {
    using generics::GenericBackgroundBlendMode<BackgroundBlendMode>::GenericBackgroundBlendMode;

  public:
    operator SkBlendMode() const
    {
      switch (tag_)
      {
      case BackgroundBlendMode::kNormal:
        return SkBlendMode::kSrcOver;
      case BackgroundBlendMode::kMultiply:
        return SkBlendMode::kMultiply;
      case BackgroundBlendMode::kScreen:
        return SkBlendMode::kScreen;
      case BackgroundBlendMode::kOverlay:
        return SkBlendMode::kOverlay;
      case BackgroundBlendMode::kDarken:
        return SkBlendMode::kDarken;
      case BackgroundBlendMode::kLighten:
        return SkBlendMode::kLighten;
      case BackgroundBlendMode::kColorDodge:
        return SkBlendMode::kColorDodge;
      case BackgroundBlendMode::kColorBurn:
        return SkBlendMode::kColorBurn;
      case BackgroundBlendMode::kHardLight:
        return SkBlendMode::kHardLight;
      case BackgroundBlendMode::kSoftLight:
        return SkBlendMode::kSoftLight;
      case BackgroundBlendMode::kDifference:
        return SkBlendMode::kDifference;
      case BackgroundBlendMode::kExclusion:
        return SkBlendMode::kExclusion;
      case BackgroundBlendMode::kHue:
        return SkBlendMode::kHue;
      case BackgroundBlendMode::kSaturation:
        return SkBlendMode::kSaturation;
      case BackgroundBlendMode::kColor:
        return SkBlendMode::kColor;
      case BackgroundBlendMode::kLuminosity:
        return SkBlendMode::kLuminosity;
      default:
        return SkBlendMode::kSrcOver; // Default to SrcOver if none match
      }
    }
  };

  class BackgroundClip : public generics::GenericBackgroundClip<BackgroundClip>
  {
    using generics::GenericBackgroundClip<BackgroundClip>::GenericBackgroundClip;
  };

  class BackgroundOrigin : public generics::GenericBackgroundOrigin<BackgroundOrigin>
  {
    using generics::GenericBackgroundOrigin<BackgroundOrigin>::GenericBackgroundOrigin;
  };

  class BackgroundRepeat : public generics::GenericBackgroundRepeat<BackgroundRepeat>
  {
    using generics::GenericBackgroundRepeat<BackgroundRepeat>::GenericBackgroundRepeat;
  };

  class BackgroundSize : public generics::GenericBackgroundSize<BackgroundSize>
  {
  private:
    LengthPercentage width_;
    std::optional<LengthPercentage> height_;

  public:
    using generics::GenericBackgroundSize<BackgroundSize>::GenericBackgroundSize;

    // Constructor for length/percentage values
    BackgroundSize(const LengthPercentage &width, const std::optional<LengthPercentage> &height = std::nullopt)
        : generics::GenericBackgroundSize<BackgroundSize>(height ? kTwoValues : kSingleValue)
        , width_(width)
        , height_(height)
    {
    }

    static BackgroundSize LengthPercentage(const computed::LengthPercentage &width, const std::optional<computed::LengthPercentage> &height = std::nullopt)
    {
      return BackgroundSize(width, height);
    }

    std::string toCss() const override
    {
      switch (tag_)
      {
      case kCover:
        return "cover";
      case kContain:
        return "contain";
      case kSingleValue:
        return width_.toCss();
      case kTwoValues:
        return width_.toCss() + " " + (height_ ? height_->toCss() : "auto");
      }
      return "";
    }

    // Getters for rendering
    const computed::LengthPercentage& getWidth() const { return width_; }
    const std::optional<computed::LengthPercentage>& getHeight() const { return height_; }
  };

  // Multiple background-size values support
  class MultipleBackgroundSize : public ToCss
  {
  private:
    std::vector<BackgroundSize> values_;

  public:
    MultipleBackgroundSize() = default;
    MultipleBackgroundSize(const std::vector<BackgroundSize> &values) : values_(values) {}

    std::string toCss() const override
    {
      if (values_.empty())
        return "";

      std::string result;
      for (size_t i = 0; i < values_.size(); ++i)
      {
        if (i > 0)
          result += ", ";
        result += values_[i].toCss();
      }
      return result;
    }

    const std::vector<BackgroundSize>& getValues() const { return values_; }
    size_t size() const { return values_.size(); }
    bool empty() const { return values_.empty(); }
    const BackgroundSize& operator[](size_t index) const { return values_[index]; }
  };

  class BackgroundPosition : public generics::GenericBackgroundPosition<BackgroundPosition>
  {
  private:
    LengthPercentage x_;
    std::optional<LengthPercentage> y_;

  public:
    using generics::GenericBackgroundPosition<BackgroundPosition>::GenericBackgroundPosition;

    // Constructor for length/percentage values
    BackgroundPosition(const LengthPercentage &x, const std::optional<LengthPercentage> &y = std::nullopt)
        : generics::GenericBackgroundPosition<BackgroundPosition>(y ? kTwoValues : kSingleLengthPercentage)
        , x_(x)
        , y_(y)
    {
    }

    static BackgroundPosition LengthPercentage(const computed::LengthPercentage &x, const std::optional<computed::LengthPercentage> &y = std::nullopt)
    {
      return BackgroundPosition(x, y);
    }

    std::string toCss() const override
    {
      switch (tag_)
      {
      case kSingleKeyword:
      case kSingleLengthPercentage:
        return x_.toCss();
      case kTwoValues:
        return x_.toCss() + " " + (y_ ? y_->toCss() : "center");
      case kEdgeOffsets:
        return x_.toCss() + " " + (y_ ? y_->toCss() : "center");
      }
      return "";
    }

    // Getters for rendering
    const computed::LengthPercentage& getX() const { return x_; }
    const std::optional<computed::LengthPercentage>& getY() const { return y_; }
  };

  // Multiple background-position values support
  class MultipleBackgroundPosition : public ToCss
  {
  private:
    std::vector<BackgroundPosition> values_;

  public:
    MultipleBackgroundPosition() = default;
    MultipleBackgroundPosition(const std::vector<BackgroundPosition> &values) : values_(values) {}

    std::string toCss() const override
    {
      if (values_.empty())
        return "";

      std::string result;
      for (size_t i = 0; i < values_.size(); ++i)
      {
        if (i > 0)
          result += ", ";
        result += values_[i].toCss();
      }
      return result;
    }

    const std::vector<BackgroundPosition>& getValues() const { return values_; }
    size_t size() const { return values_.size(); }
    bool empty() const { return values_.empty(); }
    const BackgroundPosition& operator[](size_t index) const { return values_[index]; }
  };
}
