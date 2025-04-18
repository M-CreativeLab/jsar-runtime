#include "./layout_inline.hpp"
#include "./layout_view.hpp"

namespace client_layout
{
  using namespace std;

  LayoutInline::LayoutInline(shared_ptr<dom::Node> node)
      : LayoutBoxModelObject(node),
        children_(make_shared<LayoutObjectChildList>())
  {
  }

  shared_ptr<LayoutObject> LayoutInline::firstChild() const
  {
    assert(children() != nullptr && "The children list is not set.");
    assert(children() == virtualChildren());
    return children()->firstChild();
  }

  shared_ptr<LayoutObject> LayoutInline::lastChild() const
  {
    assert(children() != nullptr && "The children list is not set.");
    assert(children() == virtualChildren());
    return children()->lastChild();
  }
}
