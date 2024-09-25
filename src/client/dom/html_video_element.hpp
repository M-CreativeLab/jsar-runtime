#pragma once

#include <string>
#include "./html_media_element.hpp"

namespace dom
{
  class HTMLVideoElement : public HTMLMediaElement
  {
  public:
    using HTMLMediaElement::HTMLMediaElement;
    HTMLVideoElement(weak_ptr<Document> ownerDocument)
        : HTMLMediaElement("VIDEO", ownerDocument)
    {
    }
  };
}
