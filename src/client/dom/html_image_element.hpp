#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLImageElement : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;
    HTMLImageElement(weak_ptr<Document> ownerDocument) : HTMLElement("IMG", ownerDocument)
    {
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
     * @param pixels The image data.
     */
    void onImageLoaded(char *pixels);

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
  };
}
