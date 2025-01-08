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
    HTMLPlaneElement(weak_ptr<Document> ownerDocument) : HTMLElement("PLANE", ownerDocument)
    {
    }
    HTMLPlaneElement(std::string tagName, weak_ptr<Document> ownerDocument) : HTMLElement(tagName, ownerDocument)
    {
    }

  public:
    void connectedCallback() override;
  };
}
