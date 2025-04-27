#pragma once

#include <string>
#include "./html_media_element.hpp"

namespace dom
{
  class HTMLVideoElement : public HTMLMediaElement
  {
  public:
    using HTMLMediaElement::HTMLMediaElement;

  public:
    HTMLVideoElement(std::shared_ptr<Document> ownerDocument)
        : HTMLMediaElement("VIDEO", media_comm::MediaContentType::Video, ownerDocument)
    {
    }
  };
}
