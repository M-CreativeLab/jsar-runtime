#pragma once

#include "./layout_box.hpp"

namespace client_layout
{
  class LayoutReplaced : public LayoutBox
  {
    using LayoutBox::LayoutBox;

  public:
    const char *name() const override { return "LayoutReplaced"; }
    bool isLayoutReplaced() const override final { return true; }

    void formattingContextDidSet(FormattingContext &) override final;
  };
}
