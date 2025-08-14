#pragma once

#include <memory>
#include <vector>
#include <crates/bindings.hpp>

#include "./constraint_space.hpp"
#include "./fragment.hpp"
#include "./layout_result.hpp"

namespace client_layout
{
  class FormattingContext;
  class LayoutObject;
  class LayoutView;

  /**
   * TaffyIntegrationLayer provides a bridge between pure CSS formatting contexts
   * and taffy's flex/grid layout computations. This layer allows taffy to be used
   * ONLY for specific layout computations (flex, grid) while the layout tree
   * management remains entirely in C++.
   *
   * This addresses @yorkie's architecture requirement to separate layout tree
   * management from layout computation engines.
   */
  class TaffyIntegrationLayer
  {
  public:
    explicit TaffyIntegrationLayer(std::shared_ptr<LayoutView> view);

    /**
     * Compute flex layout using taffy, but without taffy managing the tree structure.
     * Returns layout results that can be integrated into the C++ layout tree.
     */
    struct FlexLayoutResult
    {
      Fragment container_fragment;
      std::vector<Fragment> child_fragments;
      bool success = false;
    };

    FlexLayoutResult computeFlexLayout(
      const crates::layout2::LayoutStyle &container_style,
      const std::vector<crates::layout2::LayoutStyle> &child_styles,
      const ConstraintSpace &space);

    /**
     * Compute grid layout using taffy, but without taffy managing the tree structure.
     * Returns layout results that can be integrated into the C++ layout tree.
     */
    struct GridLayoutResult
    {
      Fragment container_fragment;
      std::vector<Fragment> child_fragments;
      bool success = false;
    };

    GridLayoutResult computeGridLayout(
      const crates::layout2::LayoutStyle &container_style,
      const std::vector<crates::layout2::LayoutStyle> &child_styles,
      const ConstraintSpace &space);

    /**
     * Create a temporary taffy node for computation only.
     * These nodes are NOT part of the layout tree hierarchy.
     */
    std::unique_ptr<crates::layout2::Node> createComputationNode(
      const crates::layout2::LayoutStyle &style);

    /**
     * Create multiple computation nodes for batch layout computation.
     */
    std::vector<std::unique_ptr<crates::layout2::Node>> createComputationNodes(
      const std::vector<crates::layout2::LayoutStyle> &styles);

  private:
    /**
     * Convert taffy layout result to our Fragment format.
     */
    Fragment convertTaffyResult(const crates::layout2::LayoutOutput &output);

    /**
     * Set up temporary taffy tree for computation only.
     */
    std::unique_ptr<crates::layout2::Node> setupTemporaryTaffyTree(
      const crates::layout2::LayoutStyle &root_style,
      const std::vector<crates::layout2::LayoutStyle> &child_styles);

    std::shared_ptr<LayoutView> view_;
  };

  /**
   * Updated TaffyBasedFormattingContext that works within the new architecture.
   * Now uses TaffyIntegrationLayer for computations while tree management
   * is handled by LayoutTreeManager.
   */
  class TaffyIntegratedFormattingContext : public FormattingContext
  {
  public:
    TaffyIntegratedFormattingContext(const DisplayType type, std::shared_ptr<LayoutView> view);

  protected:
    Fragment liveFragment() const override;

    void onAdded(const FormattingContext &, std::shared_ptr<LayoutObject>) override final;
    void onRemoved(const FormattingContext &) override final;
    void onReplaced(const FormattingContext &, const FormattingContext &) override final;

    void contentSizeDidChange(const glm::vec3 &contentSize) override final;
    void setIsEmpty(bool) override final;
    bool setLayoutStyle(crates::layout2::LayoutStyle &) override;
    std::unique_ptr<const LayoutResult> computeLayout(const ConstraintSpace &) override;
    void debugPrint() const override final;

  protected:
    /**
     * Subclasses implement this to specify which taffy computation to use.
     */
    virtual TaffyIntegrationLayer::FlexLayoutResult computeTaffyLayout(
      const ConstraintSpace &space) = 0;

    /**
     * Collect child styles for taffy computation.
     * This integrates with LayoutTreeManager to get child information.
     */
    std::vector<crates::layout2::LayoutStyle> collectChildStyles();

  private:
    Fragment computed_fragment_;
    crates::layout2::LayoutStyle style_;
    bool needs_layout_ = true;
    std::shared_ptr<TaffyIntegrationLayer> taffy_integration_;
  };
}