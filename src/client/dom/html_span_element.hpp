#pragma once

#include <string>
#include "./html_plane_element.hpp"

namespace dom
{
  class HTMLSpanElement : public HTMLPlaneElement
  {
  public:
    using HTMLPlaneElement::HTMLPlaneElement;

  public:
    HTMLSpanElement(std::shared_ptr<Document> ownerDocument)
        : HTMLPlaneElement("SPAN", ownerDocument)
    {
    }
  };
}
