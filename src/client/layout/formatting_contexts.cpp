#include <client/dom/geometry/dom_rect.hpp>

#include "./formatting_contexts-inl.hpp"
#include "./fragment.hpp"

namespace client_layout
{
  using namespace std;
  using namespace crates::layout2::styles;

  unique_ptr<FormattingContext> FormattingContext::Make(DisplayType display, shared_ptr<LayoutView> view)
  {
    if (display.isInline())
      return make_unique<InlineFormattingContext>(view);
    if (display.isFlex())
      return make_unique<FlexFormattingContext>(view);
    if (display.isGrid())
      return make_unique<GridFormattingContext>(view);

    // TODO(yorkie): support other formatting contexts.
    return make_unique<BlockFlowFormattingContext>(view);
  }

  FormattingContext::FormattingContext(DisplayType type, shared_ptr<LayoutView> view)
      : type(type), view_(view)
  {
  }

  void FormattingContext::setContentSize(const glm::vec3 &size)
  {
    if (!contentSize_.has_value() || contentSize_ != size)
    {
      contentSize_ = size;
      contentSizeDidChange(contentSize_.value());
    }
  }

  TaffyBasedFormattingContext::TaffyBasedFormattingContext(const DisplayType type, shared_ptr<LayoutView> view)
      : FormattingContext(type, view),
        node_(make_unique<crates::layout2::Node>(view->taffyNodeAllocatorRef()))
  {
  }

  Fragment TaffyBasedFormattingContext::liveFragment() const
  {
    assert(node_ != nullptr && "The Taffy node must be initialized.");

    auto layout = node_->layout();
    dom::geometry::DOMRect rect;
    rect.x() = layout.left();
    rect.y() = layout.top();
    rect.width() = layout.width();
    rect.height() = layout.height();

    Fragment fragment(rect);
    fragment.setBorder(layout.border());
    fragment.setPadding(layout.padding());
    return fragment;
  }

  void TaffyBasedFormattingContext::onAdded(const FormattingContext &parent,
                                            shared_ptr<LayoutObject> beforeChild)
  {
    auto &taffyParent = dynamic_cast<const TaffyBasedFormattingContext &>(parent);
    if (beforeChild != nullptr)
    {
      // Search for the before child util we find a Taffy node.
      bool isInserted = false;
      shared_ptr<LayoutObject> currentObject = beforeChild;

      while (currentObject != nullptr)
      {
        auto formattingCtx = dynamic_pointer_cast<TaffyBasedFormattingContext>(currentObject->formattingContext_);
        if (formattingCtx != nullptr)
        {
          taffyParent.node_->insertChild(*node_, *formattingCtx->node_);
          isInserted = true;
          break;
        }
        else
        {
          // If the current object is not a Taffy node, move to the next sibling.
          currentObject = currentObject->nextSibling();
        }
      }

      if (isInserted)
        return; // Successfully inserted before the child.
    }

    // Add the child to the end of the parent's children by default.
    taffyParent.node_->addChild(*node_);
  }

  void TaffyBasedFormattingContext::onRemoved(const FormattingContext &parent)
  {
    auto &taffyParent = dynamic_cast<const TaffyBasedFormattingContext &>(parent);
    taffyParent.node_->removeChild(*node_);
  }

  void TaffyBasedFormattingContext::onReplaced(const FormattingContext &parent, const FormattingContext &old)
  {
    auto &taffyParent = dynamic_cast<const TaffyBasedFormattingContext &>(parent);
    auto &taffyOld = dynamic_cast<const TaffyBasedFormattingContext &>(old);
    taffyParent.node_->replaceChild(*taffyOld.node_, *node_, true);
  }

  void TaffyBasedFormattingContext::contentSizeDidChange(const glm::vec3 &size)
  {
    auto nodeStyle = node_->style();
    if (nodeStyle.width().isAuto() && !std::isnan(contentSize_->x))
      nodeStyle.setWidth(Dimension::Length(contentSize_->x));
    if (nodeStyle.height().isAuto() && !std::isnan(contentSize_->y))
      nodeStyle.setHeight(Dimension::Length(contentSize_->y));

    node_->setStyle(nodeStyle);
    node_->markDirty();
  }

  bool TaffyBasedFormattingContext::setLayoutStyle(const crates::layout2::LayoutStyle &style)
  {
    assert(node_ != nullptr && "The Taffy node must be initialized.");
    node_->setStyle(style);
    node_->markDirty();
    return true;
  }

  unique_ptr<const LayoutResult> TaffyBasedFormattingContext::computeLayout(const ConstraintSpace &space)
  {
    assert(node_ != nullptr && "The Taffy node must be initialized.");
    if (node_->isDirty())
      node_->computeLayout(space.width(), space.height());

    crates::layout2::Layout layout = node_->layout();
    dom::geometry::DOMRect rect;
    rect.x() = layout.left();
    rect.y() = layout.top();
    rect.width() = layout.width();
    rect.height() = layout.height();

    auto result = make_unique<LayoutResult>(rect);
    result->fragment().setBorder(layout.border());
    result->fragment().setPadding(layout.padding());

    // Set the status by the layout result.
    if (result->needsRelayout(resultingFragment_))
      result->status() = LayoutResult::kRelayoutRequired;
    else
      result->status() = LayoutResult::kSuccess;

    // Update the resulting fragment.
    resultingFragment_ = result->fragment();
    return result;
  }

  void TaffyBasedFormattingContext::debugPrint() const
  {
    assert(node_ != nullptr && "The Taffy node must be initialized.");
    node_->debugPrint();
  }
}
