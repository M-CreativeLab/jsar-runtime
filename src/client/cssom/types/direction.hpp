#pragma once

#include <skia/modules/skparagraph/include/TextStyle.h>
#include "./keyword.hpp"

namespace client_cssom::types
{
  class Direction final : KeywordType<Direction, DirectionKeyword, DirectionKeyword::kLtr>
  {
    using KeywordType::KeywordType;

  public:
    static Direction Ltr() { return Direction(DirectionKeyword::kLtr); }
    static Direction Rtl() { return Direction(DirectionKeyword::kRtl); }

  public:
    // Convert to Skia TextDirection
    operator skia::textlayout::TextDirection() const
    {
#define XX(_, KEY)                       \
  if (keyword_ == DirectionKeyword::KEY) \
    return skia::textlayout::TextDirection::KEY;

      DIR_KEYWORDS_MAP(XX)
#undef XX

      // Default case
      return skia::textlayout::TextDirection::kLtr;
    }
  };
}
