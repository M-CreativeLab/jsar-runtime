#pragma once

#include <string>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkCanvas.h>
#include "./html_plane_element.hpp"

namespace dom
{
  class HTMLContentElement : public HTMLPlaneElement
  {
  public:
    using HTMLPlaneElement::HTMLPlaneElement;

  public:
    HTMLContentElement(std::string tagName, std::shared_ptr<Document> ownerDocument)
        : HTMLPlaneElement(tagName, ownerDocument)
    {
    }

  public:
    void connectedCallback() override;
    void onLayoutSizeChanged() override final;
    void onAdoptedStyleChanged() override final;

  private:
    sk_sp<SkSurface> createOrGetSurface();

  private:
    sk_sp<SkSurface> contentSurface_;
  };
}
