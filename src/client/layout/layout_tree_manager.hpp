#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

#include "./constraint_space.hpp"
#include "./fragment.hpp"
#include "./display_type.hpp"

namespace client_layout
{
  class LayoutObject;
  class FormattingContext;
  class LayoutView;

  /**
   * LayoutTreeManager manages the complete layout tree hierarchy in C++,
   * independent of taffy. It coordinates between different formatting context types
   * (pure CSS, taffy-computed) and maintains unified tree management.
   *
   * This addresses @yorkie's requirement to move layout tree management entirely
   * to C++ and use taffy only for specific computations (flex/grid).
   */
  class LayoutTreeManager
  {
  public:
    explicit LayoutTreeManager(std::shared_ptr<LayoutView> view);

    /**
     * Register a layout object with its formatting context in the tree.
     */
    void registerLayoutObject(std::shared_ptr<LayoutObject> object,
                              std::shared_ptr<FormattingContext> context);

    /**
     * Unregister a layout object from the tree.
     */
    void unregisterLayoutObject(std::shared_ptr<LayoutObject> object);

    /**
     * Add child relationship between parent and child objects.
     * Handles integration between different formatting context types.
     */
    void addChild(std::shared_ptr<LayoutObject> parent,
                  std::shared_ptr<LayoutObject> child,
                  std::shared_ptr<LayoutObject> beforeSibling = nullptr);

    /**
     * Remove child relationship.
     */
    void removeChild(std::shared_ptr<LayoutObject> parent,
                     std::shared_ptr<LayoutObject> child);

    /**
     * Compute layout for the entire tree or a subtree.
     * Coordinates between pure CSS contexts and taffy-computed contexts.
     */
    bool computeLayout(std::shared_ptr<LayoutObject> root,
                       const ConstraintSpace &space);

    /**
     * Get layout result for a specific object.
     */
    Fragment getLayoutResult(std::shared_ptr<LayoutObject> object) const;

    /**
     * Check if an object is managed by this tree manager.
     */
    bool isManaged(std::shared_ptr<LayoutObject> object) const;

    /**
     * Debug print the layout tree structure.
     */
    void debugPrint() const;

  private:
    struct LayoutNode
    {
      std::shared_ptr<LayoutObject> object;
      std::shared_ptr<FormattingContext> context;
      std::vector<std::shared_ptr<LayoutNode>> children;
      std::weak_ptr<LayoutNode> parent;
      Fragment computed_fragment;
      bool needs_layout = true;
    };

    /**
     * Coordinate layout computation between different context types.
     */
    bool coordinateLayout(std::shared_ptr<LayoutNode> node,
                          const ConstraintSpace &space);

    /**
     * Handle integration when adding child to parent with different context types.
     */
    void integrateChildToParent(std::shared_ptr<LayoutNode> parent,
                                std::shared_ptr<LayoutNode> child,
                                size_t position);

    /**
     * Create layout node for object.
     */
    std::shared_ptr<LayoutNode> createLayoutNode(std::shared_ptr<LayoutObject> object,
                                                 std::shared_ptr<FormattingContext> context);

    /**
     * Find layout node for object.
     */
    std::shared_ptr<LayoutNode> findLayoutNode(std::shared_ptr<LayoutObject> object) const;

    std::shared_ptr<LayoutView> view_;
    std::shared_ptr<LayoutNode> root_;
    std::unordered_map<LayoutObject *, std::shared_ptr<LayoutNode>> object_to_node_;
  };
}