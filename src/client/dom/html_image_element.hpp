#pragma once

#include <string>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkBitmap.h>
#include "./html_content2d_element.hpp"
#include "../canvas/image_source.hpp"

namespace dom
{
  class HTMLImageElement : public HTMLContent2dElement,
                           public canvas::ImageSource
  {
  public:
    using HTMLContent2dElement::HTMLContent2dElement;

  public:
    HTMLImageElement(std::shared_ptr<Document> ownerDocument)
        : HTMLContent2dElement("IMG", ownerDocument),
          canvas::ImageSource()
    {
    }

  public:
    void connectedCallback() override;

  public:
    size_t width() const override { return skBitmap_->width(); }
    size_t height() const override { return skBitmap_->height(); }
    bool readPixels(SkPixmap &dst) const override
    {
      dst.reset(skBitmap_->info(),
                skBitmap_->getPixels(),
                skBitmap_->rowBytes());
      return skBitmap_->readPixels(dst);
    }

  public:
    /**
     * Sets the image source URL.
     *
     * @param src The URL of the image to load.
     */
    void setSrc(const std::string &src)
    {
      setAttribute("src", src);
      loadImage(src);
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

  private:
    /**
     * Loads the image from the given URL.
     *
     * @param src The URL of the image to load.
     */
    void loadImage(const std::string &src);

    /**
     * Called when the image is loaded.
     *
     * @param imageData The image data.
     * @param imageByteLength The length of the image data.
     */
    void onImageLoaded(const void *imageData, size_t imageByteLength);

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

  private:
    std::shared_ptr<SkBitmap> skBitmap_;
    bool isSrcImageLoaded_ = false;
  };
}
