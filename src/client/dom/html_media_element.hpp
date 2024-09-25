#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLMediaElement : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;
    HTMLMediaElement(std::__1::string tagName, weak_ptr<Document> ownerDocument)
        : HTMLElement(tagName, ownerDocument)
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
      loadMedia(src);
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
    void loadMedia(const std::string &src);
  };
}
