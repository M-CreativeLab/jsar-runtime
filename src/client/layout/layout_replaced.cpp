#include "./layout_replaced.hpp"

namespace endor
{
  namespace client_layout
  {
    void LayoutReplaced::formattingContextDidSet(FormattingContext &context)
    {
      LayoutBox::formattingContextDidSet(context);
      context.setContentSizeEnabled(true);
    }
  }
} // namespace endor
