#pragma once

#include <skia/modules/skparagraph/include/TextStyle.h>
#include <client/cssom/values/specified/text.hpp>
#include <crates/bindings.hpp>

namespace client_cssom::values::computed
{
  class TextAlign : public specified::TextAlign
  {
    using specified::TextAlign::TextAlign;

  public:
    TextAlign(const specified::TextAlign &other)
        : specified::TextAlign(other)
    {
    }

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

    // Convert to layout value for layout system
    crates::layout2::styles::TextAlign toLayoutValue() const
    {
      switch (tag_)
      {
      case Tag::kStart:
        return crates::layout2::styles::TextAlign::Start();
      case Tag::kEnd:
        return crates::layout2::styles::TextAlign::End();
      case Tag::kLeft:
        return crates::layout2::styles::TextAlign::Left();
      case Tag::kRight:
        return crates::layout2::styles::TextAlign::Right();
      case Tag::kCenter:
        return crates::layout2::styles::TextAlign::Center();
      case Tag::kJustify:
        return crates::layout2::styles::TextAlign::Justify();
      default:
        // TODO(yorkie): support match-parent.
        return crates::layout2::styles::TextAlign::Start();
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
