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
#include <client/dom/browsing_context.hpp>
#include <client/dom/document.hpp>
#include <client/dom/geometry/dom_rect.hpp>
#include <client/layout/layout_image.hpp>

#include "./html_image_element.hpp"

namespace dom
{
  using namespace std;
  using namespace builtin_scene;
  using namespace crates::layout2::styles;

  void HTMLImageElement::createdCallback()
  {
    HTMLElement::createdCallback();

    if (hasAttribute("width"))
      width_ = stoi(getAttribute("width"));
    if (hasAttribute("height"))
      height_ = stoi(getAttribute("height"));
  }

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

  void HTMLImageElement::attributeChangedCallback(const string &name, const string &oldValue, const string &newValue)
  {
    HTMLElement::attributeChangedCallback(name, oldValue, newValue);

    if (name == "src")
    {
      setSrc(newValue);
    }
    else if (name == "width")
    {
      newValue.empty() ? width_ = 0 : width_ = stoi(newValue);
      onSizeDidChange();
    }
    else if (name == "height")
    {
      newValue.empty() ? height_ = 0 : height_ = stoi(newValue);
      onSizeDidChange();
    }
    else if (name == "loading")
    {
      if (newValue == "lazy")
        loading_ = LoadingType::kLoadingLazy;
      else if (newValue == "eager")
        loading_ = LoadingType::kLoadingEager;
      else
        loading_ = LoadingType::kLoadingEager;
    }
    else if (name == "decoding")
    {
      if (newValue == "async")
        decoding_ = DecodingType::kDecodingAsync;
      else if (newValue == "sync")
        decoding_ = DecodingType::kDecodingSync;
      else
        decoding_ = DecodingType::kDecodingAuto;
    }
    else if (name == "ismap")
    {
      is_map_ = true;
    }
    else if (name == "usemap")
    {
      use_map_ = newValue;
    }
  }

  void HTMLImageElement::loadImage()
  {
    if (is_src_image_loading || is_src_image_loaded_)
      return;

    is_src_image_loading = true;
    fetchImage(getSrc());
  }

  void HTMLImageElement::loadImageAsync()
  {
    if (is_src_image_loading ||
        is_src_image_loaded_ ||
        TR_UNLIKELY(load_async_handle_.data != this))
      return;

    // Schedule the image loading on the scripting thread.
    is_src_image_loading = true;
    uv_async_send(&load_async_handle_);
  }

  void HTMLImageElement::fetchImage(const string &src)
  {
    if (src.empty())
    {
      is_src_image_loading = false;
      return;
    }

    assert(ownerDocument->expired() == false && "The owner document is expired.");
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
        cerr << "Failed to decode the image: " << e.what() << endl
             << "    size: " << image_data_->size() << endl
             << "    data: " << (image_data_->data() != nullptr ? "valid" : "(empty)") << endl;
        return false;
      }
    }
    else
    {
      cerr << "Failed to create the image codec, url: " << getSrc() << endl;
      return false;
    }
  }

  void HTMLImageElement::decodeImageAsync(const SkBitmap &bitmap)
  {
    auto work = [](uv_work_t *handle)
    {
      if (handle != nullptr && handle->data != nullptr)
      {
        auto imageElement = static_cast<HTMLImageElement *>(handle->data);
        imageElement->decodeImage(*imageElement->sk_bitmap_);
      }
    };
    auto afterWork = [](uv_work_t *handle, int status)
    {
      if (handle != nullptr && handle->data != nullptr)
      {
        auto imageElement = static_cast<HTMLImageElement *>(handle->data);
        if (imageElement->is_src_image_decoded_)
        {
          imageElement->onImageDecoded(*imageElement->sk_bitmap_);

          // Mark the image is completed.
          imageElement->complete = true;
          imageElement->dispatchEvent(DOMEventType::Load);
        }
        else
        {
          imageElement->dispatchEvent(DOMEventType::Error);
          // TODO(yorkie): paint a placeholder image.
        }
      }
      else
      {
        assert(false);
      }
    };

    // Schedule the image decoding on the scripting thread.
    decode_work_handle_.data = this;
    uv_queue_work(TrClientContextPerProcess::Get()->getScriptingEventLoop(),
                  &decode_work_handle_, work, afterWork);
  }

  void HTMLImageElement::onImageDataReady()
  {
    // Mark the image as loaded.
    is_src_image_loading = false;
    is_src_image_loaded_ = true;

    // Dispatch the error event if the image data is null.
    if (TR_UNLIKELY(image_data_ == nullopt))
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

    decodeImageAsync(*sk_bitmap_);
  }

  void HTMLImageElement::onImageDecoded(const SkBitmap &bitmap)
  {
    // Use natural width and height if the width and height are not set.
    if (!width_.has_value())
      width_ = bitmap.width();
    if (!height_.has_value())
      height_ = bitmap.height();

    if (!connected)
      return;

    auto imageBox = dynamic_pointer_cast<client_layout::LayoutImage>(principalBox());
    assert(imageBox != nullptr && "The image box is not created yet.");
    imageBox->setImageBitmap(sk_bitmap_);
  }

  void HTMLImageElement::onSizeDidChange()
  {
    if (sk_bitmap_ == nullptr && validateSizeToMakeBitmap())
    {
      auto imageInfo = SkImageInfo::MakeN32Premul(width_.value(),
                                                  height_.value());
      // Create a new bitmap with the specified width and height.
      sk_bitmap_ = make_shared<SkBitmap>();
      sk_bitmap_->allocPixels(imageInfo);
    }
  }

  bool HTMLImageElement::validateSizeToMakeBitmap()
  {
    if (!width_.has_value() || !height_.has_value())
      return false;
    if (width_.value() <= 0 || height_.value() <= 0)
      return false;
    return true;
  }
}
