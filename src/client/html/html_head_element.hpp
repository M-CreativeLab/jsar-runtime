#pragma once

#include <string>
#include "./html_element.hpp"

namespace endor
{
  namespace dom
  {
    class HTMLHeadElement final : public HTMLElement
    {
    public:
      using HTMLElement::HTMLElement;
      HTMLHeadElement(std::shared_ptr<Document> ownerDocument)
          : HTMLElement("HEAD", ownerDocument)
      {
      }

    public:
      bool isHTMLHeadElement() const override
      {
        return true;
      }
    };
  }
} // namespace endor
