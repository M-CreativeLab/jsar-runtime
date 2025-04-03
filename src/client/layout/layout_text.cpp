#include <client/dom/text.hpp>
#include "./layout_text.hpp"

namespace client_layout
{
  using namespace std;

  LayoutText::LayoutText(shared_ptr<dom::Text> textNode)
      : LayoutObject(textNode)
  {
  }

  shared_ptr<dom::Text> LayoutText::textNode() const
  {
    return static_pointer_cast<dom::Text>(node());
  }

  string LayoutText::plainText() const
  {
    // TODO: support offset such as <div>foo<b>!</b>bar</div>, it should created as 2 `LayoutText` objects.
    return textNode()->data();
  }
}
