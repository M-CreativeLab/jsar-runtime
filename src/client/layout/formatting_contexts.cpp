#include <client/dom/geometry/dom_rect.hpp>

#include "./formatting_contexts-inl.hpp"
#include "./fragment.hpp"

namespace client_layout
{
  using namespace std;

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

  TaffyBasedFormattingContext::TaffyBasedFormattingContext(const DisplayType type, shared_ptr<LayoutView> view)
      : FormattingContext(type, view),
        node_(make_unique<crates::layout2::Node>(view->taffyNodeAllocatorRef()))
  {
  }

  void TaffyBasedFormattingContext::onAdded(const FormattingContext &parent)
  {
    if (!parent.isBlock()) // Skip if the parent is not a block.
      return;

    auto &taffyParent = dynamic_cast<const TaffyBasedFormattingContext &>(parent);
    taffyParent.node_->addChild(*node_);
  }

  void TaffyBasedFormattingContext::onRemoved(const FormattingContext &parent)
  {
    if (!parent.isBlock()) // Skip if the parent is not a block.
      return;

    auto &taffyParent = dynamic_cast<const TaffyBasedFormattingContext &>(parent);
    taffyParent.node_->removeChild(*node_);
  }

  void TaffyBasedFormattingContext::onReplaced(const FormattingContext &parent, const FormattingContext &old)
  {
    if (!parent.isBlock()) // Skip if the parent is not a block.
      return;

    auto &taffyParent = dynamic_cast<const TaffyBasedFormattingContext &>(parent);
    auto &taffyOld = dynamic_cast<const TaffyBasedFormattingContext &>(old);
    taffyParent.node_->replaceChild(*taffyOld.node_, *node_, true);
  }

  bool TaffyBasedFormattingContext::setLayoutStyle(const crates::layout2::LayoutStyle &style)
  {
    assert(node_ != nullptr && "The Taffy node must be initialized.");
    node_->setStyle(style);
    return true;
  }

  unique_ptr<const LayoutResult> TaffyBasedFormattingContext::computeLayout(const ConstraintSpace &space)
  {
    assert(node_ != nullptr && "The Taffy node must be initialized.");
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
    if (result->needsResize(resultingFragment_))
      result->status() = LayoutResult::kResizeRequired;
    else if (result->needsRelayout(resultingFragment_))
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

  void InlineFormattingContext::onAdded(const FormattingContext &parent)
  {
    // TODO(yorkie): implement this method.
  }

  void InlineFormattingContext::onRemoved(const FormattingContext &parent)
  {
    // TODO(yorkie): implement this method.
  }

  void InlineFormattingContext::onReplaced(const FormattingContext &parent, const FormattingContext &old)
  {
    // TODO(yorkie): implement this method.
  }

  bool InlineFormattingContext::setLayoutStyle(const crates::layout2::LayoutStyle &style)
  {
    return true;
  }

  unique_ptr<const LayoutResult> InlineFormattingContext::computeLayout(const ConstraintSpace &space)
  {
    auto defaultResult = make_unique<LayoutResult>(dom::geometry::DOMRect(0, 0, space.width(), space.height()));
    defaultResult->status() = LayoutResult::kSuccess;
    return defaultResult;
  }

  void InlineFormattingContext::debugPrint() const
  {
    // TODO(yorkie): implement this method.
  }
}
