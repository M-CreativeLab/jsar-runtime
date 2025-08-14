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
      : type(type)
      , view_(view)
  {
  }

  void FormattingContext::setContentSize(const glm::vec3 &size)
  {
    if (!content_size_.has_value() || content_size_ != size)
    {
      content_size_ = size;
      contentSizeDidChange(content_size_.value());
    }
  }

  void FormattingContext::setIsEmpty(bool b)
  {
    is_empty_ = b;
  }

  bool FormattingContext::setLayoutStyle(crates::layout2::LayoutStyle &style)
  {
    if (is_content_size_enabled_)
    {
      // When the incoming style is "auto", it indicates that the content size should be used.
      use_content_x_ = style.width().isAuto();
      use_content_y_ = style.height().isAuto();
    }
    return true;
  }

  TaffyBasedFormattingContext::TaffyBasedFormattingContext(const DisplayType type, shared_ptr<LayoutView> view)
      : FormattingContext(type, view)
      , node_(make_unique<crates::layout2::Node>(view->taffyNodeAllocatorRef()))
  {
  }

  Fragment TaffyBasedFormattingContext::liveFragment() const
  {
    assert(node_ != nullptr && "The Taffy node must be initialized.");
    return Fragment(node_->layout());
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
    FormattingContext::contentSizeDidChange(size);

    // Skip update style based on the new content size if the following flags are not set.
    if (!use_content_x_ && !use_content_y_)
      return;

    auto nodeStyle = node_->style();
    if (use_content_x_ && !isnan(content_size_->x))
      nodeStyle.setWidth(Dimension::Length(content_size_->x));
    if (use_content_y_ && !isnan(content_size_->y))
      nodeStyle.setHeight(Dimension::Length(content_size_->y));

    updateNodeStyle(nodeStyle);
  }

  void TaffyBasedFormattingContext::setIsEmpty(bool b)
  {
    FormattingContext::setIsEmpty(b);

    auto nodeStyle = node_->style();
    nodeStyle.setDisplay(b ? Display::None() : type);

    updateNodeStyle(nodeStyle);
  }

  bool TaffyBasedFormattingContext::setLayoutStyle(crates::layout2::LayoutStyle &style)
  {
    style.setDisplay(is_empty_ ? Display::None() : type);
    FormattingContext::setLayoutStyle(style);

    if (use_content_x_ || use_content_y_)
    {
      auto newStyle = style;
      if (use_content_x_ && !isnan(content_size_->x))
        newStyle.setWidth(Dimension::Length(content_size_->x));
      if (use_content_y_ && !isnan(content_size_->y))
        newStyle.setHeight(Dimension::Length(content_size_->y));
      updateNodeStyle(newStyle);
    }
    else
    {
      updateNodeStyle(style);
    }
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
    if (result->needsRelayout(resulting_fragment_))
      result->status() = LayoutResult::kRelayoutRequired;
    else
      result->status() = LayoutResult::kSuccess;

    // Update the resulting fragment.
    resulting_fragment_ = result->fragment();
    return result;
  }

  void TaffyBasedFormattingContext::debugPrint() const
  {
    assert(node_ != nullptr && "The Taffy node must be initialized.");
    node_->debugPrint();
  }

  void TaffyBasedFormattingContext::updateNodeStyle(const crates::layout2::LayoutStyle &style)
  {
    assert(node_ != nullptr && "The Taffy node must be initialized.");
    node_->setStyle(style);
    node_->markDirty();
  }

  // InlineFormattingContext implementation - true inline layout without taffy
  InlineFormattingContext::InlineFormattingContext(shared_ptr<LayoutView> view)
      : FormattingContext(DisplayType::Inline(), view)
      , taffy_placeholder_(make_unique<crates::layout2::Node>(view->taffyNodeAllocatorRef()))
  {
    // Initialize the taffy placeholder with block display and zero size
    // This will be updated as we compute the inline layout
    updateTaffyPlaceholder();
  }

  Fragment InlineFormattingContext::liveFragment() const
  {
    // Return a fragment based on our computed inline layout
    if (taffy_placeholder_)
    {
      return Fragment(taffy_placeholder_->layout());
    }

    Fragment fragment;
    fragment.setSize(computed_size_.x, computed_size_.y, 0.0f);
    return fragment;
  }

  void InlineFormattingContext::onAdded(const FormattingContext &parent, shared_ptr<LayoutObject> beforeChild)
  {
    // For inline formatting contexts, we add our taffy placeholder to taffy-based parents
    if (auto taffyParent = dynamic_cast<const TaffyBasedFormattingContext *>(&parent))
    {
      if (beforeChild != nullptr)
      {
        // Find the taffy node for beforeChild and insert before it
        bool isInserted = false;
        shared_ptr<LayoutObject> currentObject = beforeChild;

        while (currentObject != nullptr)
        {
          auto taffyFormattingCtx = dynamic_pointer_cast<TaffyBasedFormattingContext>(currentObject->formattingContext_);
          if (taffyFormattingCtx != nullptr)
          {
            taffyParent->node_->insertChild(*taffy_placeholder_, *taffyFormattingCtx->node_);
            isInserted = true;
            break;
          }
          currentObject = currentObject->nextSibling();
        }

        if (isInserted)
          return;
      }

      // Add to the end if no beforeChild or insertion failed
      taffyParent->node_->addChild(*taffy_placeholder_);
    }
    needs_layout_ = true;
  }

  void InlineFormattingContext::onRemoved(const FormattingContext &parent)
  {
    // Remove our taffy placeholder from taffy-based parent
    if (auto taffyParent = dynamic_cast<const TaffyBasedFormattingContext *>(&parent))
    {
      taffyParent->node_->removeChild(*taffy_placeholder_);
    }
    needs_layout_ = true;
  }

  void InlineFormattingContext::onReplaced(const FormattingContext &parent, const FormattingContext &old)
  {
    // Handle replacement in taffy-based parent
    if (auto taffyParent = dynamic_cast<const TaffyBasedFormattingContext *>(&parent))
    {
      if (auto oldTaffy = dynamic_cast<const TaffyBasedFormattingContext *>(&old))
      {
        taffyParent->node_->replaceChild(*oldTaffy->node_, *taffy_placeholder_, true);
      }
      else if (auto oldInline = dynamic_cast<const InlineFormattingContext *>(&old))
      {
        taffyParent->node_->replaceChild(*oldInline->taffy_placeholder_, *taffy_placeholder_, true);
      }
    }
    needs_layout_ = true;
  }

  void InlineFormattingContext::contentSizeDidChange(const glm::vec3 &size)
  {
    FormattingContext::contentSizeDidChange(size);
    needs_layout_ = true;
  }

  void InlineFormattingContext::setIsEmpty(bool b)
  {
    FormattingContext::setIsEmpty(b);
    if (b)
    {
      computed_size_ = glm::vec2(0.0f, 0.0f);
      line_boxes_.clear();
    }
    needs_layout_ = true;
    updateTaffyPlaceholder();
  }

  bool InlineFormattingContext::setLayoutStyle(crates::layout2::LayoutStyle &style)
  {
    FormattingContext::setLayoutStyle(style);
    needs_layout_ = true;

    // FIXED: Use Display::Inline() which maps to Display::None in taffy
    // This prevents taffy from treating inline content as block elements
    style.setDisplay(is_empty_ ? crates::layout2::styles::Display::None() : crates::layout2::styles::Display::Inline());
    taffy_placeholder_->setStyle(style);
    return true;
  }

  unique_ptr<const LayoutResult> InlineFormattingContext::computeLayout(const ConstraintSpace &space)
  {
    if (needs_layout_)
    {
      computeInlineLayout(space);
      updateTaffyPlaceholder();
      needs_layout_ = false;
    }

    // Get the actual layout from our taffy placeholder (which has the correct position from parent)
    crates::layout2::Layout layout = taffy_placeholder_->layout();
    dom::geometry::DOMRect rect;
    rect.x() = layout.left();
    rect.y() = layout.top();
    rect.width() = computed_size_.x;  // Use our computed inline width
    rect.height() = computed_size_.y; // Use our computed inline height

    auto result = make_unique<LayoutResult>(rect);
    result->fragment().setBorder(layout.border());
    result->fragment().setPadding(layout.padding());

    // Set the status by comparing with previous layout
    if (result->needsRelayout(resulting_fragment_))
      result->status() = LayoutResult::kRelayoutRequired;
    else
      result->status() = LayoutResult::kSuccess;

    // Update the resulting fragment
    resulting_fragment_ = result->fragment();
    return result;
  }

  void InlineFormattingContext::computeInlineLayout(const ConstraintSpace &space)
  {
    // Basic inline layout algorithm
    // In a real implementation, this would handle:
    // - Line breaking and wrapping
    // - Baseline alignment
    // - Inline box positioning
    // - Text measurement and positioning

    line_boxes_.clear();

    // For now, implement a simplified inline layout
    // This is a starting point that can be expanded with proper inline algorithms

    float available_width = space.width();
    float current_line_width = 0.0f;
    float line_height = 16.0f; // Default line height, should come from style
    float total_height = 0.0f;

    LineBox current_line;
    current_line.height = line_height;
    current_line.baseline = line_height * 0.8f; // Simplified baseline calculation

    // TODO: Iterate through inline children and perform proper line breaking
    // For now, create a simple single-line layout

    // If we have content size from text or replaced elements, use it
    if (content_size_.has_value())
    {
      current_line.width = std::min(content_size_->x, available_width);
      current_line.height = std::max(current_line.height, content_size_->y);
    }
    else
    {
      // Minimal size for empty inline elements
      current_line.width = 0.0f;
    }

    line_boxes_.push_back(current_line);

    // Compute final size
    computed_size_.x = current_line.width;
    computed_size_.y = current_line.height;

    // TODO: Handle multiple lines, proper text metrics, etc.
  }

  void InlineFormattingContext::updateTaffyPlaceholder()
  {
    if (!taffy_placeholder_)
      return;

    // For inline elements, we need a different approach than setting Display::Block()
    // which causes unwanted line breaks. Since taffy doesn't natively support inline layout,
    // we should not try to force inline content into taffy's block-based system.

    // TODO: Research the correct approach for inline/taffy integration
    // For now, commenting out the problematic Display::Block() setting
    auto style = taffy_placeholder_->style();

    // Set the size based on our inline layout computation
    style.setWidth(crates::layout2::styles::Dimension::Length(computed_size_.x));
    style.setHeight(crates::layout2::styles::Dimension::Length(computed_size_.y));

    // PROBLEM: Setting Display::Block() causes inline elements to break to new lines
    // This violates CSS inline layout where elements should flow horizontally
    // style.setDisplay(is_empty_ ? crates::layout2::styles::Display::None() : crates::layout2::styles::Display::Block());

    taffy_placeholder_->setStyle(style);
    taffy_placeholder_->markDirty();
  }

  void InlineFormattingContext::debugPrint() const
  {
    printf("InlineFormattingContext: size=%.2fx%.2f, lines=%zu\n",
           computed_size_.x,
           computed_size_.y,
           line_boxes_.size());
    if (taffy_placeholder_)
    {
      printf("  Taffy placeholder: ");
      taffy_placeholder_->debugPrint();
    }
  }
}
