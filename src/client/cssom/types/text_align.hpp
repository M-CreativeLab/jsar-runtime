#pragma once

#include <skia/modules/skparagraph/include/TextStyle.h>
#include "./keyword.hpp"

namespace client_cssom::types
{
  class TextAlign final : KeywordType<TextAlign, TextAlignKeyword, TextAlignKeyword::kLeft>
  {
    using KeywordType::KeywordType;

  public:
    inline static TextAlign Left() { return TextAlign(TextAlignKeyword::kLeft); }
    inline static TextAlign Right() { return TextAlign(TextAlignKeyword::kRight); }
    inline static TextAlign Center() { return TextAlign(TextAlignKeyword::kCenter); }

  public:
    // Convert to Skia TextAlign
    operator skia::textlayout::TextAlign() const
    {
#define XX(_, KEY)                       \
  if (keyword_ == TextAlignKeyword::KEY) \
    return skia::textlayout::TextAlign::KEY;

      TEXT_ALIGN_KEYWORDS_MAP(XX)
#undef XX

      // Default case
      return skia::textlayout::TextAlign::kLeft;
    }
  };
}
