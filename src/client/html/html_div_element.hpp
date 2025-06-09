#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLDivElement final : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;

  public:
    HTMLDivElement(std::shared_ptr<Document> ownerDocument)
        : HTMLElement("DIV", ownerDocument)
    {
    }

  private:
    void createdCallback(bool from_scripting) override;
  };
}
