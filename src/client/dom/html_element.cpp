#include <client/cssom/units.hpp>
#include "./html_element.hpp"
#include "./document-inl.hpp"

namespace dom
{
  using namespace std;
  using LayoutAllocator = crates::layout::Allocator;
  using LayoutNode = crates::layout::Node;

  // TODO: Implement the following methods.
  void HTMLElement::blur() {}
  void HTMLElement::focus() {}
  void HTMLElement::click() {}

  void HTMLElement::createdCallback()
  {
    Element::createdCallback();

    defaultStyle_.setPropertyIfNotPresent("display", "block");
    defaultStyle_.setPropertyIfNotPresent("width", "auto");
    defaultStyle_.setPropertyIfNotPresent("height", "auto");

    // Create style declaration from the default style & the style attribute.
    string concatedCssText = defaultStyle_.cssText() + ";" + getAttribute("style");
    style = make_shared<client_cssom::CSSStyleDeclaration>(concatedCssText);
  }

  void HTMLElement::connectedCallback()
  {
  }

  bool HTMLElement::adoptStyle(const client_cssom::CSSStyleDeclaration &style)
  {
    // Do nothing
    return true;
  }
}
