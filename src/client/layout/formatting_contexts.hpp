#pragma once

#include <cmath>
#include <concepts>
#include <memory>
#include <glm/glm.hpp>
#include <crates/bindings.hpp>

#include "./display_type.hpp"
#include "./constraint_space.hpp"
#include "./fragment.hpp"
#include "./layout_result.hpp"

namespace client_layout
{
  class LayoutView;
  class LayoutObject;
  class FormattingContextsChildList;

  class FormattingContext
  {
  public:
    static std::unique_ptr<FormattingContext> Make(const DisplayType display, std::shared_ptr<LayoutView> view);

  protected:
    FormattingContext(const DisplayType type, std::shared_ptr<LayoutView> view);

  public:
    virtual ~FormattingContext() = default;

  public:
    virtual bool isInline() const
    {
      return false;
    }
    virtual bool isBlock() const
    {
      return false;
    }
    virtual bool isFlow() const
    {
      return false;
    }
    virtual bool isFlex() const
    {
      return false;
    }
    virtual bool isGrid() const
    {
      return false;
    }

    // `computeLayout` updates this fragment with the resulting geometry information.
    const Fragment &resultingFragment() const
    {
      return resulting_fragment_;
    }
    virtual Fragment liveFragment() const = 0;

    // Should call this method when the node is added to a parent context.
    virtual void onAdded(const FormattingContext &parent,
                         std::shared_ptr<LayoutObject> beforeChild = nullptr) = 0;
    // Should call this method when the node is removed from a parent context.
    virtual void onRemoved(const FormattingContext &parent) = 0;
    // Should call this method when the node is replaced by a new node.
    virtual void onReplaced(const FormattingContext &parent, const FormattingContext &old) = 0;

    // Set the content size of the formatting context.
    void setContentSize(const glm::vec3 &size);
    // Set the content size with width and height.
    inline void setContentSize(float width, float height)
    {
      setContentSize(glm::vec3(width, height, 0));
    }
    // Enable or disable the content size, only the replaced element needs to use content size.
    inline void setContentSizeEnabled(bool b)
    {
      is_content_size_enabled_ = b;
    }
    inline void resetContentSize()
    {
      content_size_ = std::nullopt;
    }
    virtual void contentSizeDidChange(const glm::vec3 &contentSize)
    {
    }

    // Set if the formatting context node is empty, which will be ignored in the layout.
    virtual void setIsEmpty(bool);
    virtual bool setLayoutStyle(crates::layout2::LayoutStyle &);
    virtual std::unique_ptr<const LayoutResult> computeLayout(const ConstraintSpace &) = 0;

    // Print the debug information of the formatting context.
    virtual void debugPrint() const = 0;

  public:
    DisplayType type;

  protected:
    std::weak_ptr<LayoutView> view_;
    std::optional<glm::vec3> content_size_ = std::nullopt;
    Fragment resulting_fragment_;
    bool is_empty_ = false;

    // flags to indicate if this node should update the layout size when the content size is changed.
    bool use_content_x_ = false;
    bool use_content_y_ = false;
    bool is_content_size_enabled_ = false;
  };

  class TaffyBasedFormattingContext : public FormattingContext
  {
  public:
    TaffyBasedFormattingContext(const DisplayType type, std::shared_ptr<LayoutView> view);

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

  private:
    void updateNodeStyle(const crates::layout2::LayoutStyle &style);

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
    bool isBlock() const override final
    {
      return true;
    }
  };

  class BlockFlowFormattingContext : public BlockFormattingContext<>
  {
    using BlockFormattingContext<>::BlockFormattingContext;

  private:
    bool isFlow() const override final
    {
      return true;
    }
  };

  class FlexFormattingContext : public BlockFormattingContext<DisplayInside::kFlex>
  {
    using BlockFormattingContext<DisplayInside::kFlex>::BlockFormattingContext;

  private:
    bool isFlex() const override final
    {
      return true;
    }
  };

  class GridFormattingContext : public BlockFormattingContext<DisplayInside::kGrid>
  {
    using BlockFormattingContext<DisplayInside::kGrid>::BlockFormattingContext;

  private:
    bool isGrid() const override final
    {
      return true;
    }
  };

  /**
   * InlineFormattingContext implements true inline layout behavior without relying on taffy.
   * This handles line wrapping, baseline alignment, and proper inline box model behavior.
   * Unlike block layout, inline layout flows text horizontally and wraps to new lines.
   * 
   * For integration with taffy-based parents, this context creates a placeholder taffy node
   * that represents the space taken by the inline layout, while the actual inline positioning
   * is computed using custom algorithms.
   */
  class InlineFormattingContext : public FormattingContext
  {
  public:
    InlineFormattingContext(std::shared_ptr<LayoutView> view);

  protected:
    Fragment liveFragment() const override;

    void onAdded(const FormattingContext &parent, std::shared_ptr<LayoutObject> beforeChild = nullptr) override;
    void onRemoved(const FormattingContext &parent) override;
    void onReplaced(const FormattingContext &parent, const FormattingContext &old) override;

    void contentSizeDidChange(const glm::vec3 &contentSize) override;
    void setIsEmpty(bool) override;
    bool setLayoutStyle(crates::layout2::LayoutStyle &) override;
    std::unique_ptr<const LayoutResult> computeLayout(const ConstraintSpace &) override;
    void debugPrint() const override;

  private:
    bool isInline() const override final
    {
      return true;
    }
    bool isFlow() const override final
    {
      return true;
    }

    // Inline-specific layout computation
    void computeInlineLayout(const ConstraintSpace &space);

    // Update the taffy placeholder node with our computed size
    void updateTaffyPlaceholder();

    // Line box management for inline layout
    struct LineBox
    {
      float baseline = 0.0f;
      float ascent = 0.0f;
      float descent = 0.0f;
      float width = 0.0f;
      float height = 0.0f;
      std::vector<std::shared_ptr<FormattingContext>> inline_boxes;
    };

    std::vector<LineBox> line_boxes_;

    // Current computed size for this inline formatting context
    glm::vec2 computed_size_{0.0f, 0.0f};

    // Track if we need layout recomputation
    bool needs_layout_ = true;

    // Taffy placeholder node for integration with taffy-based parents
    // This node represents the space taken by our inline layout in the taffy tree
    std::unique_ptr<crates::layout2::Node> taffy_placeholder_;
  };
}
