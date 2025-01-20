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
      HTMLContent2dElement::createdCallback();
    }
  };
}
