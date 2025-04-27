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
    void createdCallback() override
    {
      defaultStyle_.setProperty("display", "inline");
      HTMLElement::createdCallback();
    }
  };
}
