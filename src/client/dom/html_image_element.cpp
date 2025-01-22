#include <iostream>
#include <skia/include/codec/SkCodec.h>
#include <skia/include/codec/SkPngDecoder.h>
#include <skia/include/codec/SkJpegDecoder.h>
#include "./html_image_element.hpp"
#include "./document.hpp"
#include "./browsing_context.hpp"

namespace dom
{
  using namespace builtin_scene;

  void HTMLImageElement::connectedCallback()
  {
    HTMLContent2dElement::connectedCallback();
    skBitmap_ = std::make_shared<SkBitmap>();

    // Add the Image2d component to the entity.
    assert(entity_.has_value());
    auto addImageComponent = [this](Scene &scene)
    {
      scene.addComponent(entity_.value(),
                         Image2d(getSrc(), nullptr));
    };
    useScene(addImageComponent);

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

  void HTMLImageElement::onImageLoaded(const void *imageData, size_t imageByteLength)
  {
    static constexpr const SkCodecs::Decoder decoders[] = {
        SkPngDecoder::Decoder(),
        SkJpegDecoder::Decoder(),
    };

    // Mark the image as loaded.
    isSrcImageLoaded_ = true;

    // Dispatch the error event if the image data is null.
    if (imageData == nullptr)
    {
      dispatchEvent(DOMEventType::Error);
      return;
    }

    auto codec = SkCodec::MakeFromData(SkData::MakeWithoutCopy(imageData, imageByteLength), decoders);
    if (codec)
    {
      SkImageInfo info = codec->getInfo().makeColorType(kN32_SkColorType);
      skBitmap_->allocPixels(info);
      codec->getPixels(info, skBitmap_->getPixels(), skBitmap_->rowBytes());
      complete = true;
      dispatchEvent(DOMEventType::Load);

      // Update the Image2d component and mark the content as dirty to update the texture.
      auto updateImageBitmap = [this](Scene &scene)
      {
        auto entity = entity_.value();
        Image2d& imageComponent = scene.getComponentChecked<Image2d>(entity);
        imageComponent.bitmap = skBitmap_;

        WebContent& webContent = scene.getComponentChecked<WebContent>(entity);
        webContent.setDirty(true);  // Mark the content as dirty to update the texture.
      };
      useScene(updateImageBitmap);
    }
    else
    {
      dispatchEvent(DOMEventType::Error);
    }
  }
}
