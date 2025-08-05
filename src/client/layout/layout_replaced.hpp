#pragma once

#include "./layout_box.hpp"

namespace client_layout
{
  class LayoutReplaced : public LayoutBox
  {
    using LayoutBox::LayoutBox;

  public:
    const char *name() const override
    {
      return "LayoutReplaced";
    }
    bool isLayoutReplaced() const override final
    {
      return true;
    }

    // Replaced elements should not have any layout children
    void addChild(std::shared_ptr<LayoutObject> newChild,
                  std::shared_ptr<LayoutObject> beforeChild = nullptr) override final
    {
      // Do nothing - replaced elements ignore all children
    }

    void formattingContextDidSet(FormattingContext &) override final;
  };
}
