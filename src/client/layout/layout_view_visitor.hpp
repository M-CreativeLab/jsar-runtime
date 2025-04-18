#pragma once

#include "./layout_view.hpp"

namespace client_layout
{
  class LayoutObject;
  class LayoutBoxModelObject;
  class LayoutText;
  class LayoutBox;
  class LayoutBlock;
  class LayoutInline;

  class LayoutViewVisitor
  {
  public:
    LayoutViewVisitor() = default;
    virtual ~LayoutViewVisitor() = default;

  public:
    virtual bool onVisitObject(LayoutObject &object, int depth) = 0;
    virtual void onVisitText(const LayoutText &text, int depth) {};
    virtual void onVisitBox(const LayoutBoxModelObject &box, int depth) {};
    virtual void onVisitBlock(const LayoutBlock &block, int depth) {};
    virtual void onVisitInline(const LayoutInline &inlineObject, int depth) {};

  public:
    void visit(LayoutView &view);
  };
}
