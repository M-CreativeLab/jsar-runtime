#include <iostream>
#include <optional>
#include <common/image/image_processor.hpp>
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

  void HTMLImageElement::createdCallback(bool from_scripting)
  {
    HTMLElement::createdCallback(from_scripting);

    if (hasAttribute("width"))
      width_ = stoi(getAttribute("width"));
    if (hasAttribute("height"))
      height_ = stoi(getAttribute("height"));

    loading_ = from_scripting ? LoadingHint::kLoadingEager : LoadingHint::kLoadingLazy;
    decoding_ = DecodingType::kDecodingAuto;
  }

  void HTMLImageElement::connectedCallback()
  {
    HTMLElement::connectedCallback();
    sk_bitmap_ = make_shared<SkBitmap>();
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
        loading_ = LoadingHint::kLoadingLazy;
      else if (newValue == "eager")
        loading_ = LoadingHint::kLoadingEager;
      else
        loading_ = LoadingHint::kLoadingEager;
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
    else if (name == "spatial")
    {
      spatial_ = newValue;
    }
  }

  void HTMLImageElement::styleAdoptedCallback()
  {
    HTMLElement::styleAdoptedCallback();

    const client_cssom::ComputedStyle &adopted_style = adoptedStyleRef();
    if (adopted_style.hasProperty("height") ||
        adopted_style.hasProperty("width"))
    {
      if (!adopted_style.height().isAuto())
        height_ = adopted_style.height().toLayoutValue().value();
      if (!adopted_style.width().isAuto())
        width_ = adopted_style.width().toLayoutValue().value();

      // Schedule the image decoding if the sizes in style is changed.
      decodeImageAsync();
    }
  }

  void HTMLImageElement::loadImage()
  {
    if (is_src_image_loading ||
        is_src_image_loaded_ ||
        getSrc().empty())
      return;

    is_src_image_loading = true;
    fetchArrayBufferLikeResource(getSrc(), [this](const void *data, size_t length)
                                 { this->onImageDataReady(data, length); });
  }

  bool HTMLImageElement::decodeImage(SkBitmap &bitmap)
  {
    is_src_image_decoded_ = canvas::ImageCodec::Decode(image_data_.value(),
                                                       &image_format_,
                                                       bitmap,
                                                       getSrc(),
                                                       width_,
                                                       height_);
    if (is_src_image_decoded_)
    {
      bool perserve_image_data = image_format_.isSVG();
      if (!perserve_image_data)
      {
        image_data_->clear();
        image_data_.reset();
      }
    }
    return is_src_image_decoded_;
  }

  void HTMLImageElement::decodeImageAsync()
  {
    // Skip the decoding if the image data is not ready.
    if (!is_src_image_loaded_ || !image_data_.has_value())
      return;

    // Skip the decoding if the image is already decoded except for SVG images, which are needed to be rasterized with
    // the current size always.
    bool always_decoding = image_format_.isSVG();
    if (!always_decoding && is_src_image_decoded_)
      return;

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

          // Mark the image is completed for the first decoded time.
          if (!imageElement->complete)
          {
            imageElement->complete = true;
            imageElement->dispatchEvent(DOMEventType::Load);
          }
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
                  &decode_work_handle_,
                  work,
                  afterWork);
  }

  void HTMLImageElement::onImageDataReady(const void *imageData, size_t imageByteLength)
  {
    image_data_ = vector<char>(imageByteLength);
    image_data_->assign(static_cast<const char *>(imageData),
                        static_cast<const char *>(imageData) + imageByteLength);

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

    // TODO(yorkie): support `decoding` options.
    decodeImageAsync();
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
