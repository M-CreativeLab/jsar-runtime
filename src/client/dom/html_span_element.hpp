#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLSpanElement : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;
    HTMLSpanElement(weak_ptr<Document> ownerDocument) : HTMLElement("SPAN", ownerDocument)
    {
    }
  };
}
