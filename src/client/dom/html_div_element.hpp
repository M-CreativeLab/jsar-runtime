#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLDivElement : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;
    HTMLDivElement(weak_ptr<Document> ownerDocument) : HTMLElement("DIV", ownerDocument)
    {
    }

  public:
    void connectedCallback() override;
  };
}
