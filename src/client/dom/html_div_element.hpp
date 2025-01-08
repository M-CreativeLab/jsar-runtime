#pragma once

#include <string>
#include "./html_plane_element.hpp"

namespace dom
{
  class HTMLDivElement final : public HTMLPlaneElement
  {
  public:
    using HTMLPlaneElement::HTMLPlaneElement;
    HTMLDivElement(weak_ptr<Document> ownerDocument) : HTMLPlaneElement("DIV", ownerDocument)
    {
    }
  };
}
