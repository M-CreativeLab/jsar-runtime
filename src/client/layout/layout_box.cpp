#include <client/dom/node.hpp>
#include "./layout_box.hpp"

namespace client_layout
{
  using namespace std;

  LayoutBox::LayoutBox(shared_ptr<dom::Node> node)
      : LayoutBoxModelObject(node)
  {
  }

  shared_ptr<LayoutBox> LayoutBox::firstChildBox() const
  {
    return dynamic_pointer_cast<LayoutBox>(slowFirstChild());
  }

  shared_ptr<LayoutBox> LayoutBox::lastChildBox() const
  {
    return dynamic_pointer_cast<LayoutBox>(slowLastChild());
  }

  shared_ptr<LayoutBox> LayoutBox::nextSiblingBox() const
  {
    return dynamic_pointer_cast<LayoutBox>(nextSibling());
  }

  shared_ptr<LayoutBox> LayoutBox::previousSiblingBox() const
  {
    return dynamic_pointer_cast<LayoutBox>(prevSibling());
  }

  shared_ptr<LayoutBox> LayoutBox::parentBox() const
  {
    return dynamic_pointer_cast<LayoutBox>(parent());
  }
}
