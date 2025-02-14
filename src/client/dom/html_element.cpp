#include <client/cssom/units.hpp>
#include "./html_element.hpp"
#include "./document-inl.hpp"

namespace dom
{
  using namespace std;
  using LayoutAllocator = crates::layout2::Allocator;
  using LayoutNode = crates::layout2::Node;

  // TODO: Implement the following methods.
  void HTMLElement::blur() {}
  void HTMLElement::focus() {}
  void HTMLElement::click() {}

  void HTMLElement::createdCallback()
  {
    Element::createdCallback();

    // Create style declaration from the default style & the style attribute.
    style_ = make_shared<client_cssom::CSSStyleDeclaration>(getAttribute("style"));
  }

  void HTMLElement::connectedCallback()
  {
  }
}
