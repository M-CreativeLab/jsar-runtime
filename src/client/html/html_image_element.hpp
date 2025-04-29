#pragma once

#include <string>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkBitmap.h>
#include <node/uv.h>
#include <client/dom/geometry/dom_rect.hpp>

#include "./html_element.hpp"
#include "../canvas/image_source.hpp"

namespace dom
{
  class HTMLImageElement final : public HTMLElement,
                                 public canvas::ImageSource
  {
    using HTMLElement::HTMLElement;

  public:
    enum DecodingType
    {
      // Decode the image synchronously along with rendering the other DOM content, and present everything together.
      kDecodingAsync,
      // Decode the image asynchronously, after rendering and presenting the other DOM content.
      kDecodingSync,
      // No preference for the decoding mode; the browser decides what is best for the user. This is the default value.
      kDecodingAuto,
    };

    enum LoadingType
    {
      // Loads the image immediately, regardless of whether or not the image is currently within the visible viewport
      // (this is the default value).
      kLoadingEager,
      // Defers loading the image until it reaches a calculated distance from the viewport, as defined by the browser.
      // The intent is to avoid the network and storage bandwidth needed to handle the image until it's reasonably
      // certain that it will be needed. This generally improves the performance of the content in most typical use
      // cases.
      kLoadingLazy,
    };

  public:
    HTMLImageElement(std::shared_ptr<Document> ownerDocument)
        : HTMLElement("IMG", ownerDocument),
          canvas::ImageSource()
    {
    }

  public:
    void createdCallback() override;
    void connectedCallback() override;
    void attributeChangedCallback(const std::string &name,
                                  const std::string &oldValue, const std::string &newValue) override;

    inline geometry::DOMRect getImageClientRect() const
    {
      return geometry::DOMRect(0, 0, sk_bitmap_->width(), sk_bitmap_->height());
    }
    bool readPixels(SkPixmap &dst) const override
    {
      dst.reset(sk_bitmap_->info(),
                sk_bitmap_->getPixels(),
                sk_bitmap_->rowBytes());
      return sk_bitmap_->readPixels(dst);
    }

    /**
     * Sets the image source URL.
     *
     * @param src The URL of the image to load.
     */
    void setSrc(const std::string &src)
    {
      is_src_image_loaded_ = false;
      is_src_image_decoded_ = false;

      setAttribute("src", src, false);
      loadImage();
    }

    /**
     * Returns the image source URL.
     *
     * @returns The URL of the image to load.
     */
    std::string getSrc()
    {
      return getAttribute("src");
    }

    /**
     * Load the image at the scriting thread, if you want to achieve the loading from other threads, you must use
     * `loadImageAsync()`.
     */
    void loadImage();

    /**
     * Load the image asynchronously, it must be used to schedule the image loading from the non-scripting thread.
     */
    void loadImageAsync();

  private:
    void fetchImage(const std::string &src);
    bool decodeImage(SkBitmap &);

    void onImageDataReady();
    void onImageDecoded(const SkBitmap &bitmap);
    void onSizeDidChange();

    // Validate if the current size is valid to create bitmap.
    bool validateSizeToMakeBitmap();

  public:
    /**
     * Returns a boolean value that is true if the user agent has finished fetching the image, whether successful or
     * not. That means this value is also true if the image has no src value indicating an image to load.
     */
    bool complete = false;
    /**
     * Returns a string representing the URL from which the currently displayed image was loaded.
     */
    std::string currentSrc;
    /**
     * A string that reflects the alt HTML attribute, thus indicating the alternate fallback content to be displayed if
     * the image has not been loaded.
     */
    std::string alt;

    inline size_t width() const override { return width_.value_or(0); }
    inline size_t height() const override { return height_.value_or(0); }
    inline void setWidth(size_t width)
    {
      width_ = width;
      onSizeDidChange();
    }
    inline void setHeight(size_t height)
    {
      height_ = height;
      onSizeDidChange();
    }

    inline LoadingType loading() const { return loading_; }
    inline DecodingType decoding() const { return decoding_; }

    /**
     * @returns a boolean value which indicates that the image is to be used by a server-side image map. This may only
     * be used on images located within an <a> element.
     */
    inline bool isMap() const { return is_map_; }

    /**
     * This property on the `HTMLImageElement` interface reflects the value of the HTML `usemap` attribute, which is a
     * string providing the name of the client-side image map to apply to the image.
     */
    inline std::string useMap() const { return use_map_; }

    /**
     * @returns The natural width of the image in pixels.
     */
    inline int naturalWidth() const { return sk_bitmap_ ? sk_bitmap_->width() : 0; }

    /**
     * @returns The natural height of the image in pixels.
     */
    inline int naturalHeight() const { return sk_bitmap_ ? sk_bitmap_->height() : 0; }

  private:
    uv_async_t load_async_handle_;

    std::optional<int> width_;
    std::optional<int> height_;

    std::optional<std::vector<char>> image_data_ = std::nullopt;
    std::shared_ptr<SkBitmap> sk_bitmap_;
    bool is_src_image_loading = false;
    bool is_src_image_loaded_ = false;
    bool is_src_image_decoded_ = false;

    LoadingType loading_;
    DecodingType decoding_;

    bool is_map_ = false;
    std::string use_map_;
  };
}
