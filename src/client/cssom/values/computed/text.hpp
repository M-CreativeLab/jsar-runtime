#pragma once

#include <skia/modules/skparagraph/include/TextStyle.h>
#include <client/cssom/values/specified/text.hpp>

namespace client_cssom::values::computed
{
  class TextAlign : public client_cssom::values::specified::TextAlign
  {
    using client_cssom::values::specified::TextAlign::TextAlign;

  public:
    operator skia::textlayout::TextAlign() const
    {
      switch (tag_)
      {
      case Tag::kStart:
        return skia::textlayout::TextAlign::kStart;
      case Tag::kEnd:
        return skia::textlayout::TextAlign::kEnd;
      case Tag::kLeft:
        return skia::textlayout::TextAlign::kLeft;
      case Tag::kRight:
        return skia::textlayout::TextAlign::kRight;
      case Tag::kCenter:
        return skia::textlayout::TextAlign::kCenter;
      case Tag::kJustify:
        return skia::textlayout::TextAlign::kJustify;
      default:
        // TODO(yorkie): support match-parent.
        assert(false && "Invalid tag.");
        return skia::textlayout::TextAlign::kStart;
      }
    }
  };

  class Direction : public client_cssom::values::specified::Direction
  {
    using client_cssom::values::specified::Direction::Direction;

  public:
    operator skia::textlayout::TextDirection() const
    {
      return tag_ == Tag::kLTR
                 ? skia::textlayout::TextDirection::kLtr
                 : skia::textlayout::TextDirection::kRtl;
    }
  };
}
