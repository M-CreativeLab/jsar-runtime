#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLHeadElement : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;
    HTMLHeadElement(weak_ptr<Document> ownerDocument) : HTMLElement("HEAD", ownerDocument)
    {
    }
  };
}
