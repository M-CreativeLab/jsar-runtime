#include "./taffy_integration_layer.hpp"
#include "./layout_view.hpp"
#include "./layout_object.hpp"

namespace client_layout
{
  using namespace std;

  TaffyIntegrationLayer::TaffyIntegrationLayer(shared_ptr<LayoutView> view)
      : view_(view)
  {
  }

  TaffyIntegrationLayer::FlexLayoutResult
  TaffyIntegrationLayer::computeFlexLayout(
    const crates::layout2::LayoutStyle &container_style,
    const vector<crates::layout2::LayoutStyle> &child_styles,
    const ConstraintSpace &space)
  {
    FlexLayoutResult result;

    // Create temporary taffy tree for computation only
    auto root_node = setupTemporaryTaffyTree(container_style, child_styles);
    if (!root_node)
    {
      return result; // Failed to create computation tree
    }

    // Perform flex layout computation using taffy
    auto available_space = crates::layout2::Size<crates::layout2::AvailableSpace>{
      crates::layout2::AvailableSpace::Definite(space.avilableSize().x),
      crates::layout2::AvailableSpace::Definite(space.avilableSize().y)};

    auto layout_output = root_node->computeLayout(available_space);

    // Convert results back to our format
    result.container_fragment = convertTaffyResult(layout_output);
    result.success = true;

    // Get child layout results
    auto child_nodes = root_node->getChildren();
    for (size_t i = 0; i < child_nodes.size() && i < child_styles.size(); ++i)
    {
      auto child_output = child_nodes[i]->getLayout();
      result.child_fragments.push_back(convertTaffyResult(child_output));
    }

    return result;
  }

  TaffyIntegrationLayer::GridLayoutResult
  TaffyIntegrationLayer::computeGridLayout(
    const crates::layout2::LayoutStyle &container_style,
    const vector<crates::layout2::LayoutStyle> &child_styles,
    const ConstraintSpace &space)
  {
    GridLayoutResult result;

    // Similar to flex layout but for grid
    auto root_node = setupTemporaryTaffyTree(container_style, child_styles);
    if (!root_node)
    {
      return result;
    }

    auto available_space = crates::layout2::Size<crates::layout2::AvailableSpace>{
      crates::layout2::AvailableSpace::Definite(space.avilableSize().x),
      crates::layout2::AvailableSpace::Definite(space.avilableSize().y)};

    auto layout_output = root_node->computeLayout(available_space);

    result.container_fragment = convertTaffyResult(layout_output);
    result.success = true;

    auto child_nodes = root_node->getChildren();
    for (size_t i = 0; i < child_nodes.size() && i < child_styles.size(); ++i)
    {
      auto child_output = child_nodes[i]->getLayout();
      result.child_fragments.push_back(convertTaffyResult(child_output));
    }

    return result;
  }

  unique_ptr<crates::layout2::Node>
  TaffyIntegrationLayer::createComputationNode(const crates::layout2::LayoutStyle &style)
  {
    // Create a taffy node for computation only - not part of the layout tree
    return make_unique<crates::layout2::Node>(view_->taffyNodeAllocatorRef(), style);
  }

  vector<unique_ptr<crates::layout2::Node>>
  TaffyIntegrationLayer::createComputationNodes(const vector<crates::layout2::LayoutStyle> &styles)
  {
    vector<unique_ptr<crates::layout2::Node>> nodes;
    nodes.reserve(styles.size());

    for (const auto &style : styles)
    {
      nodes.push_back(createComputationNode(style));
    }

    return nodes;
  }

  Fragment TaffyIntegrationLayer::convertTaffyResult(const crates::layout2::LayoutOutput &output)
  {
    Fragment fragment;
    fragment.setSize(output.size.width, output.size.height, 0.0f);
    fragment.setPosition(output.location.x, output.location.y, 0.0f);
    return fragment;
  }

  unique_ptr<crates::layout2::Node>
  TaffyIntegrationLayer::setupTemporaryTaffyTree(
    const crates::layout2::LayoutStyle &root_style,
    const vector<crates::layout2::LayoutStyle> &child_styles)
  {
    // Create root computation node
    auto root_node = createComputationNode(root_style);
    if (!root_node)
    {
      return nullptr;
    }

    // Create child computation nodes
    auto child_nodes = createComputationNodes(child_styles);

    // Add children to root for computation
    vector<crates::layout2::NodeId> child_ids;
    for (auto &child : child_nodes)
    {
      child_ids.push_back(child->id());
    }

    root_node->setChildren(child_ids);

    // Note: child_nodes will be automatically cleaned up when they go out of scope
    // The taffy allocator manages the lifecycle

    return root_node;
  }

  // TaffyIntegratedFormattingContext implementation
  TaffyIntegratedFormattingContext::TaffyIntegratedFormattingContext(
    const DisplayType type, shared_ptr<LayoutView> view)
      : FormattingContext(type, view)
      , taffy_integration_(make_shared<TaffyIntegrationLayer>(view))
  {
  }

  Fragment TaffyIntegratedFormattingContext::liveFragment() const
  {
    return computed_fragment_;
  }

  void TaffyIntegratedFormattingContext::onAdded(const FormattingContext &parent,
                                                 shared_ptr<LayoutObject> beforeChild)
  {
    // Integration is handled by LayoutTreeManager
    // No direct taffy tree manipulation needed
    needs_layout_ = true;
  }

  void TaffyIntegratedFormattingContext::onRemoved(const FormattingContext &parent)
  {
    // No taffy cleanup needed - computation nodes are temporary
    needs_layout_ = true;
  }

  void TaffyIntegratedFormattingContext::onReplaced(const FormattingContext &parent,
                                                    const FormattingContext &old)
  {
    // No taffy replacement needed - computation nodes are temporary
    needs_layout_ = true;
  }

  void TaffyIntegratedFormattingContext::contentSizeDidChange(const glm::vec3 &contentSize)
  {
    FormattingContext::contentSizeDidChange(contentSize);
    needs_layout_ = true;
  }

  void TaffyIntegratedFormattingContext::setIsEmpty(bool b)
  {
    FormattingContext::setIsEmpty(b);
    needs_layout_ = true;
  }

  bool TaffyIntegratedFormattingContext::setLayoutStyle(crates::layout2::LayoutStyle &style)
  {
    FormattingContext::setLayoutStyle(style);
    style_ = style;
    needs_layout_ = true;
    return true;
  }

  unique_ptr<const LayoutResult> TaffyIntegratedFormattingContext::computeLayout(const ConstraintSpace &space)
  {
    if (needs_layout_)
    {
      auto layout_result = computeTaffyLayout(space);
      if (layout_result.success)
      {
        computed_fragment_ = layout_result.container_fragment;
        // TODO: Integrate child fragments with LayoutTreeManager
      }
      needs_layout_ = false;
    }

    // Create layout result
    dom::geometry::DOMRect rect;
    rect.x() = computed_fragment_.position().x;
    rect.y() = computed_fragment_.position().y;
    rect.width() = computed_fragment_.size().x;
    rect.height() = computed_fragment_.size().y;

    auto result = make_unique<LayoutResult>(rect);
    result->fragment() = computed_fragment_;
    result->status() = LayoutResult::kSuccess;

    return result;
  }

  void TaffyIntegratedFormattingContext::debugPrint() const
  {
    printf("TaffyIntegratedFormattingContext: size=%.2fx%.2f (computation via taffy, tree via C++)\n",
           computed_fragment_.size().x,
           computed_fragment_.size().y);
  }

  vector<crates::layout2::LayoutStyle> TaffyIntegratedFormattingContext::collectChildStyles()
  {
    // TODO: Integrate with LayoutTreeManager to collect child styles
    // For now, return empty vector
    return vector<crates::layout2::LayoutStyle>();
  }
}