#pragma once

#include "./layout_box_model_object.hpp"

namespace client_layout
{
  class LayoutInline : public LayoutBoxModelObject
  {
    using LayoutBoxModelObject::LayoutBoxModelObject;

  public:
    const char *name() const override { return "LayoutInline"; }
    bool isLayoutInline() const override final { return true; }
  };
}
