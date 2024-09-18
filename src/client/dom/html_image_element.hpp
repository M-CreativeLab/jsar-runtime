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
