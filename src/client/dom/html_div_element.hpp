#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLDivElement final : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;
    HTMLDivElement(weak_ptr<Document> ownerDocument) : HTMLElement("DIV", ownerDocument)
    {
    }

  public:
    void createdCallback() override
    {
      HTMLElement::createdCallback();

      style.setProperty("width", "auto");
      style.setProperty("height", "auto");

    }
    void connectedCallback() override;
  };
}
