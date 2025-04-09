#include <iostream>
#include <optional>
#include <skia/include/codec/SkCodec.h>
#include <skia/include/codec/SkPngDecoder.h>
#include <skia/include/codec/SkJpegDecoder.h>
#include <skia/include/codec/SkWebpDecoder.h>
#include <skia/include/codec/SkGifDecoder.h>
#include <crates/bindings.hpp>
#include <client/cssom/layout.hpp>
#include <client/layout/layout_image.hpp>

#include "./geometry/dom_rect.hpp"
#include "./html_image_element.hpp"
#include "./document.hpp"
#include "./browsing_context.hpp"

namespace dom
{
  using namespace std;
  using namespace builtin_scene;
  using namespace crates::layout2::styles;

  void HTMLImageElement::connectedCallback()
  {
    HTMLElement::connectedCallback();
    skBitmap_ = std::make_shared<SkBitmap>();

    // Load the image from the src attribute.
    auto src = getSrc();
    if (!src.empty())
      loadImage(src);
  }

  void HTMLImageElement::loadImage(const std::string &src)
  {
    if (isSrcImageLoaded_)
      return;
    auto browsingContext = ownerDocument->lock()->browsingContext;
    browsingContext->fetchImageResource(src, [this](const void *imageData, size_t imageByteLength)
                                        { onImageLoaded(imageData, imageByteLength); });
  }

  // Decode the image data to the output `SkBitmap`.
  inline bool DecodeImage(const void *imageData, size_t imageByteLength, SkBitmap &bitmap)
  {
    static constexpr const SkCodecs::Decoder decoders[] = {
        SkPngDecoder::Decoder(),
        SkJpegDecoder::Decoder(),
        SkWebpDecoder::Decoder(),
        SkGifDecoder::Decoder()};

    unique_ptr<SkCodec> codec = SkCodec::MakeFromData(SkData::MakeWithoutCopy(imageData, imageByteLength), decoders);
    if (codec)
    {
      try
      {
        SkImageInfo info = codec->getInfo().makeColorType(kN32_SkColorType);
        bitmap.allocPixels(info);
        codec->getPixels(info, bitmap.getPixels(), bitmap.rowBytes());
        return true;
      }
      catch (const std::exception &e)
      {
        std::cerr << "Failed to decode the image: " << e.what() << std::endl;
        return false;
      }
    }
    else
    {
      return false;
    }
  }

  void HTMLImageElement::onImageLoaded(const void *imageData, size_t imageByteLength)
  {
    // Mark the image as loaded.
    isSrcImageLoaded_ = true;

    // Dispatch the error event if the image data is null.
    if (imageData == nullptr)
    {
      dispatchEvent(DOMEventType::Error);
      return;
    }

    // Instantiate the `SkBitmap` if it is not created.
    //
    // ```js
    // const image = new Image('...');
    // image.onload = () => { ... };
    // ```
    //
    // The above code snippet creates a new `Image` object without connecting it to the DOM, and it's allowed to load
    // the image data without the `connectedCallback` being called. In this case, the `skBitmap_` is not created yet.
    if (skBitmap_ == nullptr)
      skBitmap_ = std::make_shared<SkBitmap>();

    if (DecodeImage(imageData, imageByteLength, *skBitmap_))
    {
      complete = true;
      dispatchEvent(DOMEventType::Load);

      // When the image is connected to the DOM, set the image bitmap to the layout image box.
      if (connected)
      {
        auto imageBox = dynamic_pointer_cast<client_layout::LayoutImage>(principalBox());
        assert(imageBox != nullptr && "The image box is not created yet.");
        imageBox->setImageBitmap(skBitmap_);
      }
    }
    else
    {
      std::cerr << "Failed to decode the image: " << getSrc() << std::endl;
      dispatchEvent(DOMEventType::Error);

      // TODO(yorkie): paint a placeholder image.
    }
  }
}
