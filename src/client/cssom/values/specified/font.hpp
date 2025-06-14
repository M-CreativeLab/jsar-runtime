#pragma once

#include <variant>
#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/common.hpp>
#include <client/cssom/values/generics/font.hpp>
#include <client/cssom/values/specified/angle.hpp>
#include <client/cssom/values/specified/length.hpp>
#include <client/cssom/values/computed/font.hpp>

namespace client_cssom::values::specified
{
  enum class SystemFont
  {
    kCaption,
    kIcon,
    kMenu,
    kMessageBox,
    kSmallCaption,
    kStatusBar,
    kEnd,
  };

  enum class FontSizeKeyword
  {
    kXXSmall,
    kXSmall,
    kSmall,
    kMedium,
    kLarge,
    kXLarge,
    kXXLarge,
    kMath,
    kNone,
  };

  class KeywordInfo : public ToCss,
                      public ToComputedValue<float>
  {
  public:
    KeywordInfo(FontSizeKeyword keyword)
        : keyword(keyword)
        , factor(0.0f)
        , offset(0.0f)
    {
    }

    std::string toCss() const override
    {
      switch (keyword)
      {
      case FontSizeKeyword::kXXSmall:
        return "xx-small";
      case FontSizeKeyword::kXSmall:
        return "x-small";
      case FontSizeKeyword::kSmall:
        return "small";
      case FontSizeKeyword::kMedium:
        return "medium";
      case FontSizeKeyword::kLarge:
        return "large";
      case FontSizeKeyword::kXLarge:
        return "x-large";
      case FontSizeKeyword::kXXLarge:
        return "xx-large";
      case FontSizeKeyword::kMath:
        return "math";
      case FontSizeKeyword::kNone:
        return "none";
      }
      assert(false && "Invalid keyword.");
    }
    float toComputedValue(computed::Context &) const override
    {
      switch (keyword)
      {
      case FontSizeKeyword::kXXSmall:
        return 0.5f;
      case FontSizeKeyword::kXSmall:
        return 0.75f;
      case FontSizeKeyword::kSmall:
        return 0.875f;
      case FontSizeKeyword::kMedium:
        return 1.0f;
      case FontSizeKeyword::kLarge:
        return 1.25f;
      case FontSizeKeyword::kXLarge:
        return 1.5f;
      case FontSizeKeyword::kXXLarge:
        return 2.0f;
      case FontSizeKeyword::kMath:
        return 1.2f;
      case FontSizeKeyword::kNone:
        return 0.0f;
      }
      assert(false && "Invalid keyword.");
    }

  public:
    FontSizeKeyword keyword;
    float factor;
    float offset;
  };

  class FontSize : public Parse,
                   public ToCss,
                   public ToComputedValue<computed::FontSize>
  {
    friend class Parse;

  private:
    enum Tag
    {
      kLength,
      kKeyword,
      kSmaller,
      kLarger,
      kSystem,
    };
    using ValueVariant = std::variant<LengthPercentage, KeywordInfo, SystemFont>;

  private:
    bool parse(const std::string &input) override
    {
#define CHECK_AND_SET(NAME, TAG, VALUE) \
  if (input == NAME)                    \
  {                                     \
    tag_ = TAG;                         \
    value_ = VALUE;                     \
    return true;                        \
  }

#define CHECK_AND_SET_TAG_ONLY(NAME, TAG) \
  if (input == NAME)                      \
  {                                       \
    tag_ = TAG;                           \
    return true;                          \
  }

      // System font keywords.
      CHECK_AND_SET("caption", kSystem, SystemFont::kCaption)
      CHECK_AND_SET("icon", kSystem, SystemFont::kIcon)
      CHECK_AND_SET("menu", kSystem, SystemFont::kMenu)
      CHECK_AND_SET("message-box", kSystem, SystemFont::kMessageBox)
      CHECK_AND_SET("small-caption", kSystem, SystemFont::kSmallCaption)
      CHECK_AND_SET("status-bar", kSystem, SystemFont::kStatusBar)

      // Font size keywords.
      CHECK_AND_SET("xx-small", kKeyword, KeywordInfo(FontSizeKeyword::kXXSmall))
      CHECK_AND_SET("x-small", kKeyword, KeywordInfo(FontSizeKeyword::kXSmall))
      CHECK_AND_SET("small", kKeyword, KeywordInfo(FontSizeKeyword::kSmall))
      CHECK_AND_SET("medium", kKeyword, KeywordInfo(FontSizeKeyword::kMedium))
      CHECK_AND_SET("large", kKeyword, KeywordInfo(FontSizeKeyword::kLarge))
      CHECK_AND_SET("x-large", kKeyword, KeywordInfo(FontSizeKeyword::kXLarge))
      CHECK_AND_SET("xx-large", kKeyword, KeywordInfo(FontSizeKeyword::kXXLarge))
      CHECK_AND_SET("math", kKeyword, KeywordInfo(FontSizeKeyword::kMath))
      CHECK_AND_SET("none", kKeyword, KeywordInfo(FontSizeKeyword::kNone))

      // Font size relative keywords.
      CHECK_AND_SET_TAG_ONLY("smaller", kSmaller)
      CHECK_AND_SET_TAG_ONLY("larger", kLarger)

#undef CHECK_AND_SET
#undef CHECK_AND_SET_TAG_ONLY

      // Length
      tag_ = kLength;
      value_ = Parse::ParseSingleValue<LengthPercentage>(input);
      return true;
    }

  public:
    std::string toCss() const override
    {
      switch (tag_)
      {
      case kLength:
        return std::get<LengthPercentage>(value_).toCss();
      case kKeyword:
        return std::get<KeywordInfo>(value_).toCss();
      case kSmaller:
        return "smaller";
      case kLarger:
        return "larger";
      case kSystem:
        return "system";
      }
      assert(false && "Invalid tag.");
    }

    computed::FontSize toComputedValue(computed::Context &context) const override
    {
      if (tag_ == kLength)
      {
        const auto &length = std::get<LengthPercentage>(value_);
        return computed::FontSize(length.toComputedValue(context).getLength().px());
      }
      if (tag_ == kKeyword)
      {
        const auto &keyword = std::get<KeywordInfo>(value_);
        return computed::FontSize(keyword.toComputedValue(context));
      }
      return computed::FontSize();
    }

  private:
    Tag tag_;
    ValueVariant value_ = KeywordInfo(FontSizeKeyword::kMedium);
  };

  class AbsoluteFontWeight : public ToCss,
                             public ToComputedValue<int>
  {
  private:
    enum Tag
    {
      kWeight,
      kNormal,
      kBold,
    };

  public:
    static AbsoluteFontWeight Normal()
    {
      return AbsoluteFontWeight(kNormal, 400);
    }
    static AbsoluteFontWeight Bold()
    {
      return AbsoluteFontWeight(kBold, 700);
    }
    static AbsoluteFontWeight Weight(int weight_number)
    {
      if (weight_number < 1 || weight_number > 1000)
        throw std::out_of_range("Weight number must be between 1 and 1000");
      return AbsoluteFontWeight(kWeight, weight_number);
    }

  private:
    AbsoluteFontWeight(Tag tag, float weight_number)
        : tag_(tag)
        , weight_number_(weight_number)
    {
    }

  public:
    std::string toCss() const override
    {
      switch (tag_)
      {
      case kWeight:
        return std::to_string(weight_number_);
      case kNormal:
        return "normal";
      case kBold:
        return "bold";
      }
      assert(false && "Invalid tag.");
    }
    int toComputedValue(computed::Context &) const override
    {
      return weight_number_;
    }

  private:
    Tag tag_;
    int weight_number_;
  };

  class FontWeight : public Parse,
                     public ToCss,
                     public ToComputedValue<computed::FontWeight>
  {
    friend class Parse;

  private:
    enum Tag : uint8_t
    {
      kAbsolute,
      kBolder,
      kLighter,
      kSystem,
    };
    using ValueVariant = std::variant<AbsoluteFontWeight, SystemFont>;

  private:
    bool parse(const std::string &input) override
    {
      if (isdigit(input[0]))
      {
        tag_ = kAbsolute;
        value_ = AbsoluteFontWeight::Weight(std::stoi(input));
        return true;
      }
      if (input == "normal")
      {
        tag_ = kAbsolute;
        value_ = AbsoluteFontWeight::Normal();
        return true;
      }
      if (input == "bold")
      {
        tag_ = kAbsolute;
        value_ = AbsoluteFontWeight::Bold();
        return true;
      }
      if (input == "bolder")
      {
        tag_ = kBolder;
        return true;
      }
      if (input == "lighter")
      {
        tag_ = kLighter;
        return true;
      }

      // TODO(yorkie): support system font.
      return false;
    }

  public:
    std::string toCss() const override
    {
      switch (tag_)
      {
      case kAbsolute:
        return std::get<AbsoluteFontWeight>(value_).toCss();
      case kBolder:
        return "bolder";
      case kLighter:
        return "lighter";
      case kSystem:
        // TODO(yorkie): support system font.
        return "system";
      }
      assert(false && "Invalid tag.");
    }
    computed::FontWeight toComputedValue(computed::Context &context) const override
    {
      if (tag_ == kAbsolute)
        return computed::FontWeight(std::get<AbsoluteFontWeight>(value_).toComputedValue(context));
      if (tag_ == kBolder)
        return computed::FontWeight(context.baseFontWeight() + 100);
      if (tag_ == kLighter)
        return computed::FontWeight(context.baseFontWeight() - 100);
      assert(false && "Invalid tag.");
    }

  private:
    Tag tag_;
    ValueVariant value_ = AbsoluteFontWeight::Normal();
  };

  class FontStyle : public generics::FontStyle<FontStyle, Angle>,
                    public Parse,
                    public ToCss,
                    public ToComputedValue<computed::FontStyle>
  {
    friend class Parse;

  private:
    bool parse(const std::string &input) override
    {
      if (input == "normal")
      {
        tag_ = kNormal;
        return true;
      }
      if (input == "italic")
      {
        tag_ = kItalic;
        return true;
      }
      if (input == "oblique")
      {
        tag_ = kOblique;
        return true;
      }
      if (input.starts_with("oblique "))
      {
        tag_ = kOblique;
        auto angle_str = input.substr(8);
        if (!angle_str.empty())
          oblique_angle_ = Parse::ParseSingleValue<Angle>(angle_str);
        return true;
      }
      return false;
    }

  public:
    std::string toCss() const override
    {
      switch (tag_)
      {
      case kNormal:
        return "normal";
      case kItalic:
        return "italic";
      case kOblique:
        if (oblique_angle_.has_value())
          return "oblique " + oblique_angle_->toCss();
        return "oblique";
      }
      assert(false && "Invalid tag.");
    }
    computed::FontStyle toComputedValue(computed::Context &context) const override
    {
      switch (tag_)
      {
      case kNormal:
        return computed::FontStyle::Normal();
      case kItalic:
        return computed::FontStyle::Italic();
      case kOblique:
        return computed::FontStyle::Oblique(oblique_angle_->toComputedValue(context));
      }
      assert(false && "Invalid tag.");
    }
  };

  class LineHeight : public generics::GenericLineHeight<LineHeight, NonNegativeNumber, NonNegativeLengthPercentage>,
                     public Parse,
                     public ToCss,
                     public ToComputedValue<computed::LineHeight>
  {
    friend class Parse;
    using generics::GenericLineHeight<LineHeight, NonNegativeNumber, NonNegativeLengthPercentage>::GenericLineHeight;

  private:
    bool parse(const std::string &input) override
    {
      if (input == "normal")
      {
        setNormal();
        return true;
      }
      if (LengthPercentage::IsLengthOrPercentage(input))
      {
        setLength(Parse::ParseSingleValue<NonNegativeLengthPercentage>(input));
        return true;
      }
      else
      {
        setNumber(Parse::ParseSingleValue<NonNegativeNumber>(input));
        return true;
      }
      return false;
    }

  public:
    std::string toCss() const override
    {
      if (isNormal())
        return "normal";
      if (isLength())
        return getLength().toCss();
      if (isNumber())
        return getNumber().toCss();

      // unreachable
      assert(false && "Invalid tag.");
    }
    computed::LineHeight toComputedValue(computed::Context &context) const override
    {
      if (isNormal())
      {
        return computed::LineHeight::Normal();
      }
      if (isLength())
      {
        auto computed_length = getLength().toComputedValue(context).getLength();
        return computed::LineHeight::Length(computed_length.px());
      }
      if (isNumber())
      {
        CSSFloat computed_number = getNumber().toComputedValue(context);
        return computed::LineHeight::Number(computed_number);
      }

      // unreachable
      assert(false && "Invalid tag.");
    }
  };
}
