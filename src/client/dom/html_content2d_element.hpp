#pragma once

#include <string>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkCanvas.h>
#include "./html_element.hpp"

namespace dom
{
  class HTMLContent2dElement : public HTMLElement
  {
    using HTMLElement::HTMLElement;

  // public:
  //   HTMLContent2dElement(const HTMLContent2dElement &other)
  //       : HTMLElement(other),
  //         content2d_(nullptr) // ignore the content2d_ from the other
  //   {
  //   }

  // public:
  //   void createdCallback() override;
  //   void connectedCallback() override;
  //   void disconnectedCallback() override;
  //   // void onLayoutChanged() override;
  //   // void onAdoptedStyleChanged() override;

  // private:
  //   // Get the content2d and expect it to be valid.
  //   inline Content2d &content2d()
  //   {
  //     if (TR_UNLIKELY(content2d_ == nullptr))
  //     {
  //       std::string msg = "Failed to get the `Content2d` on <" + tagName + ">: the content2d is not created yet.";
  //       throw std::runtime_error(msg);
  //     }
  //     return *content2d_;
  //   }

  // private:
  //   std::unique_ptr<Content2d> content2d_;
  };
}
