#pragma once

#include <skia/modules/skparagraph/include/TextStyle.h>
#include "./keyword.hpp"

namespace client_cssom::types
{
  class FontStyle final : KeywordType<FontStyle, FontStyleKeyword, FontStyleKeyword::kNormal>
  {
    using KeywordType::KeywordType;

  public:
    // Convert to `SkFontStyle::Slant`
    operator SkFontStyle::Slant() const
    {
      switch (keyword_)
      {
      case FontStyleKeyword::kNormal:
        return SkFontStyle::Slant::kUpright_Slant;
      case FontStyleKeyword::kItalic:
        return SkFontStyle::Slant::kItalic_Slant;
      case FontStyleKeyword::kOblique:
        return SkFontStyle::Slant::kOblique_Slant;
      }
    }
  };
}
