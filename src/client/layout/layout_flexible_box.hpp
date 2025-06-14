#pragma once

#include "./layout_block.hpp"

namespace client_layout
{
  class LayoutFlexibleBox : public LayoutBlock
  {
    using LayoutBlock::LayoutBlock;

  public:
    const char *name() const override
    {
      return "LayoutFlexBox";
    }
    bool isFlexibleBox() const override final
    {
      return true;
    }
  };
}
