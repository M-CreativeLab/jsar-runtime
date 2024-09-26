#pragma once

#include <string>
#include "./html_media_element.hpp"

namespace dom
{
  class HTMLAudioElement : public HTMLMediaElement
  {
  public:
    using HTMLMediaElement::HTMLMediaElement;
    HTMLAudioElement(weak_ptr<Document> ownerDocument)
        : HTMLMediaElement("AUDIO", media_comm::MediaContentType::Audio, ownerDocument)
    {
    }
  };
}
