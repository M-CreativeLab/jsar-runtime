#pragma once

#include <string>
#include "./html_content2d_element.hpp"

namespace dom
{
  class HTMLBodyElement final : public HTMLContent2dElement
  {
  public:
    using HTMLContent2dElement::HTMLContent2dElement;

  public:
    HTMLBodyElement(std::shared_ptr<Document> ownerDocument)
        : HTMLContent2dElement("BODY", ownerDocument)
    {
    }

  public:
    void createdCallback() override
    {
      defaultStyle_.setProperty("width", "100%");
      defaultStyle_.setProperty("height", "100%");
      defaultStyle_.setProperty("margin", "2px");
      defaultStyle_.setProperty("padding", "2px");

      HTMLContent2dElement::createdCallback();
    }
  };
}
