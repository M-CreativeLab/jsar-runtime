#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLParagraphElement : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;

  public:
    HTMLParagraphElement(std::shared_ptr<Document> ownerDocument)
        : HTMLElement("P", ownerDocument)
    {
    }

  public:
    void createdCallback(bool from_scripting) override
    {
      defaultStyle_.setProperty("display", "block");
      defaultStyle_.setProperty("margin", "10px 0");
      defaultStyle_.setProperty("height", "auto");
      defaultStyle_.setProperty("width", "auto");

      HTMLElement::createdCallback(from_scripting);
    }
  };
}
