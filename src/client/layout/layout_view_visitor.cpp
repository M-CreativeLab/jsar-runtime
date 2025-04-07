#include "./layout_view_visitor.hpp"
#include "./layout_object.hpp"
#include "./layout_text.hpp"
#include "./layout_box.hpp"
#include "./layout_block.hpp"

namespace client_layout
{
  void LayoutViewVisitor::visit(LayoutView &view)
  {
    int depth = 0;
    function<void(LayoutObject &)> visitObject =
        [this, &depth, &visitObject](LayoutObject &object)
    {
      if (false == onVisitObject(object, depth))
        return;

      if (object.isText())
        onVisitText(dynamic_cast<const LayoutText &>(object), depth);
      if (object.isBox())
        onVisitBox(dynamic_cast<const LayoutBox &>(object), depth);

      if (object.isLayoutBlock())
      {
        auto &block = dynamic_cast<LayoutBlock &>(object);
        onVisitBlock(block, depth);

        depth++;
        for (auto &child : block.childrenRef())
        {
          assert(child.parent() != nullptr && "The child must have a parent.");
          visitObject(child);
        }
        depth--;
      }
    };

    // Start visiting the layout view.
    visitObject(view);
  }
}
