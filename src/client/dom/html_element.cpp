#include "./html_element.hpp"
#include "./document.hpp"

namespace dom
{
  void HTMLElement::focus()
  {
  }

  void HTMLElement::connectedCallback()
  {
    auto ownerDocumentRef = ownerDocument->lock();
    assert(ownerDocumentRef != nullptr);
  }
}
