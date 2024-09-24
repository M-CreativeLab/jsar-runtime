#pragma once

#include <string>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkBitmap.h>
#include "./html_element.hpp"
#include "../canvas/image_source.hpp"

namespace dom
{
  class HTMLImageElement : public HTMLElement,
                           public canvas::ImageSource
  {
  public:
    using HTMLElement::HTMLElement;
    HTMLImageElement(weak_ptr<Document> ownerDocument)
        : HTMLElement("IMG", ownerDocument),
          canvas::ImageSource(),
          skBitmap(std::make_shared<SkBitmap>())
    {
    }

  public:
    size_t width() override { return skBitmap->width(); }
    size_t height() override { return skBitmap->height(); }
    bool readPixels(SkPixmap &dst) override
    {
      dst.reset(skBitmap->info(),
                skBitmap->getPixels(),
                skBitmap->rowBytes());
      return skBitmap->readPixels(dst);
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
    std::shared_ptr<SkBitmap> skBitmap;
  };
}
