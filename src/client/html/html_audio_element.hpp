#pragma once

#include <string>
#include "./html_media_element.hpp"

namespace dom
{
  class HTMLAudioElement : public HTMLMediaElement
  {
  public:
    using HTMLMediaElement::HTMLMediaElement;

  public:
    HTMLAudioElement(std::shared_ptr<Document> ownerDocument)
        : HTMLMediaElement("AUDIO", media_comm::MediaContentType::Audio, ownerDocument)
    {
    }
  };
}
