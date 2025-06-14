#pragma once

#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/generics/border.hpp>
#include <client/cssom/values/computed/border.hpp>
#include <client/cssom/values/specified/length.hpp>

namespace client_cssom::values::specified
{
  class BorderStyle : public generics::GenericBorderStyle<BorderStyle>,
                      public Parse,
                      public ToComputedValue<BorderStyle>
  {
    friend class Parse;
    using generics::GenericBorderStyle<BorderStyle>::GenericBorderStyle;

  private:
    bool parse(const std::string &input) override
    {
      if (input == "hidden")
        tag_ = kHidden;
      else if (input == "none")
        tag_ = kNone;
      else if (input == "inset")
        tag_ = kInset;
      else if (input == "groove")
        tag_ = kGroove;
      else if (input == "outset")
        tag_ = kOutset;
      else if (input == "ridge")
        tag_ = kRidge;
      else if (input == "dotted")
        tag_ = kDotted;
      else if (input == "dashed")
        tag_ = kDashed;
      else if (input == "solid")
        tag_ = kSolid;
      else if (input == "double")
        tag_ = kDouble;
      return true;
    }

  public:
    BorderStyle toComputedValue(computed::Context &context) const override
    {
      return *this;
    }
  };

  class BorderSideWidth : public Parse,
                          public ToCss,
                          public ToComputedValue<computed::BorderSideWidth>
  {
    friend class Parse;

  private:
    enum Tag
    {
      kLength,
      kThin,
      kMedium,
      kThick,
    };

    static constexpr float THIN_IN_PX = 1.0f;
    static constexpr float MEDIUM_IN_PX = 3.0f;
    static constexpr float THICK_IN_PX = 5.0f;

  public:
    static BorderSideWidth Thin()
    {
      return BorderSideWidth(kThin, THIN_IN_PX);
    }
    static BorderSideWidth Medium()
    {
      return BorderSideWidth(kMedium, MEDIUM_IN_PX);
    }
    static BorderSideWidth Thick()
    {
      return BorderSideWidth(kThick, THICK_IN_PX);
    }

  public:
    BorderSideWidth()
        : tag_(kMedium)
        , length_(NoCalcLength::FromPx(MEDIUM_IN_PX))
    {
    }

  private:
    BorderSideWidth(Tag tag, float px)
        : tag_(tag)
        , length_(NoCalcLength::FromPx(px))
    {
    }

    bool parse(const std::string &input) override
    {
      if (input == "thin")
      {
        tag_ = kThin;
        length_ = NoCalcLength::FromPx(THIN_IN_PX);
      }
      else if (input == "medium")
      {
        tag_ = kMedium;
        length_ = NoCalcLength::FromPx(MEDIUM_IN_PX);
      }
      else if (input == "thick")
      {
        tag_ = kThick;
        length_ = NoCalcLength::FromPx(THICK_IN_PX);
      }
      else
      {
        length_ = Parse::ParseSingleValue<NoCalcLength>(input);
        tag_ = kLength;
      }
      return true;
    }

  public:
    std::string toCss() const override
    {
      switch (tag_)
      {
      case kThin:
        return "thin";
      case kMedium:
        return "medium";
      case kThick:
        return "thick";
      default:
        return length_.toCss();
      }
    }
    computed::BorderSideWidth toComputedValue(computed::Context &context) const override
    {
      switch (tag_)
      {
      case kThin:
        return computed::BorderSideWidth(THIN_IN_PX);
      case kMedium:
        return computed::BorderSideWidth(MEDIUM_IN_PX);
      case kThick:
        return computed::BorderSideWidth(THICK_IN_PX);
      default:
        return computed::BorderSideWidth(length_.toComputedValue(context));
      }
    }

  private:
    Tag tag_;
    // TODO(yorkie): support calc length.
    NoCalcLength length_;
  };

  class BorderSideStyle : public generics::GenericBorderStyle<BorderSideStyle>,
                          public Parse,
                          public ToComputedValue<computed::BorderSideStyle>
  {
    friend class Parse;
    using generics::GenericBorderStyle<BorderSideStyle>::GenericBorderStyle;

  private:
    bool parse(const std::string &input) override
    {
      if (input == "hidden")
        tag_ = kHidden;
      else if (input == "none")
        tag_ = kNone;
      else if (input == "inset")
        tag_ = kInset;
      else if (input == "groove")
        tag_ = kGroove;
      else if (input == "outset")
        tag_ = kOutset;
      else if (input == "ridge")
        tag_ = kRidge;
      else if (input == "dotted")
        tag_ = kDotted;
      else if (input == "dashed")
        tag_ = kDashed;
      else if (input == "solid")
        tag_ = kSolid;
      else if (input == "double")
        tag_ = kDouble;
      return true;
    }

  public:
    computed::BorderSideStyle toComputedValue(computed::Context &context) const override
    {
      switch (tag_)
      {
      case kHidden:
        return computed::BorderSideStyle::Hidden();
      case kNone:
        return computed::BorderSideStyle::None();
      case kInset:
        return computed::BorderSideStyle::Inset();
      case kGroove:
        return computed::BorderSideStyle::Groove();
      case kOutset:
        return computed::BorderSideStyle::Outset();
      case kRidge:
        return computed::BorderSideStyle::Ridge();
      case kDotted:
        return computed::BorderSideStyle::Dotted();
      case kDashed:
        return computed::BorderSideStyle::Dashed();
      case kSolid:
        return computed::BorderSideStyle::Solid();
      case kDouble:
        return computed::BorderSideStyle::Double();
      default:
        return computed::BorderSideStyle::None();
      }
    }
  };

  class BorderCornerRadius : public generics::GenericBorderCornerRadius<NonNegativeLengthPercentage>,
                             public Parse,
                             public ToCss,
                             public ToComputedValue<computed::BorderCornerRadius>
  {
    friend class Parse;
    using generics::GenericBorderCornerRadius<NonNegativeLengthPercentage>::GenericBorderCornerRadius;

  private:
    bool parse(const std::string &input) override
    {
      value_ = Parse::ParseSingleValue<NonNegativeLengthPercentage>(input);
      return true;
    }

  public:
    std::string toCss() const override
    {
      return value_.toCss();
    }
    computed::BorderCornerRadius toComputedValue(computed::Context &context) const override
    {
      return computed::BorderCornerRadius(value_.toComputedValue(context));
    }
  };
}
