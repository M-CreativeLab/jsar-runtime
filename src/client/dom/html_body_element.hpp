#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLBodyElement : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;
    HTMLBodyElement(weak_ptr<Document> ownerDocument) : HTMLElement("BODY", ownerDocument)
    {
    }
  };
}
