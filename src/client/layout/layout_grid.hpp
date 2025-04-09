#pragma once

#include "./layout_block.hpp"

namespace client_layout
{
  class LayoutGrid : public LayoutBlock
  {
    using LayoutBlock::LayoutBlock;

  public:
    const char *name() const override { return "LayoutGrid"; }
    bool isLayoutGrid() const override final { return true; }
  };
}
