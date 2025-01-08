#pragma once

#include <string>
#include "./html_plane_element.hpp"

namespace dom
{
  class HTMLSpanElement : public HTMLPlaneElement
  {
  public:
    using HTMLPlaneElement::HTMLPlaneElement;
    HTMLSpanElement(weak_ptr<Document> ownerDocument) : HTMLPlaneElement("SPAN", ownerDocument)
    {
    }
  };
}
