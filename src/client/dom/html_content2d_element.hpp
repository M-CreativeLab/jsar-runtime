#pragma once

#include <string>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkCanvas.h>
#include "./html_plane_element.hpp"
#include "./content2d.hpp"

namespace dom
{
  class HTMLContent2dElement : public HTMLPlaneElement
  {
    using HTMLPlaneElement::HTMLPlaneElement;

  public:
    HTMLContent2dElement(const HTMLContent2dElement &other)
        : HTMLPlaneElement(other),
          content2d_(nullptr) // ignore the content2d_ from the other
    {
    }

  public:
    void createdCallback() override;
    void connectedCallback() override;
    void disconnectedCallback() override;
    void onLayoutChanged() override;
    void onAdoptedStyleChanged() override;

  private:
    // Get the content2d and expect it to be valid.
    inline Content2d &content2d()
    {
      assert(content2d_ != nullptr);
      return *content2d_;
    }

  private:
    std::unique_ptr<Content2d> content2d_;
  };
}
