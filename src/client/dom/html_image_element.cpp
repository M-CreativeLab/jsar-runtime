#include <iostream>
#include <skia/include/codec/SkCodec.h>
#include <skia/include/codec/SkPngDecoder.h>
#include <skia/include/codec/SkJpegDecoder.h>
#include "./html_image_element.hpp"
#include "./document.hpp"
#include "./browsing_context.hpp"

namespace dom
{
  void HTMLImageElement::loadImage(const std::string &src)
  {
    auto browsingContext = ownerDocument->lock()->browsingContext;
    browsingContext->fetchImageResource(src, [this](const void *imageData, size_t imageByteLength)
                                         { onImageLoaded(imageData, imageByteLength); });
  }

  void HTMLImageElement::onImageLoaded(const void *imageData, size_t imageByteLength)
  {
    static constexpr const SkCodecs::Decoder decoders[] = {
      SkPngDecoder::Decoder(),
      SkJpegDecoder::Decoder(),
    };

    if (imageData == nullptr)
    {
      dispatchEvent(DOMEventType::Error);
      return;
    }

    auto codec = SkCodec::MakeFromData(SkData::MakeWithoutCopy(imageData, imageByteLength), decoders);
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
