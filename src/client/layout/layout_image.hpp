#pragma once

#include "./layout_replaced.hpp"

namespace client_layout
{
  /**
   * The layout box for image types:
   * 
   * 1. normal image types: <img>, <picture>.
   * 2. content image types: "content: url(path/to/image.png)".
   */
  class LayoutImage : public LayoutReplaced
  {
    using LayoutReplaced::LayoutReplaced;

  public:
    const char *name() const override { return "LayoutImage"; }
    bool isLayoutImage() const override final { return true; }
  };
}
