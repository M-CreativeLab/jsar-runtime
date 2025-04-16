#include <iostream>
#include <optional>
#include <skia/include/codec/SkCodec.h>
#include <skia/include/codec/SkPngDecoder.h>
#include <skia/include/codec/SkJpegDecoder.h>
#include <skia/include/codec/SkWebpDecoder.h>
#include <skia/include/codec/SkGifDecoder.h>
#include <crates/bindings.hpp>
#include <client/per_process.hpp>
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
    sk_bitmap_ = make_shared<SkBitmap>();

    load_async_handle_.data = this;
    uv_async_init(TrClientContextPerProcess::Get()->getScriptingEventLoop(), &load_async_handle_,
                  [](uv_async_t *handle)
                  {
                    auto imageElement = static_cast<HTMLImageElement *>(handle->data);
                    imageElement->loadImage();
                  });
  }

  void HTMLImageElement::loadImage()
  {
    if (is_src_image_loaded_)
      return;
    fetchImage(getSrc());
  }

  void HTMLImageElement::loadImageAsync()
  {
    if (is_src_image_loaded_ || load_async_handle_.data != this)
      return;

    // Schedule the image loading on the scripting thread.
    uv_async_send(&load_async_handle_);
  }

  void HTMLImageElement::fetchImage(const string &src)
  {
    auto browsingContext = ownerDocument->lock()->browsingContext;
    auto responseCallback = [this](const void *imageData, size_t imageByteLength)
    {
      image_data_ = vector<char>(imageByteLength);
      image_data_->assign(static_cast<const char *>(imageData),
                          static_cast<const char *>(imageData) + imageByteLength);
      onImageDataReady();
    };
    browsingContext->fetchImageResource(src, responseCallback);
  }

  bool HTMLImageElement::decodeImage(SkBitmap &bitmap)
  {
    static constexpr const SkCodecs::Decoder decoders[] = {
        SkPngDecoder::Decoder(),
        SkJpegDecoder::Decoder(),
        SkWebpDecoder::Decoder(),
        SkGifDecoder::Decoder()};

    if (is_src_image_decoded_)
      return true;

    sk_sp<SkData> imageData = SkData::MakeWithoutCopy(image_data_->data(), image_data_->size());
    unique_ptr<SkCodec> codec = SkCodec::MakeFromData(imageData, decoders);
    if (codec)
    {
      try
      {
        SkImageInfo info = codec->getInfo().makeColorType(kN32_SkColorType);
        bitmap.allocPixels(info);
        codec->getPixels(info, bitmap.getPixels(), bitmap.rowBytes());
        is_src_image_decoded_ = true;
        return true;
      }
      catch (const exception &e)
      {
        cerr << "Failed to decode the image: " << e.what() << endl;
        return false;
      }
    }
    else
    {
      return false;
    }
  }

  void HTMLImageElement::onImageDataReady()
  {
    // Mark the image as loaded.
    is_src_image_loaded_ = true;

    // Dispatch the error event if the image data is null.
    if (image_data_ == nullopt)
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
    // the image data without the `connectedCallback` being called. In this case, the `sk_bitmap_` is not created yet.
    if (sk_bitmap_ == nullptr)
      sk_bitmap_ = make_shared<SkBitmap>();
    assert(sk_bitmap_ != nullptr && "The image bitmap is not created yet.");

    if (decodeImage(*sk_bitmap_))
    {
      onImageDecoded(*sk_bitmap_);

      complete = true;
      dispatchEvent(DOMEventType::Load);
    }
    else
    {
      cerr << "Failed to decode the image: " << getSrc() << endl;
      dispatchEvent(DOMEventType::Error);

      // TODO(yorkie): paint a placeholder image.
    }
  }

  void HTMLImageElement::onImageDecoded(const SkBitmap &bitmap)
  {
    if (!connected)
      return;

    auto imageBox = dynamic_pointer_cast<client_layout::LayoutImage>(principalBox());
    assert(imageBox != nullptr && "The image box is not created yet.");
    imageBox->setImageBitmap(sk_bitmap_);
  }
}
