#pragma once

#include <string>
#include "./html_content_element.hpp"

namespace dom
{
  class HTMLDivElement final : public HTMLContentElement
  {
  public:
    using HTMLContentElement::HTMLContentElement;

  public:
    HTMLDivElement(std::shared_ptr<Document> ownerDocument)
        : HTMLContentElement("DIV", ownerDocument)
    {
    }
  };
}
