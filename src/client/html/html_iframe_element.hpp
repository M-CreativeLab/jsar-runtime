#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLIframeElement final : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;

  public:
    HTMLIframeElement(std::shared_ptr<Document> ownerDocument)
        : HTMLElement("IFRAME", ownerDocument)
    {
    }

  private:
    void createdCallback(bool from_scripting) override;
  };
}
