#include <iostream>
#include <common/media/message.hpp>
#include <client/dom/document.hpp>
#include <client/dom/browsing_context.hpp>

#include "./html_media_element.hpp"

namespace dom
{
  using namespace std;

  void HTMLMediaElement::createdCallback()
  {
    HTMLElement::createdCallback();

    assert(clientContext != nullptr && "The client context is not initialized.");
    player_ = clientContext->createMediaPlayer(contentType_);
    player_->resetGlobalEventListener([this](auto eventType, auto event)
                                      { onMediaEvent(eventType, event); });
  }

  void HTMLMediaElement::loadMedia(const string &src)
  {
    auto browsingContext = ownerDocument->lock()->browsingContext;
    browsingContext->fetchImageResource(src, [this](const void *data, size_t byteLength)
                                        { onMediaLoaded(data, byteLength); });
  }

  void HTMLMediaElement::onMediaLoaded(const void *mediaData, size_t mediaByteLength)
  {
    assert(player_ != nullptr && "The media player is not initialized.");
    player_->setSrc((void *)mediaData, mediaByteLength);
    player_->load();
  }

  void HTMLMediaElement::onMediaEvent(media_comm::TrMediaEventType eventType, shared_ptr<media_client::MediaEvent> event)
  {
    if (eventType == media_comm::TrMediaEventType::LoadedMetadata)
      readyState = MediaReadyState::HAVE_METADATA;
    else if (eventType == media_comm::TrMediaEventType::LoadedData)
    {
      readyState = MediaReadyState::HAVE_ENOUGH_DATA;
      if (autoPlay || playScheduled_)
        player_->play();
    }

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
