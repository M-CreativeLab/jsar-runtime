#pragma once

#include <string>
#include "./html_content2d_element.hpp"

namespace dom
{
  class HTMLParagraphElement : public HTMLContent2dElement
  {
  public:
    using HTMLContent2dElement::HTMLContent2dElement;

  public:
    HTMLParagraphElement(std::shared_ptr<Document> ownerDocument)
        : HTMLContent2dElement("P", ownerDocument)
    {
    }

  public:
    void createdCallback() override
    {
      defaultStyle_.setProperty("display", "block");
      defaultStyle_.setProperty("margin", "10px 0");
      defaultStyle_.setProperty("height", "auto");
      defaultStyle_.setProperty("width", "auto");

      HTMLContent2dElement::createdCallback();
    }
  };
}
