#pragma once

#include "./formatting_contexts.hpp"
#include "./layout_view.hpp"
#include "./layout_object.hpp"

namespace client_layout
{
  template <DisplayInside Inner>
    requires is_taffy_supported<Inner>
  BlockFormattingContext<Inner>::BlockFormattingContext(std::shared_ptr<LayoutView> view)
      : TaffyBasedFormattingContext(DisplayType::Block(Inner), view)
  {
  }
}
