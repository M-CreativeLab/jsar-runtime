#include <client/dom/element.hpp>
#include <client/dom/html_element.hpp>
#include <client/dom/document.hpp>
#include "./window.hpp"

namespace browser
{
  using namespace std;
  using namespace client_cssom;

  Window::Window(TrClientContextPerProcess *clientContext)
      : dom::DOMEventTarget(),
        clientContext_(clientContext)
  {
    assert(clientContext_ != nullptr);
  }

  const CSSStyleDeclaration Window::getComputedStyle(shared_ptr<dom::Element> element,
                                                     optional<string> pseudoElt) const
  {
    auto htmlElement = dynamic_pointer_cast<dom::HTMLElement>(element);
    if (htmlElement == nullptr)
      return CSSStyleDeclaration();

    // const auto& stylesheets = htmlElement->getOwnerDocumentChecked().styleSheets();
    // TODO: Implement the computed style.
    return *htmlElement->style;
  }
}
