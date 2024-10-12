#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLParagraphElement : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;
    HTMLParagraphElement(weak_ptr<Document> ownerDocument) : HTMLElement("P", ownerDocument)
    {
    }
  };
}
