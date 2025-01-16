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
  public:
    using HTMLPlaneElement::HTMLPlaneElement;

  public:
    void createdCallback() override;
    void connectedCallback() override;
    void onLayoutChanged() override final;
    void onAdoptedStyleChanged() override final;

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
