#include <iostream>
#include "common/media/message.hpp"
#include "./html_media_element.hpp"
#include "./document.hpp"
#include "./rendering_context.hpp"

namespace dom
{
  void HTMLMediaElement::loadMedia(const std::string &src)
  {
    auto renderingContext = ownerDocument->lock()->renderingContext;
    renderingContext->fetchImageResource(src, [this](const void *data, size_t byteLength)
                                         { onMediaLoaded(data, byteLength); });
  }

  void HTMLMediaElement::onMediaLoaded(const void *mediaData, size_t mediaByteLength)
  {
    player_->setSrc((void *)mediaData, mediaByteLength);
    player_->load();
  }

  void HTMLMediaElement::onMediaEvent(media_comm::TrMediaEventType eventType, media_client::MediaEvent &event)
  {
    if (eventType == media_comm::TrMediaEventType::LoadedData && autoPlay == true)
      player_->play();

    if (eventCallback_ != nullptr)
      eventCallback_(eventType, event);

    switch (eventType)
    {
#define XX(typeId, nameStr)                   \
  case media_comm::TrMediaEventType::typeId:  \
    dispatchEvent(dom::DOMEventType::typeId); \
    break;

      TR_MEDIA_EVENT_TYPES_MAP(XX)
#undef XX
    default:
      break;
    }
  }
}
