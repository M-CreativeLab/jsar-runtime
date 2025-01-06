#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLBodyElement final : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;
    HTMLBodyElement(weak_ptr<Document> ownerDocument) : HTMLElement("BODY", ownerDocument)
    {
    }

  public:
    void createdCallback() override
    {
      HTMLElement::createdCallback();

      style.setProperty("width", "100%");
      style.setProperty("height", "100%");
      style.setProperty("margin", "2px");
      style.setProperty("padding", "2px");
    }
  };
}
