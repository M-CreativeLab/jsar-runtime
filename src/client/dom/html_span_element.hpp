#pragma once

#include <string>
#include "./html_content2d_element.hpp"

namespace dom
{
  class HTMLSpanElement : public HTMLContent2dElement
  {
  public:
    using HTMLContent2dElement::HTMLContent2dElement;

  public:
    HTMLSpanElement(std::shared_ptr<Document> ownerDocument)
        : HTMLContent2dElement("SPAN", ownerDocument)
    {
    }

  public:
    void createdCallback() override
    {
      defaultStyle_.setProperty("display", "inline");
      defaultStyle_.setProperty("height", "auto");
      defaultStyle_.setProperty("margin", "auto");

      HTMLContent2dElement::createdCallback();
    }
  };
}
