#include <iostream>
#include <skia/include/codec/SkCodec.h>
#include "./html_image_element.hpp"
#include "./document.hpp"
#include "./rendering_context.hpp"

namespace dom
{
  void HTMLImageElement::loadImage(const std::string &src)
  {
    auto renderingContext = ownerDocument->lock()->renderingContext;
    renderingContext->fetchImageResource(src, [this](const void *imageData, size_t imageByteLength)
                                         { onImageLoaded(imageData, imageByteLength); });
  }

  void HTMLImageElement::onImageLoaded(const void *imageData, size_t imageByteLength)
  {
    if (imageData == nullptr)
    {
      dispatchEvent(DOMEventType::Error);
      return;
    }

    auto codec = SkCodec::MakeFromData(SkData::MakeWithoutCopy(imageData, imageByteLength));
    if (codec)
    {
      SkImageInfo info = codec->getInfo().makeColorType(kN32_SkColorType);
      skBitmap->allocPixels(info);
      codec->getPixels(info, skBitmap->getPixels(), skBitmap->rowBytes());
      complete = true;
      dispatchEvent(DOMEventType::Load);
    }
    else
    {
      dispatchEvent(DOMEventType::Error);
    }
  }
}
