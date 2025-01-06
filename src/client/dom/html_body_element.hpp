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
      defaultStyle_.setProperty("width", "100%");
      defaultStyle_.setProperty("height", "100%");
      defaultStyle_.setProperty("margin", "2px");
      defaultStyle_.setProperty("padding", "2px");

      HTMLElement::createdCallback();
    }
  };
}
