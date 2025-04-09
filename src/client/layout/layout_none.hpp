#pragma once

#include "./layout_box_model_object.hpp"

namespace client_layout
{
  // The LayoutNone class is used for the layout object that has no layout.
  class LayoutNone : public LayoutBoxModelObject
  {
    using LayoutBoxModelObject::LayoutBoxModelObject;

  public:
    const char *name() const override { return "LayoutNone"; }
    bool isNone() const override final { return true; }
  };
}
