#pragma once

#include <memory>
#include <client/builtin_scene/scene.hpp>
#include <client/cssom/css_style_declaration.hpp>
#include <client/dom/types.hpp>

#include "./constraint_space.hpp"
#include "./formatting_contexts.hpp"
#include "./fragment.hpp"
#include "./layout_object_child_list.hpp"

namespace client_layout
{
  class LayoutBlock;
  class LayoutView;

  /**
   * The CSS box model implementation
   */
  class LayoutObject : public std::enable_shared_from_this<LayoutObject>
  {
    friend class LayoutObjectChildList;
    friend class TaffyBasedFormattingContext;

  protected:
    LayoutObject(std::shared_ptr<dom::Node> node);
    LayoutObject(const LayoutObject &) = delete;
    LayoutObject &operator=(const LayoutObject &) = delete;

  public:
    virtual ~LayoutObject() = default;

  public:
    virtual const char *name() const = 0;
    std::string toString() const;
    std::string debugName() const;

    virtual bool isBoxModelObject() const { return false; }
    virtual bool isBox() const { return false; }
    virtual bool isText() const { return false; }
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

    // Absolute or fixed positioning
    bool isRelativelyPositioned() const { return false; }
    bool isStickyPositioned() const { return false; }
    bool isFixedPositioned() const { return false; }
    bool isAbsolutelyPositioned() const { return false; }
    bool isPositioned() const { return false; }
    bool isInline() const { return false; }
    bool isAtomicInlineLevel() const { return false; }

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

    std::shared_ptr<client_cssom::CSSStyleDeclaration> style() const { return style_; }
    const client_cssom::CSSStyleDeclaration &styleRef() const
    {
      assert(style_ != nullptr);
      return *style_;
    }

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
    virtual void onChildReplaced(std::shared_ptr<LayoutObject> newChild, std::shared_ptr<LayoutObject> oldChild);

    virtual void addChild(std::shared_ptr<LayoutObject> newChild,
                          std::shared_ptr<LayoutObject> beforeChild = nullptr);
    virtual void removeChild(std::shared_ptr<LayoutObject> oldChild);
    virtual void replaceChild(std::shared_ptr<LayoutObject> newChild, std::shared_ptr<LayoutObject> oldChild);

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
    bool setStyle(const client_cssom::CSSStyleDeclaration &style);

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

  protected:
    FormattingContext &formattingContext() const { return *formattingContext_; }

    virtual void entityDidCreated(builtin_scene::ecs::EntityId entity);
    virtual void entityWillBeDestroyed(builtin_scene::ecs::EntityId entity);

    virtual void styleWillChange(const client_cssom::CSSStyleDeclaration &newStyle);
    virtual void styleDidChanged(const client_cssom::CSSStyleDeclaration &oldStyle);

    virtual void sizeWillChange(const Fragment &newSize);
    virtual void sizeDidChanged();

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
    std::shared_ptr<client_cssom::CSSStyleDeclaration> style_;
    std::optional<builtin_scene::ecs::EntityId> entity_;
    // TODO(yorkie): support fragments

  private: // Hierarchy fields
    /**
     * Use linked list to manage the hierarchy of the layout objects to improve the modification performance.
     */
    std::weak_ptr<LayoutObject> parent_;
    std::weak_ptr<LayoutObject> previous_;
    std::weak_ptr<LayoutObject> next_;
  };
}
