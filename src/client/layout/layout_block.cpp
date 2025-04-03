#include "./layout_block.hpp"

namespace client_layout
{
  using namespace std;

  LayoutBlock::LayoutBlock(shared_ptr<dom::Node> node)
      : LayoutBox(node),
        children_(make_shared<LayoutObjectChildList>())
  {
  }

  shared_ptr<LayoutObject> LayoutBlock::firstChild() const
  {
    assert(children() != nullptr && "The children list is not set.");
    assert(children() == virtualChildren());
    return children()->firstChild();
  }

  shared_ptr<LayoutObject> LayoutBlock::lastChild() const
  {
    assert(children() != nullptr && "The children list is not set.");
    assert(children() == virtualChildren());
    return children()->lastChild();
  }
}
