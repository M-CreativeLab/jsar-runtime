#pragma once

#include <memory>
#include "./layout_block.hpp"

namespace client_layout
{
  class LayoutBlockFlow : public LayoutBlock
  {
    using LayoutBlock::LayoutBlock;

  public:
    const char *name() const override
    {
      return "LayoutBlockFlow";
    }
    bool isLayoutBlockFlow() const override final
    {
      return true;
    }

    void addChild(std::shared_ptr<LayoutObject> child,
                  std::shared_ptr<LayoutObject> beforeChild = nullptr) override;
    void removeChild(std::shared_ptr<LayoutObject> child) override;
  };
}
