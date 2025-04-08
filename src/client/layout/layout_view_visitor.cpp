#include "./layout_view_visitor.hpp"
#include "./layout_object.hpp"
#include "./layout_text.hpp"
#include "./layout_box.hpp"
#include "./layout_block.hpp"
#include "./layout_inline.hpp"

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
      if (object.isBoxModelObject()) // block, inline or any other objects based on box model.
        onVisitBox(dynamic_cast<const LayoutBoxModelObject &>(object), depth);

      shared_ptr<client_layout::LayoutObjectChildList> children = nullptr;
      if (object.isLayoutBlock())
      {
        auto &block = dynamic_cast<LayoutBlock &>(object);
        onVisitBlock(block, depth);
        children = block.children();
      }
      else if (object.isLayoutInline())
      {
        auto &inlineBox = dynamic_cast<LayoutInline &>(object);
        onVisitInline(inlineBox, depth);
        children = inlineBox.children();
      }

      if (children != nullptr)
      {
        client_layout::LayoutObjectChildList &childrenRef = *children;
        depth++;
        for (auto child : childrenRef)
        {
          assert(child != nullptr && child->parent() != nullptr &&
                 "The child must have a parent.");
          visitObject(*child);
        }
        depth--;
      }
    };

    // Start visiting the layout view.
    visitObject(view);
  }
}
