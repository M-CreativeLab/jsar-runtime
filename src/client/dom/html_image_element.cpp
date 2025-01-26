#include <iostream>
#include <optional>
#include <skia/include/codec/SkCodec.h>
#include <skia/include/codec/SkPngDecoder.h>
#include <skia/include/codec/SkJpegDecoder.h>
#include <skia/include/codec/SkWebpDecoder.h>
#include <skia/include/codec/SkGifDecoder.h>
#include <crates/bindings.hpp>

#include "./geometry/dom_rect.hpp"
#include "./html_image_element.hpp"
#include "./document.hpp"
#include "./browsing_context.hpp"

namespace dom
{
  using namespace std;
  using namespace builtin_scene;
  using namespace crates::layout::style;

  // Adjust the image size based on the adopted style.
  bool adjustImageSize(const client_cssom::CSSStyleDeclaration &adoptedStyle,
                       geometry::DOMRect srcImageRect,
                       optional<client_cssom::Layout> &lastComputedLayout,
                       function<void(float)> widthSetter,
                       function<void(float)> heightSetter)
  {
    optional<Dimension> adoptedWidth = nullopt;
    optional<Dimension> adoptedHeight = nullopt;

    if (adoptedStyle.hasProperty("width"))
    {
      auto widthDimension = adoptedStyle.getPropertyValueAs<Dimension>("width");
      if (!widthDimension.isAuto() &&
          lastComputedLayout.has_value() &&
          lastComputedLayout->width() > 0)
        adoptedWidth = widthDimension;
    }
    if (adoptedStyle.hasProperty("height"))
    {
      auto heightDimension = adoptedStyle.getPropertyValueAs<Dimension>("height");
      if (!heightDimension.isAuto() &&
          lastComputedLayout.has_value() &&
          lastComputedLayout->height() > 0)
        adoptedHeight = heightDimension;
    }

    // If both width and height are specified, then use them.
    if (adoptedWidth.has_value() && adoptedHeight.has_value())
      return false;

    // If both width and height are auto, then use the image's size.
    if (!adoptedWidth.has_value() && !adoptedHeight.has_value())
    {
      widthSetter(srcImageRect.width());
      heightSetter(srcImageRect.height());
      return true;
    }

    const float aspectRatio = srcImageRect.width() / srcImageRect.height();
    if (adoptedWidth.has_value() && !adoptedHeight.has_value())
    {
      // Calculate the height = width / aspectRatio.
      heightSetter(lastComputedLayout->width() / aspectRatio);
      return true;
    }
    else if (!adoptedWidth.has_value() && adoptedHeight.has_value())
    {
      // Calculate the width = height * aspectRatio.
      widthSetter(lastComputedLayout->height() * aspectRatio);
      return true;
    }

    assert(false && "Unreachable");
    return false;
  }

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

  void HTMLImageElement::onLayoutChanged()
  {
    HTMLContent2dElement::onLayoutChanged();

    // Adjust the image size if the layout is to be changed.
    if (isSrcImageLoaded_)
    {
      auto setWidth = [this](float width)
      { defaultBoundingBox_.width = width; };
      auto setHeight = [this](float height)
      { defaultBoundingBox_.height = height; };
      if (adjustImageSize(
              adoptedStyle_, getImageClientRect(), computedLayout_, setWidth, setHeight))
      {
        // Update the layout style if the size is changed.
        updateLayoutStyle();
      }
    }
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
        SkWebpDecoder::Decoder(),
        SkGifDecoder::Decoder()};

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

      // Adjust the image size and update the layout style if the image is loaded.
      {
        auto setWidth = [this](float width)
        { defaultBoundingBox_.width = width; };
        auto setHeight = [this](float height)
        { defaultBoundingBox_.height = height; };
        if (adjustImageSize(
                adoptedStyle_, getImageClientRect(), computedLayout_, setWidth, setHeight))
        {
          // Update the layout style if the size is changed.
          updateLayoutStyle();
        }
      }

      // Update the `Image2d` component and mark the content as dirty to update the texture.
      auto updateImageBitmap = [this](Scene &scene)
      {
        auto entity = entity_.value();
        Image2d &imageComponent = scene.getComponentChecked<Image2d>(entity);
        imageComponent.bitmap = skBitmap_;

        WebContent &webContent = scene.getComponentChecked<WebContent>(entity);
        webContent.setDirty(true); // Mark the content as dirty to update the texture.
      };
      useScene(updateImageBitmap);
    }
    else
    {
      std::cerr << "Failed to decode the image: " << getSrc() << std::endl;
      dispatchEvent(DOMEventType::Error);
    }
  }
}
