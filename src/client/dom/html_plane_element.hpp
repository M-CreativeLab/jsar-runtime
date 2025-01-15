#pragma once

#include <string>
#include <optional>
#include "./html_element.hpp"

namespace dom
{
  class HTMLPlaneElement : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;

  public:
    HTMLPlaneElement(std::shared_ptr<Document> ownerDocument)
        : HTMLElement("PLANE", ownerDocument)
    {
    }
    HTMLPlaneElement(std::string tagName, std::shared_ptr<Document> ownerDocument)
        : HTMLElement(tagName, ownerDocument)
    {
    }

  public:
    void connectedCallback() override;
  };
}
