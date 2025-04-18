#pragma once

#include <memory>
#include <client/builtin_scene/scene.hpp>
#include <client/cssom/css_style_declaration.hpp>
#include <client/dom/types.hpp>

#include "./constraint_space.hpp"
#include "./formatting_contexts.hpp"
#include "./fragment.hpp"
#include "./layout_object_child_list.hpp"
#include "./hit_test_phase.hpp"
#include "./hit_test_ray.hpp"
#include "./hit_test_request.hpp"
#include "./hit_test_result.hpp"

namespace client_layout
{
  class LayoutBlock;
  class LayoutView;

  /**
   * The CSS box model implementation
   */
  class LayoutObject : public std::enable_shared_from_this<LayoutObject>
  {
    friend class LayoutView;
    friend class LayoutObjectChildList;
    friend class TaffyBasedFormattingContext;

  protected:
    LayoutObject(std::shared_ptr<dom::Node> node);
    LayoutObject(const LayoutObject &) = delete;
    LayoutObject &operator=(const LayoutObject &) = delete;

  public:
    virtual ~LayoutObject();

  public:
    virtual const char *name() const = 0;
    std::string toString() const;
    std::string debugName() const;

    virtual bool isBoxModelObject() const { return false; }
    virtual bool isBox() const { return false; }
    virtual bool isText() const { return false; }
    virtual bool isEmptyText() const { return false; }
    virtual bool isNone() const { return false; }
    virtual bool isFlexibleBox() const { return false; }
    virtual bool isLayoutGrid() const { return false; }
    virtual bool isLayoutImage() const { return false; }
    virtual bool isLayoutView() const { return false; }
    virtual bool isLayoutReplaced() const { return false; }
    virtual bool isLayoutBlock() const { return false; }
    virtual bool isLayoutBlockFlow() const { return false; }
    virtual bool isLayoutInline() const { return false; }

    bool isDocumentElement() const;
    bool isBody() const;

    virtual bool isSVG() const { return false; }
    virtual bool isSVGRoot() const { return false; }
    virtual bool isSVGChild() const { return isSVG() && !isSVGRoot(); }
    virtual bool isSVGContainer() const { return false; }

    bool isAnonymous() const
    {
      return node_.expired();
    }
    inline bool isAnonymousBlock() const
    {
      return isAnonymous() && isLayoutBlock();
    }

    // Returns `true` if this is a `LayoutBox` without physical fragments.
    virtual bool isFragmentLessBox() const { return false; }

    // Absolute or fixed positioning
    bool isRelativelyPositioned() const { return false; }
    bool isStickyPositioned() const { return false; }
    bool isFixedPositioned() const { return false; }
    bool isAbsolutelyPositioned() const { return false; }
    bool isPositioned() const { return false; }
    bool isInline() const { return false; }
    bool isAtomicInlineLevel() const { return false; }

    bool hasClip() const;
    bool isScrollContainer() const;

    inline std::shared_ptr<dom::Node> node() const { return node_.lock(); }
    inline dom::Node &nodeRef() const { return *node(); }

    std::shared_ptr<dom::HTMLDocument> document() const;
    dom::HTMLDocument &documentRef() const { return *document(); }

    std::shared_ptr<LayoutView> view();
    LayoutView &viewRef();
    std::shared_ptr<const LayoutView> view() const;
    const LayoutView &viewRef() const;

    void useSceneWithCallback(const std::function<void(builtin_scene::Scene &)> &);
    inline bool hasEntity() const { return entity_.has_value(); }
    builtin_scene::ecs::EntityId entity() const { return entity_.value(); }
    void createEntity();
    void destroyEntity();
    // Use the entity from the other layout object, this is useful when replacing the layout object.
    void useEntity(std::shared_ptr<LayoutObject> other);
    void destroy();

    std::optional<client_cssom::CSSStyleDeclaration> style() const;
    const client_cssom::CSSStyleDeclaration &styleRef() const;

    // Returns the current layout object's fragment.
    const Fragment fragment() const;

    std::shared_ptr<LayoutObject> parent() const { return parent_.lock(); }
    std::shared_ptr<LayoutObject> prevSibling() const { return previous_.lock(); }
    std::shared_ptr<LayoutObject> nextSibling() const { return next_.lock(); }

    bool isDescendantOf(std::shared_ptr<LayoutObject>) const;
    std::shared_ptr<LayoutObject> slowFirstChild() const;
    std::shared_ptr<LayoutObject> slowLastChild() const;

    virtual void onChildAdded(std::shared_ptr<LayoutObject> newChild, std::shared_ptr<LayoutObject> beforeChild);
    virtual void onChildRemoved(std::shared_ptr<LayoutObject> oldChild);

    virtual void addChild(std::shared_ptr<LayoutObject> newChild,
                          std::shared_ptr<LayoutObject> beforeChild = nullptr);
    virtual void removeChild(std::shared_ptr<LayoutObject> oldChild);

    virtual std::shared_ptr<LayoutObjectChildList> virtualChildren() { return nullptr; }
    virtual std::shared_ptr<LayoutObjectChildList> virtualChildren() const { return nullptr; }

    /**
     * Check if this box's associated entity has the specified scene component `T`.
     *
     * @tparam T The scene component type.
     * @returns Whether the scene component is found.
     */
    template <typename T>
    [[nodiscard]] bool hasSceneComponent() const
    {
      auto sceneRef = scene_.lock();
      if (TR_UNLIKELY(sceneRef == nullptr) || !entity_.has_value())
        return false;
      return sceneRef->hasComponent<T>(entity_.value());
    }

    /**
     * Get the scene's component of the given component type.
     *
     * @tparam T The scene component type.
     * @returns The scene component if found, otherwise `nullptr`.
     */
    template <typename T>
    [[nodiscard]] std::shared_ptr<T> getSceneComponent() const
    {
      auto sceneRef = scene_.lock();
      if (TR_UNLIKELY(sceneRef == nullptr) || !entity_.has_value())
        return nullptr;
      return sceneRef->getComponent<T>(entity_.value());
    }

    /**
     * Set the formatting context by the display type.
     *
     * @param display The display type to set.
     */
    void setFormattingContext(DisplayType display);

    /**
     * Update the style of the box.
     *
     * @param style The new style to update.
     * @returns `true` if the style is updated, otherwise `false`.
     */
    bool setStyle(client_cssom::CSSStyleDeclaration style);

    /**
     * This checks if there is a need to adjust the size of this object, and if so, it will adjust the size.
     *
     * @returns Whether the size is adjusted.
     */
    bool maybeAdjustSize();

    /**
     * It computes the layout of the box with the specified maximum width and height.
     *
     * @param avilableSpace The available space for the layout computation.
     * @returns Whether the layout is computed successfully.
     */
    virtual bool computeLayout(const ConstraintSpace &avilableSpace);

    /**
     * Prints the debug information of the formatting context.
     */
    void debugPrintFormattingContext() const;

    /**
     * It returns the containing block of the object.
     *
     * Due to CSS being inconsistent, a containing block can be a relatively positioned inline, thus we can't return a
     * `LayoutBlock` from this method.
     *
     * This method is extremely similar to containingBlock(), but with a few notable exceptions:
     * - For normal flow elements, it just returns the parent.
     * - For absolutely positioned elements, it will return a relative positioned inline. `containingBlock()` simply
     *   skips relative positioned inlines and lets an enclosing block handle the layout of the positioned object.
     *
     * This function should be used for any invalidation as it would correctly walk the containing block chain. It is
     * also used for correctly sizing absolutely positioned elements.
     */
    std::shared_ptr<LayoutObject> container() const;
    // Finds the container as if this object is absolute-position.
    std::shared_ptr<LayoutObject> containerForFixedPosition() const;
    // Finds the container as if this object is fixed-position.
    std::shared_ptr<LayoutObject> containerForAbsolutePosition() const;

    /**
     * Returns `true` if style would make this object a fixed container.
     *
     * @param style The style to check.
     * @returns Whether the box is a fixed container.
     */
    bool computeIsFixedContainer(const client_cssom::CSSStyleDeclaration &style) const;

    /**
     * Returns `true` if style would make this object an absolute container.
     *
     * @param style The style to check.
     * @returns Whether the box is an absolute container.
     */
    bool computeIsAbsoluteContainer(const client_cssom::CSSStyleDeclaration &style) const;

    /**
     * It returns an enclosing non-anonymous block box for this element.
     *
     * LayoutBlocks are the one that handle laying out positioned elements, thus this function is important during
     * layout, to insert the positioned elements into the correct block.
     */
    std::shared_ptr<LayoutBlock> containingBlock() const;
    std::shared_ptr<LayoutBlock> containingBlockForFixedPosition() const;
    std::shared_ptr<LayoutBlock> containingBlockForAbsolutePosition() const;

    std::shared_ptr<const LayoutBlock> containingScrollContainer() const;

    bool isHorizontalWritingMode() const { return bitfields_.HorizontalWritingMode(); }
    bool hasNonVisibleOverflow() const { return bitfields_.HasNonVisibleOverflow(); }
    bool hasValidCachedGeometry() const { return bitfields_.HasValidCachedGeometry(); }

    void setHorizontalWritingMode(bool b) { bitfields_.SetHorizontalWritingMode(b); }
    void setHasNonVisibleOverflow(bool b) { bitfields_.SetHasNonVisibleOverflow(b); }
    void setHasValidCachedGeometry(bool b) { bitfields_.SetHasValidCachedGeometry(b); }

    bool visibleToHitTestRequest(const HitTestRequest &) const;
    bool visibleToHitTesting() const;

    virtual bool hitTestAllPhases(HitTestResult &, const HitTestRay &, const glm::vec3 &accumulatedOffset);
    // Returns the node that is ultimately added to the hit test result. Some objects report a hit testing node that is
    // not their own (such as continuations and some psuedo elements) and it is important that the node be consistent
    // between point- and list-based hit test results.
    virtual std::shared_ptr<dom::Node> nodeForHitTest() const;
    virtual void updateHitTestResult(HitTestResult &, const glm::vec3 &point) const;
    virtual bool nodeAtPoint(HitTestResult &, const HitTestRay &, const glm::vec3 &accumulatedOffset,
                             HitTestPhase) { return false; }

  protected:
    FormattingContext &formattingContext() const { return *formattingContext_; }

    virtual void entityDidCreate(builtin_scene::ecs::EntityId entity);
    virtual void entityWillBeDestroyed(builtin_scene::ecs::EntityId entity);

    virtual void styleWillChange(client_cssom::CSSStyleDeclaration &newStyle);
    virtual void styleDidChange();

    virtual void sizeWillChange(const Fragment &newSize);
    virtual void sizeDidChange();

    virtual void willComputeLayout(const ConstraintSpace &);
    virtual void didComputeLayoutOnce(const ConstraintSpace &);

  private:
    void setParent(std::shared_ptr<LayoutObject> parent) { parent_ = parent; }
    void setPrevSibling(std::shared_ptr<LayoutObject> prev) { previous_ = prev; }
    void setNextSibling(std::shared_ptr<LayoutObject> next) { next_ = next; }

    inline bool isTextOrSVGChild() const
    {
      return isText() || isSVGChild();
    }

    // Resize the layout object with the given size.
    bool resize(const Fragment &newSize);

  private:
    std::shared_ptr<FormattingContext> formattingContext_;
    std::weak_ptr<dom::Node> node_;
    std::weak_ptr<builtin_scene::Scene> scene_;
    // TODO(yorkie): will be replaced by the computed style type.
    std::optional<builtin_scene::ecs::EntityId> entity_;
    // TODO(yorkie): support fragments

  private: // Hierarchy fields
    /**
     * Use linked list to manage the hierarchy of the layout objects to improve the modification performance.
     */
    std::weak_ptr<LayoutObject> parent_;
    std::weak_ptr<LayoutObject> previous_;
    std::weak_ptr<LayoutObject> next_;

  private: // LayoutObjectBitfields: holds all the boolean values for `LayoutObject`.
#define ADD_BOOLEAN_BITFIELD(field_name_, MethodNameBase)               \
public:                                                                 \
  bool MethodNameBase() const { return field_name_; }                   \
  void Set##MethodNameBase(bool new_value) { field_name_ = new_value; } \
                                                                        \
private:                                                                \
  unsigned field_name_ : 1

    class LayoutObjectBitfields
    {
    public:
      explicit LayoutObjectBitfields()
          : floating_(false),
            horizontal_writing_mode_(true),
            has_non_visible_overflow_(false),
            has_valid_cached_geometry_(false)
      {
      }

      ADD_BOOLEAN_BITFIELD(floating_, Floating);
      ADD_BOOLEAN_BITFIELD(horizontal_writing_mode_, HorizontalWritingMode);

      // This boolean is set if this object is a root scroller.
      ADD_BOOLEAN_BITFIELD(is_global_root_scroller_, IsGlobalRootScroller);

      // This boolean is set if overflow != 'visible'.
      // This means that this object may need an overflow clip to be applied at paint time to its visual overflow (see
      // `OverflowModel` for more details). Only set for LayoutBoxes and descendants.
      ADD_BOOLEAN_BITFIELD(has_non_visible_overflow_, HasNonVisibleOverflow);

      // `true` if `LayoutBox::frame_size_` has the latest value computed from its physical fragments.
      // This is set to false when `LayoutBox::layout_results_` is updated.
      ADD_BOOLEAN_BITFIELD(has_valid_cached_geometry_, HasValidCachedGeometry);
    };
#undef ADD_BOOLEAN_BITFIELD

    LayoutObjectBitfields bitfields_;
  };
}
