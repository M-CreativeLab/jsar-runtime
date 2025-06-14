#pragma once

#include <memory>
#include <string>
#include <crates/bindings.hpp>
#include <client/browser/types.hpp>
#include <client/dom/types.hpp>

#include "./geometry/viewport3d.hpp"
#include "./layout_block_flow.hpp"
#include "./layout_box_model_object.hpp"
#include "./layout_text.hpp"
#include "./hit_test_cache.hpp"
#include "./hit_test_ray.hpp"
#include "./hit_test_result.hpp"

namespace client_layout
{
  /**
   * `LayoutView` is the root of the layout tree.
   */
  class LayoutView : public LayoutBlockFlow
  {
  public:
    // Construct a new `LayoutView` instance.
    static std::shared_ptr<LayoutView> Make(std::shared_ptr<dom::Document>, const browser::Window &);

  public:
    /**
     * @deprecated Use `LayoutView::Make` instead.
     */
    LayoutView(std::shared_ptr<dom::Document>, const browser::Window &);

  public:
    const char *name() const final override
    {
      return "LayoutView";
    }
    bool isLayoutView() const final override
    {
      return true;
    }

    crates::layout2::Allocator &taffyNodeAllocatorRef() const
    {
      return *taffy_node_allocator_;
    }
    bool computeLayout(const ConstraintSpace &avilableSpace) override final;

    bool hitTest(const HitTestRay &, HitTestResult &);
    bool hitTestNoLifecycleUpdate(const HitTestRay &, HitTestResult &);

    size_t hitTestCount() const
    {
      return hit_test_count_;
    }
    size_t hitTestCacheHits() const
    {
      return hit_test_cache_hits_;
    }
    void clearHitTestCache();

    size_t computeMinimumWidth();

    // The options for `debugPrint()`.
    struct DebugOptions
    {
      bool disabled;
      // Print the *FC information.
      bool printFormattingContext = false;
      // Show the timestamp for the debug information.
      bool showTimestamp = false;
      // Show the text content for `LayoutText`.
      bool showTextContent = true;
      // Show the block recursively.
      bool showBlockRecursively = true;

      DebugOptions(bool disabled = false)
          : disabled(disabled)
      {
      }

      // Disable the debug information or not.
      inline DebugOptions withDisabled(bool disabled = true) const
      {
        auto nextOptions = *this;
        nextOptions.disabled = disabled;
        return nextOptions;
      }

      // Enable or disable the formatting context debug information.
      inline DebugOptions withFormattingContext(bool enabled = true) const
      {
        auto nextOptions = *this;
        nextOptions.printFormattingContext = enabled;
        return nextOptions;
      }
      // Enable or disable the timestamp debug information.
      inline DebugOptions withTimestamp(bool enabled = true) const
      {
        auto nextOptions = *this;
        nextOptions.showTimestamp = enabled;
        return nextOptions;
      }
      // Enable or disable the text content debug information.
      inline DebugOptions withTextContent(bool enabled = true) const
      {
        auto nextOptions = *this;
        nextOptions.showTextContent = enabled;
        return nextOptions;
      }
      // Enable or disable showing the block recursively.
      inline DebugOptions withBlockRecursively(bool enabled = true) const
      {
        auto nextOptions = *this;
        nextOptions.showBlockRecursively = enabled;
        return nextOptions;
      }

      // The default options for `debugPrint()`, minimal enabled debug information:
      // - No formatting context.
      // - No timestamp.
      // - Enable text content.
      // - Enable showing block recursively.
      static DebugOptions Default()
      {
        return DebugOptions(false);
      }
      // The disabled options for `debugPrint()`, no debug information.
      static DebugOptions Disabled()
      {
        return DebugOptions(true);
      }
      // The full options for `debugPrint()`, all debug information enabled.
      static DebugOptions Full()
      {
        return Default()
          .withFormattingContext(true)
          .withTimestamp(true)
          .withTextContent(true)
          .withBlockRecursively(true);
      }
    };
    // Print the debug information of the layout tree.
    void debugPrint(const std::string &message = "Default", DebugOptions options = DebugOptions::Default()) const;

  public:
    /**
     * Create a new box with the initial display type.
     *
     * @param display The display type to create the box.
     * @param element The element to create the box.
     * @param parentBlock The parent block to add the box.
     * @param beforeObject The reference box to insert the new box before.
     * @returns The new box.
     */
    std::shared_ptr<LayoutBoxModelObject> createBox(const std::string &display,
                                                    std::shared_ptr<dom::Element> element,
                                                    std::shared_ptr<LayoutBlock> parentBlock,
                                                    std::shared_ptr<LayoutObject> beforeObject = nullptr);

    /**
     * Create a new layout text object.
     *
     * @param textNode The text node to create the text object.
     * @param parentBox The parent box of the text object.
     * @returns The new text object.
     */
    std::shared_ptr<LayoutText> createText(std::shared_ptr<dom::Text> textNode,
                                           std::shared_ptr<LayoutBoxModelObject> parentBox);

    /**
     * Remove the child layout object.
     *
     * @param object The child object to remove.
     */
    void removeObject(std::shared_ptr<LayoutObject> object);

  private:
    std::unique_ptr<LayoutBoxModelObject> makeBox(const std::string &displayStr,
                                                  std::shared_ptr<dom::Element> element);
    std::unique_ptr<LayoutText> makeText(std::shared_ptr<dom::Text> textNode);

  public:
    geometry::Viewport3d viewport;

  private:
    // JSAR uses taffy for block, flex and grid layout, this allocator is used to create the taffy nodes.
    std::shared_ptr<crates::layout2::Allocator> taffy_node_allocator_;

    size_t hit_test_count_;
    size_t hit_test_cache_hits_;
    std::unique_ptr<HitTestCache> hit_test_cache_;
  };
}
