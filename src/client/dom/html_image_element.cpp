#include <iostream>
#include "./html_image_element.hpp"
#include "./document.hpp"
#include "./rendering_context.hpp"

namespace dom
{
  void HTMLImageElement::loadImage(const std::string &src)
  {
    auto renderingContext = ownerDocument->lock()->renderingContext;
    renderingContext->fetchImageResource(src, [this](const std::string &imageData)
                                         { onImageLoaded(nullptr); });
  }

  void HTMLImageElement::onImageLoaded(char *pixels)
  {
    complete = true;
    dispatchEvent(DOMEventType::Load);
  }
}
