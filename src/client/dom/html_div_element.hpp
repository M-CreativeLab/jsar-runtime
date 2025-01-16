#pragma once

#include <string>
#include "./html_content2d_element.hpp"

namespace dom
{
  class HTMLDivElement final : public HTMLContent2dElement
  {
  public:
    using HTMLContent2dElement::HTMLContent2dElement;

  public:
    HTMLDivElement(std::shared_ptr<Document> ownerDocument)
        : HTMLContent2dElement("DIV", ownerDocument)
    {
    }
  };
}
