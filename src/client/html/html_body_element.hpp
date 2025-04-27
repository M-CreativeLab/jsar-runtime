#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLBodyElement final : public HTMLElement
  {
    using HTMLElement::HTMLElement;

  public:
    HTMLBodyElement(std::shared_ptr<Document> ownerDocument)
        : HTMLElement("BODY", ownerDocument)
    {
    }

  public:
    void createdCallback() override
    {
      defaultStyle_.setProperty("width", "auto");
      defaultStyle_.setProperty("height", "auto");
      defaultStyle_.setProperty("min-width", "100vw");
      defaultStyle_.setProperty("min-height", "100vh");
      defaultStyle_.setProperty("background-color", "transparent");
      renderable = true;

      HTMLElement::createdCallback();
    }
  };
}
