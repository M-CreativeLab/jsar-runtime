#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLSpanElement : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;

  public:
    HTMLSpanElement(std::shared_ptr<Document> ownerDocument)
        : HTMLElement("SPAN", ownerDocument)
    {
    }

  public:
    void createdCallback(bool from_scripting) override
    {
      defaultStyle_.setProperty("display", "inline");
      HTMLElement::createdCallback(from_scripting);
    }
  };
}
