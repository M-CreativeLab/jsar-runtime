#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLBodyElement final : public HTMLElement
  {
    using HTMLElement::HTMLElement;

  public:
    HTMLBodyElement(std::shared_ptr<Document> ownerDocument)
        : HTMLElement("BODY", ownerDocument)
    {
    }

  public:
    void createdCallback() override
    {
      defaultStyle_.setProperty("width", "100%");
      defaultStyle_.setProperty("height", "100%");
      defaultStyle_.setProperty("margin", "2px");
      defaultStyle_.setProperty("padding", "2px");
      defaultStyle_.setProperty("background-color", "transparent");
      renderable = true;

      HTMLElement::createdCallback();
    }
  };
}
