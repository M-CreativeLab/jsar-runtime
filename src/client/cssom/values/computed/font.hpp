#pragma once

#include <optional>
#include <skia/modules/skparagraph/include/TextStyle.h>
#include <client/cssom/values/generics/font.hpp>
#include <client/cssom/values/computed/angle.hpp>
#include <client/cssom/values/computed/common.hpp>
#include <client/cssom/values/computed/length.hpp>

namespace client_cssom::values::computed
{
  class FontSize
  {
  public:
    static constexpr float FONT_SMALL_PX = 12.0f;
    static constexpr float FONT_MEDIUM_PX = 16.0f;
    static constexpr float FONT_LARGE_PX = 20.0f;

    static FontSize Large()
    {
      return FontSize(FONT_SMALL_PX);
    }
    static FontSize Medium()
    {
      return FontSize(FONT_MEDIUM_PX);
    }
    static FontSize Small()
    {
      return FontSize(FONT_LARGE_PX);
    }

  public:
    FontSize(float sizeInPx = FONT_MEDIUM_PX)
        : computed_size_(sizeInPx)
        , used_size_(sizeInPx)
    {
    }

  public:
    Length computedSize() const
    {
      return computed_size_;
    }
    Length usedSize() const
    {
      return used_size_;
    }

  private:
    NonNegativeLength computed_size_;
    NonNegativeLength used_size_;
  };

  class FontWeight
  {
  private:
    static constexpr int NORMAL = 400;
    static constexpr int BOLD = 700;
    static constexpr int BOLDER = 900;
    static constexpr int LIGHTER = 100;

  public:
    static FontWeight Normal()
    {
      return FontWeight(NORMAL);
    }
    static FontWeight Bold()
    {
      return FontWeight(BOLD);
    }
    static FontWeight Bolder()
    {
      return FontWeight(BOLDER);
    }
    static FontWeight Lighter()
    {
      return FontWeight(LIGHTER);
    }

  public:
    FontWeight(int value = NORMAL)
        : value_(value)
    {
    }

    int value() const
    {
      return value_;
    }

  private:
    int value_;
  };

  class FontStyle
  {
  public:
    enum class Slant
    {
      kNormal,
      kItalic,
      kOblique,
    };

    static FontStyle Normal()
    {
      return FontStyle(Slant::kNormal);
    }
    static FontStyle Italic()
    {
      return FontStyle(Slant::kItalic);
    }
    static FontStyle Oblique(std::optional<Angle> angle = std::nullopt)
    {
      FontStyle oblique_style(Slant::kOblique);
      oblique_style.oblique_angle_ = angle;
      return oblique_style;
    }

  public:
    FontStyle(Slant slant = Slant::kNormal)
        : slant_(slant)
    {
    }

    Slant slant() const
    {
      return slant_;
    }
    operator SkFontStyle::Slant() const
    {
      switch (slant_)
      {
      case Slant::kNormal:
        return SkFontStyle::Slant::kUpright_Slant;
      case Slant::kItalic:
        return SkFontStyle::Slant::kItalic_Slant;
      case Slant::kOblique:
        return SkFontStyle::Slant::kOblique_Slant;
      }
    }

  private:
    Slant slant_;
    std::optional<Angle> oblique_angle_;
  };

  class LineHeight : public generics::GenericLineHeight<LineHeight, NonNegativeNumber, NonNegativeLength>
  {
    using generics::GenericLineHeight<LineHeight, NonNegativeNumber, NonNegativeLength>::GenericLineHeight;

  public:
    // Returns the computed size in pixels based on the base font size.
    float computedSize(float base_font_size) const
    {
      if (isLength())
        return getLength().px();
      else if (isNumber())
        return getNumber().value * base_font_size;
      else
        return 1.2f * base_font_size; // Default line height
    }
  };
}
