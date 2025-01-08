#pragma once

#include <string>
#include "./html_plane_element.hpp"

namespace dom
{
  class HTMLParagraphElement : public HTMLPlaneElement
  {
  public:
    using HTMLPlaneElement::HTMLPlaneElement;
    HTMLParagraphElement(weak_ptr<Document> ownerDocument) : HTMLPlaneElement("P", ownerDocument)
    {
    }
  };
}
