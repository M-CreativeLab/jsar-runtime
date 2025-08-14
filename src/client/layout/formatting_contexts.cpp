#include <client/dom/geometry/dom_rect.hpp>

#include "./formatting_contexts-inl.hpp"
#include "./fragment.hpp"
#include "./taffy_integration_layer.hpp"

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

    // Use pure CSS block layout instead of taffy-based for regular blocks
    return make_unique<BlockFormattingContext>(view);
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

  // InlineFormattingContext implementation - pure CSS inline layout without taffy
  InlineFormattingContext::InlineFormattingContext(shared_ptr<LayoutView> view)
      : FormattingContext(DisplayType::Inline(), view)
  {
    // No taffy nodes created - pure C++ layout tree management
  }

  Fragment InlineFormattingContext::liveFragment() const
  {
    Fragment fragment;
    fragment.setSize(computed_size_.x, computed_size_.y, 0.0f);
    return fragment;
  }

  void InlineFormattingContext::onAdded(const FormattingContext &parent, shared_ptr<LayoutObject> beforeChild)
  {
    // Handle integration with taffy-based parents
    // Since inline elements don't use taffy nodes, we need special handling
    // when added to taffy-based parents (blocks, flex, grid)

    if (auto taffyParent = dynamic_cast<const TaffyBasedFormattingContext *>(&parent))
    {
      // The taffy-based parent will need to account for our computed size
      // but we don't create a taffy node ourselves
      // This represents the architectural gap that @yorkie wants to fix:
      // ideally, we'd have a unified layout tree management system

      // For now, do nothing - the parent will need to handle inline children differently
      // TODO: Implement proper inline/block layout tree integration
    }
  }

  void InlineFormattingContext::onRemoved(const FormattingContext &parent)
  {
    // No taffy placeholder to remove - pure layout tree management
  }

  void InlineFormattingContext::onReplaced(const FormattingContext &parent, const FormattingContext &old)
  {
    // No taffy placeholder replacement needed - pure layout tree management
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
  }

  bool InlineFormattingContext::setLayoutStyle(crates::layout2::LayoutStyle &style)
  {
    FormattingContext::setLayoutStyle(style);
    style_ = style;
    needs_layout_ = true;

    // Do NOT modify the style for taffy - this is pure CSS layout
    // No Display::None mapping needed since we don't create taffy nodes
    return true;
  }

  unique_ptr<const LayoutResult> InlineFormattingContext::computeLayout(const ConstraintSpace &space)
  {
    if (needs_layout_)
    {
      computeInlineLayout(space);
      needs_layout_ = false;
    }

    // Create layout result from our pure CSS computation
    dom::geometry::DOMRect rect;
    rect.x() = 0.0f; // Position will be set by parent
    rect.y() = 0.0f;
    rect.width() = computed_size_.x;
    rect.height() = computed_size_.y;

    auto result = make_unique<LayoutResult>(rect);

    // Update the resulting fragment
    resulting_fragment_.setSize(computed_size_.x, computed_size_.y, 0.0f);
    result->fragment() = resulting_fragment_;
    result->status() = LayoutResult::kSuccess;

    return result;
  }

  void InlineFormattingContext::computeInlineLayout(const ConstraintSpace &space)
  {
    // Pure CSS inline layout algorithm according to CSS 2.1 specification
    line_boxes_.clear();
    computed_size_ = glm::vec2(0.0f, 0.0f);

    if (is_empty_)
    {
      return;
    }

    const float available_width = space.avilableSize().x;
    float line_height = 16.0f; // Basic line height - should come from style

    // Basic single-line layout for now
    // TODO: Implement proper line breaking, text measurement, and multi-line support

    LineBox line;
    line.height = line_height;
    line.baseline = line_height * 0.8f; // Basic baseline calculation
    line.position = glm::vec2(0.0f, 0.0f);

    // If we have content size from text or replaced elements, use it
    if (content_size_.has_value())
    {
      line.width = std::min(content_size_->x, available_width);
      line.height = std::max(line.height, content_size_->y);
    }
    else
    {
      // Minimal inline content width
      line.width = std::min(100.0f, available_width);
    }

    line_boxes_.push_back(line);

    // Set computed size based on line boxes
    computed_size_.x = line.width;
    computed_size_.y = line.height;
  }

  void InlineFormattingContext::debugPrint() const
  {
    printf("InlineFormattingContext: size=%.2fx%.2f, lines=%zu (pure CSS layout, no taffy)\n",
           computed_size_.x,
           computed_size_.y,
           line_boxes_.size());
  }

  // BlockFormattingContext implementation - pure CSS block layout without taffy
  BlockFormattingContext::BlockFormattingContext(shared_ptr<LayoutView> view)
      : FormattingContext(DisplayType::Block(), view)
  {
    // No taffy nodes created - pure C++ layout tree management
  }

  Fragment BlockFormattingContext::liveFragment() const
  {
    Fragment fragment;
    fragment.setSize(computed_size_.x, computed_size_.y, 0.0f);
    return fragment;
  }

  void BlockFormattingContext::onAdded(const FormattingContext &parent, shared_ptr<LayoutObject> beforeChild)
  {
    // Handle integration with different parent context types
    // This is managed by the LayoutTreeManager for proper integration

    if (auto taffyParent = dynamic_cast<const TaffyBasedFormattingContext *>(&parent))
    {
      // Block context being added to taffy-based parent (flex/grid)
      // The LayoutTreeManager will coordinate this integration
      // TODO: Create proper integration through LayoutTreeManager
    }
    // For pure CSS parents, integration is handled through tree manager
  }

  void BlockFormattingContext::onRemoved(const FormattingContext &parent)
  {
    // No taffy cleanup needed - pure layout tree management
  }

  void BlockFormattingContext::onReplaced(const FormattingContext &parent, const FormattingContext &old)
  {
    // No taffy replacement needed - pure layout tree management
  }

  void BlockFormattingContext::contentSizeDidChange(const glm::vec3 &size)
  {
    FormattingContext::contentSizeDidChange(size);
    needs_layout_ = true;
  }

  void BlockFormattingContext::setIsEmpty(bool b)
  {
    FormattingContext::setIsEmpty(b);
    if (b)
    {
      computed_size_ = glm::vec2(0.0f, 0.0f);
      child_boxes_.clear();
    }
    needs_layout_ = true;
  }

  bool BlockFormattingContext::setLayoutStyle(crates::layout2::LayoutStyle &style)
  {
    FormattingContext::setLayoutStyle(style);
    style_ = style;
    needs_layout_ = true;

    // Do NOT modify the style for taffy - this is pure CSS layout
    // No taffy integration needed since we don't create taffy nodes
    return true;
  }

  unique_ptr<const LayoutResult> BlockFormattingContext::computeLayout(const ConstraintSpace &space)
  {
    if (needs_layout_)
    {
      computeBlockLayout(space);
      needs_layout_ = false;
    }

    // Create layout result from our pure CSS computation
    dom::geometry::DOMRect rect;
    rect.x() = 0.0f; // Position will be set by parent
    rect.y() = 0.0f;
    rect.width() = computed_size_.x;
    rect.height() = computed_size_.y;

    auto result = make_unique<LayoutResult>(rect);

    // Update the resulting fragment
    resulting_fragment_.setSize(computed_size_.x, computed_size_.y, 0.0f);
    result->fragment() = resulting_fragment_;
    result->status() = LayoutResult::kSuccess;

    return result;
  }

  void BlockFormattingContext::computeBlockLayout(const ConstraintSpace &space)
  {
    // Pure CSS block layout algorithm according to CSS 2.1 specification
    child_boxes_.clear();
    computed_size_ = glm::vec2(0.0f, 0.0f);

    if (is_empty_)
    {
      return;
    }

    const float available_width = space.avilableSize().x;
    const float available_height = space.avilableSize().y;

    // Block layout: children are laid out vertically
    float current_y = 0.0f;
    float max_width = 0.0f;

    // Basic block layout - stack children vertically
    // TODO: Implement proper margin collapsing, clearance, and CSS box model

    // If we have content size from replaced elements, use it
    if (content_size_.has_value())
    {
      computed_size_.x = std::min(content_size_->x, available_width);
      computed_size_.y = content_size_->y;
    }
    else
    {
      // Default block behavior: take available width, height based on content
      computed_size_.x = available_width;

      // For now, minimal height
      computed_size_.y = std::max(20.0f, current_y);
    }

    // TODO: Implement child layout coordination through LayoutTreeManager
    // For now, basic block container behavior
  }

  void BlockFormattingContext::debugPrint() const
  {
    printf("BlockFormattingContext: size=%.2fx%.2f, children=%zu (pure CSS layout, no taffy)\n",
           computed_size_.x,
           computed_size_.y,
           child_boxes_.size());
  }

  // FlexFormattingContext implementation - uses taffy for computation but C++ for tree management
  FlexFormattingContext::FlexFormattingContext(shared_ptr<LayoutView> view)
      : TaffyIntegratedFormattingContext(DisplayType::Flex(), view)
  {
  }

  TaffyIntegrationLayer::FlexLayoutResult FlexFormattingContext::computeTaffyLayout(const ConstraintSpace &space)
  {
    // Get child styles through the layout tree manager
    auto child_styles = collectChildStyles();

    // Use taffy integration layer for flex computation
    // TODO: Implement proper style collection and integration
    TaffyIntegrationLayer integration_layer(view_.lock());
    crates::layout2::LayoutStyle container_style; // TODO: Get from current style
    return integration_layer.computeFlexLayout(container_style, child_styles, space);
  }

  // GridFormattingContext implementation - uses taffy for computation but C++ for tree management
  GridFormattingContext::GridFormattingContext(shared_ptr<LayoutView> view)
      : TaffyIntegratedFormattingContext(DisplayType::Grid(), view)
  {
  }

  TaffyIntegrationLayer::FlexLayoutResult GridFormattingContext::computeTaffyLayout(const ConstraintSpace &space)
  {
    // Get child styles through the layout tree manager
    auto child_styles = collectChildStyles();

    // Use taffy integration layer for grid computation
    // TODO: Implement proper style collection and integration
    TaffyIntegrationLayer integration_layer(view_.lock());
    crates::layout2::LayoutStyle container_style; // TODO: Get from current style

    // Note: Using FlexLayoutResult for now, should create proper GridLayoutResult method
    auto grid_result = integration_layer.computeGridLayout(container_style, child_styles, space);

    // Convert GridLayoutResult to FlexLayoutResult for interface compatibility
    TaffyIntegrationLayer::FlexLayoutResult result;
    result.container_fragment = grid_result.container_fragment;
    result.child_fragments = grid_result.child_fragments;
    result.success = grid_result.success;
    return result;
  }
}
