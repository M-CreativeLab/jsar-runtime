#pragma once

#include <concepts>
#include <memory>
#include <crates/bindings.hpp>

#include "./display_type.hpp"
#include "./constraint_space.hpp"
#include "./fragment.hpp"
#include "./layout_result.hpp"

namespace client_layout
{
  class LayoutView;
  class LayoutObject;

  class FormattingContext
  {
  public:
    static std::unique_ptr<FormattingContext> Make(const DisplayType display, std::shared_ptr<LayoutView> view);

  protected:
    FormattingContext(const DisplayType type, std::shared_ptr<LayoutView> view);

  public:
    virtual ~FormattingContext() = default;

  public:
    virtual bool isInline() const { return false; }
    virtual bool isBlock() const { return false; }
    virtual bool isFlow() const { return false; }
    virtual bool isFlex() const { return false; }
    virtual bool isGrid() const { return false; }

    // `computeLayout` updates this fragment with the resulting geometry information.
    const Fragment &resultingFragment() const { return resultingFragment_; }
    virtual Fragment liveFragment() const = 0;

    // Should call this method when the node is added to a parent context.
    virtual void onAdded(const FormattingContext &parent) = 0;
    // Should call this method when the node is removed from a parent context.
    virtual void onRemoved(const FormattingContext &parent) = 0;
    // Should call this method when the node is replaced by a new node.
    virtual void onReplaced(const FormattingContext &parent, const FormattingContext &old) = 0;

    virtual bool setLayoutStyle(const crates::layout2::LayoutStyle &style) = 0;
    virtual std::unique_ptr<const LayoutResult> computeLayout(const ConstraintSpace &) = 0;
    virtual void debugPrint() const = 0;

  public:
    DisplayType type;

  protected:
    std::weak_ptr<LayoutView> view_;
    Fragment resultingFragment_;
  };

  class TaffyBasedFormattingContext : public FormattingContext
  {
  public:
    TaffyBasedFormattingContext(const DisplayType type, std::shared_ptr<LayoutView> view);

  protected:
    Fragment liveFragment() const override;

    void onAdded(const FormattingContext &) override final;
    void onRemoved(const FormattingContext &) override final;
    void onReplaced(const FormattingContext &, const FormattingContext &) override final;

    bool setLayoutStyle(const crates::layout2::LayoutStyle &style) override;
    std::unique_ptr<const LayoutResult> computeLayout(const ConstraintSpace &) override;
    void debugPrint() const override final;

  protected:
    // taffy binding will remove the node itself from taffy tree when it's destructed.
    std::unique_ptr<crates::layout2::Node> node_;
  };

  // Check if the display type is supported by Taffy: kFlex, kGrid, kFlow, kFlowRoot.
  template <DisplayInside Inner>
  concept is_taffy_supported = (Inner == DisplayInside::kFlex ||
                                Inner == DisplayInside::kGrid ||
                                Inner == DisplayInside::kFlow ||
                                Inner == DisplayInside::kFlowRoot);

  /**
   * The template class for flex or grid formatting context.
   *
   * @tparam D The display type: such as DisplayInside::kFlex, DisplayInside::kGrid, etc.
   */
  template <DisplayInside D = DisplayInside::kFlow>
    requires is_taffy_supported<D>
  class BlockFormattingContext : public TaffyBasedFormattingContext
  {
  public:
    BlockFormattingContext(std::shared_ptr<LayoutView> view);

  private:
    // Taffy implements the layout algorithm for the block formatting context including flow, flex and grid.
    bool isBlock() const override final { return true; }
  };

  class BlockFlowFormattingContext : public BlockFormattingContext<>
  {
    using BlockFormattingContext<>::BlockFormattingContext;

  private:
    bool isFlow() const override final { return true; }
  };

  class FlexFormattingContext : public BlockFormattingContext<DisplayInside::kFlex>
  {
    using BlockFormattingContext<DisplayInside::kFlex>::BlockFormattingContext;

  private:
    bool isFlex() const override final { return true; }
  };

  class GridFormattingContext : public BlockFormattingContext<DisplayInside::kGrid>
  {
    using BlockFormattingContext<DisplayInside::kGrid>::BlockFormattingContext;

  private:
    bool isGrid() const override final { return true; }
  };

  class InlineFormattingContext : public FormattingContext
  {
  public:
    InlineFormattingContext(std::shared_ptr<LayoutView> view)
        : FormattingContext(DisplayType::Inline(), view)
    {
    }

  public:
    Fragment liveFragment() const override;
    void onAdded(const FormattingContext &) override final;
    void onRemoved(const FormattingContext &) override final;
    void onReplaced(const FormattingContext &, const FormattingContext &) override final;

    bool setLayoutStyle(const crates::layout2::LayoutStyle &style) override;
    std::unique_ptr<const LayoutResult> computeLayout(const ConstraintSpace &) override;
    void debugPrint() const override final;

  private:
    bool isInline() const override final { return true; }
    bool isFlow() const override final { return true; }

  private:
    // TODO: Implement the inline formatting context.
    FormattingContext *parent_ = nullptr;
  };
}
