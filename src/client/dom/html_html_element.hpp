#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLHtmlElement : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;
    HTMLHtmlElement(std::shared_ptr<Document> ownerDocument) : HTMLElement("HTML", ownerDocument)
    {
    }
  };
}
