#include <vector>
#include <client/cssom/types/transform.hpp>
#include <client/dom/node.hpp>
#include <client/dom/document-inl.hpp>
#include <client/html/all_html_elements.hpp>

#include "./layout_object.hpp"
#include "./layout_block.hpp"
#include "./layout_view.hpp"

namespace client_layout
{
  using namespace std;
  using namespace builtin_scene;
  using namespace client_cssom;

  LayoutObject::LayoutObject(shared_ptr<dom::Node> node)
      : node_(node),
        formattingContext_(nullptr)
  {
    if (dom::Node::Is<dom::Document>(node))
      scene_ = dom::Node::As<dom::Document>(node)->scene;
    else
      scene_ = node->getOwnerDocumentReference()->scene;
  }

  LayoutObject::~LayoutObject()
  {
  }

  string LayoutObject::toString() const
  {
    return node() != nullptr ? node()->toString() : "(anonymous)";
  }

  string LayoutObject::debugName() const
  {
    stringstream ss;
    ss << name() << " ";
    if (node() != nullptr)
    {
      ss << "{";
      if (dom::Node::Is<dom::Element>(node())) // Use the `tagName` for the element node.
        ss << dom::Node::AsChecked<dom::Element>(node()).tagName;
      else
        ss << node()->nodeName;
      ss << "}";
    }
    else
      ss << "(anonymous)";
    return ss.str();
  }

  bool LayoutObject::isDocumentElement() const
  {
    return document()->documentElement() == node();
  }

  bool LayoutObject::isBody() const
  {
    return node() != nullptr && dom::Node::Is<dom::HTMLBodyElement>(node());
  }

  bool LayoutObject::hasClip() const
  {
    auto element = dom::Node::As<dom::Element>(node());
    if (element == nullptr)
      return false;

    const auto &elementStyle = element->adoptedStyleRef();
    if (elementStyle.hasProperty("clip"))
    {
      auto clip = elementStyle.getPropertyValue("clip");
      if (clip != "auto")
        return true;
    }
    return false;
  }

  bool LayoutObject::isScrollContainer() const
  {
    // Replaced elements don't support scrolling.
    if (isLayoutReplaced())
      return false;

    // LayoutView is always a scroll container.
    if (isLayoutView())
      return true;

    auto element = dom::Node::As<dom::Element>(node());
    if (element == nullptr)
      return false;

    if (!hasNonVisibleOverflow())
      return false;

    const auto &elementStyle = element->adoptedStyleRef();

    // An overflow value of `visible` or `clip` means that the element is a scroll container, all other values result
    // in a scrollable container. Also note that if `visible` or `clip` is set on one axis, then the other axis must be
    // set to `visible` or `clip` as well.
    bool isScrollableInX = false;
    bool isScrollableInY = false;
    if (elementStyle.hasProperty("overflow-x"))
    {
      auto overflowX = elementStyle.getPropertyValue("overflow-x");
      if (overflowX != "visible" && overflowX != "clip")
        isScrollableInX = true;
    }
    if (elementStyle.hasProperty("overflow-y"))
    {
      auto overflowY = elementStyle.getPropertyValue("overflow-y");
      if (overflowY != "visible" && overflowY != "clip")
        isScrollableInY = true;
    }
    return isScrollableInX || isScrollableInY;
  }

  shared_ptr<dom::HTMLDocument> LayoutObject::document() const
  {
    assert(node() != nullptr || parent() != nullptr);
    if (node() == nullptr)
      return parent()->document();

    if (dom::Node::Is<dom::Document>(node())) // Document will be the document itself.
      return dom::Node::As<dom::HTMLDocument>(node());
    else
      return node()->getOwnerDocumentReferenceAs<dom::HTMLDocument>();
  }

  shared_ptr<LayoutView> LayoutObject::view()
  {
    return isLayoutView()
               ? dynamic_pointer_cast<LayoutView>(shared_from_this())
               : document()->layoutView();
  }

  LayoutView &LayoutObject::viewRef()
  {
    return isLayoutView()
               ? dynamic_cast<LayoutView &>(*this)
               : document()->layoutViewRef();
  }

  shared_ptr<const LayoutView> LayoutObject::view() const
  {
    return isLayoutView()
               ? dynamic_pointer_cast<const LayoutView>(shared_from_this())
               : document()->layoutView();
  }

  const LayoutView &LayoutObject::viewRef() const
  {
    return isLayoutView()
               ? dynamic_cast<const LayoutView &>(*this)
               : document()->layoutViewRef();
  }

  void LayoutObject::useSceneWithCallback(const function<void(builtin_scene::Scene &)> &callback)
  {
    auto sceneRef = scene_.lock();
    if (sceneRef != nullptr)
      callback(*sceneRef);
  }

  void LayoutObject::createEntity()
  {
    if (TR_UNLIKELY(!node()->isElementOrText() && !node()->isDocument()))
    {
      assert(false && "The node in `LayoutObject` must be an element or text node.");
      return;
    }

    // 1. Create the entity
    auto createEntity = [this](Scene &scene)
    {
      entity_ = scene.createElement(node()->nodeName, node(),
                                    parent() == nullptr ? nullopt : parent()->entity_);
    };

    useSceneWithCallback(createEntity);
    entityDidCreate(entity_.value());
  }

  void LayoutObject::destroyEntity()
  {
    if (TR_UNLIKELY(!hasEntity()))
      return;

    auto removeEntity = [this](Scene &scene)
    {
      scene.removeElement(entity_.value());
      entity_ = nullopt;
    };

    entityWillBeDestroyed(entity_.value());
    useSceneWithCallback(removeEntity);
  }

  void LayoutObject::useEntity(shared_ptr<LayoutObject> other)
  {
    entity_ = other->entity_;
    // TODO: update the Element
  }

  void LayoutObject::destroy()
  {
    destroyEntity();

    auto children = virtualChildren();
    if (children != nullptr)
    {
      vector<shared_ptr<LayoutObject>> listToReset; // List to be reset.
      for (auto child : *children)
      {
        child->destroy();
        listToReset.push_back(child); // Push the child to the list which will be reset later.
      }

      // Reset all the children after `destroy()` to avoid the dangling pointer.
      for (auto item : listToReset)
      {
        item->setParent(nullptr);
        item->setPrevSibling(nullptr);
        item->setNextSibling(nullptr);
      }
    }
  }

  optional<client_cssom::CSSStyleDeclaration> LayoutObject::style() const
  {
    if (TR_UNLIKELY(!node()->isElementOrText()))
      return nullopt;

    if (node()->isElement())
    {
      auto element = dom::Node::As<dom::Element>(node());
      if (element != nullptr)
        return element->adoptedStyleRef();
    }
    else if (node()->isText())
    {
      auto textNode = dom::Node::As<dom::Text>(node());
      if (textNode != nullptr)
        return textNode->adoptedStyleRef();
    }
    return nullopt;
  }

  const client_cssom::CSSStyleDeclaration &LayoutObject::styleRef() const
  {
    assert(node()->isElementOrText() && "The node must be an element or text node.");
    if (node()->isElement())
    {
      auto element = dom::Node::As<dom::Element>(node());
      return element->adoptedStyleRef();
    }
    else if (node()->isText())
    {
      auto textNode = dom::Node::As<dom::Text>(node());
      return textNode->adoptedStyleRef();
    }
    assert(false && "Unrachable");
  }

  const Fragment LayoutObject::computeOrGetFragment(FragmentDifference &diff) const
  {
    Fragment nodeFragment = formattingContext_->liveFragment();
    if (isText())
    {
      auto layoutText = dynamic_pointer_cast<const LayoutText>(shared_from_this());
      assert(layoutText != nullptr && "Text node must be a LayoutText.");
      if (layoutText->isEmptyText())
        nodeFragment = Fragment::None(); // Set the fragment to none if a text and empty content.
    }

    assert(formattingContext_ != nullptr && "Formatting context must be set.");
    if (parent() == nullptr)
    {
      if (diff.enabled)
      {
        diff.changed = mutateAccumulatedFragment(nodeFragment);
        return accumulated_fragment_.value();
      }
      else
        return nodeFragment;
    }
    else
    {
      auto parentBox = parent();
      Fragment baseFragment = parentBox->accumulatedFragment();

      // Move the fragment by the scroll offset if the object is a scroll container.
      if (parentBox->isBox() && parentBox->isScrollContainer())
      {
        auto scrollableArea = dynamic_pointer_cast<const LayoutBox>(parentBox)->getScrollableArea();
        if (scrollableArea != nullptr)
        {
          auto offset = scrollableArea->getScrollOffset();
          baseFragment.moveBy(offset.x, offset.y, offset.z);
        }
      }

      // Returns the fragment with the parent's offset.
      auto &finalFragment = baseFragment.position(nodeFragment);
      if (diff.enabled)
      {
        diff.changed = mutateAccumulatedFragment(finalFragment);
        return accumulated_fragment_.value();
      }
      else
      {
        return finalFragment;
      }
    }
  }

  const Fragment LayoutObject::fragment() const
  {
    auto _ = FragmentDifference::Disabled();
    return computeOrGetFragment(_);
  }

  bool LayoutObject::isDescendantOf(shared_ptr<LayoutObject> object) const
  {
    auto r = parent();
    while (r != nullptr)
    {
      if (r == object)
        return true;
      r = r->parent();
    }
    return false;
  }

  shared_ptr<LayoutObject> LayoutObject::slowFirstChild() const
  {
    const auto children = virtualChildren();
    if (children != nullptr)
      return children->firstChild();
    return nullptr;
  }

  shared_ptr<LayoutObject> LayoutObject::slowLastChild() const
  {
    const auto children = virtualChildren();
    if (children != nullptr)
      return children->lastChild();
    return nullptr;
  }

  void LayoutObject::onChildAdded(shared_ptr<LayoutObject> newChild, shared_ptr<LayoutObject> beforeChild)
  {
    assert(newChild != nullptr && "The new child must be set.");
    assert(newChild->formattingContext_ != nullptr && "The formatting context must be set for the new child.");
    assert(formattingContext_ != nullptr && "The formatting context must be set for the parent.");

    auto &parentCtx = *formattingContext_;
    newChild->formattingContext_->onAdded(parentCtx, beforeChild);
  }

  void LayoutObject::onChildRemoved(shared_ptr<LayoutObject> child)
  {
    assert(child != nullptr && "The child must be set.");
    assert(child->formattingContext_ != nullptr && "The formatting context must be set for the child.");

    child->formattingContext_->onRemoved(*formattingContext_);
    child->destroy();
  }

  void LayoutObject::addChild(shared_ptr<LayoutObject> newChild, shared_ptr<LayoutObject> beforeChild)
  {
    auto children = virtualChildren();
    if (!children)
      return;

    // TODO: special handling for the first child.
    children->insertChildNode(shared_from_this(), newChild, beforeChild);
    onChildAdded(newChild, beforeChild);
  }

  void LayoutObject::removeChild(shared_ptr<LayoutObject> oldChild)
  {
    auto children = virtualChildren();
    if (!children)
      return;

    children->removeChildNode(shared_from_this(), oldChild);
    onChildRemoved(oldChild);
  }

  void LayoutObject::setFormattingContext(DisplayType display)
  {
    formattingContextWillSet(display);
    formattingContext_ = FormattingContext::Make(display, view());
    if (formattingContext_ != nullptr)
      formattingContextDidSet(*formattingContext_);
  }

  bool LayoutObject::setStyle(CSSStyleDeclaration style)
  {
    styleWillChange(style);

    // Update the `WebContent` style.
    if (hasEntity())
    {
      auto webContent = getSceneComponent<WebContent>();
      if (webContent != nullptr)
      {
        shared_ptr<WebContent> parentContent = nullptr;
        if (parent() != nullptr)
          parentContent = parent()->getSceneComponent<WebContent>();
        webContent->setStyle(style, parentContent);
      }
    }

    // Update the layout style in formatting context.
    crates::layout2::LayoutStyle layoutStyle = style;
    bool success = formattingContext_->setLayoutStyle(layoutStyle);

    styleDidChange();
    return success;
  }

  bool LayoutObject::maybeAdjustSize()
  {
    // The method `resize` will check if the size is changed.
    return resize(fragment());
  }

  bool LayoutObject::resize(const Fragment &newSize)
  {
    sizeWillChange(newSize);

    bool resized = false;
    auto resizeEntity = [this, &resized, &newSize](Scene &scene)
    {
      if (TR_LIKELY(hasEntity()))
      {
        if (node()->enableCustomGeometry())
        {
          // TODO: Skip the resizing for the mesh element.
          cerr << "The custom geometry node resizing is not supported yet." << endl;
          resized = true;
        }
        else
        {
          auto &webContent = scene.getComponentChecked<WebContent>(entity_.value());
          resized = webContent.resetSkSurface(newSize.contentWidth(), newSize.contentHeight());
        }
      }
    };
    useSceneWithCallback(resizeEntity);

    if (resized == true)
      sizeDidChange();
    return resized;
  }

  bool LayoutObject::mutateAccumulatedFragment(const Fragment &f) const
  {
    if (TR_UNLIKELY(!accumulated_fragment_.has_value()))
    {
      accumulated_fragment_ = f;
      return true;
    }

    bool isChanged = accumulated_fragment_.value() != f;
    accumulated_fragment_ = f;
    return isChanged;
  }

  bool LayoutObject::computeLayout(const ConstraintSpace &avilableSpace)
  {
    if (TR_UNLIKELY(formattingContext_ == nullptr))
      return false;

    unique_ptr<const LayoutResult> result = formattingContext_->computeLayout(avilableSpace);
    if (TR_UNLIKELY(result == nullptr))
      return false;

    if (result->status() == LayoutResult::kRelayoutRequired)
      return computeLayout(avilableSpace);
    return result->status() == LayoutResult::kSuccess;
  }

  void LayoutObject::debugPrintFormattingContext() const
  {
    if (formattingContext_ != nullptr)
      formattingContext_->debugPrint();
  }

  shared_ptr<LayoutObject> LayoutObject::container() const
  {
    if (isTextOrSVGChild())
      return parent();

    if (styleRef().hasProperty("position"))
    {
      auto position = styleRef().getPropertyValue("position");
      if (position == "fixed")
        return containerForFixedPosition();
      else if (position == "absolute")
        return containerForAbsolutePosition();
    }

    // Returns the parent directly by default.
    return parent();
  }

  shared_ptr<LayoutObject> LayoutObject::containerForFixedPosition() const
  {
    // TODO: implement this method.
    return nullptr;
  }

  shared_ptr<LayoutObject> LayoutObject::containerForAbsolutePosition() const
  {
    // TODO: implement this method.
    return nullptr;
  }

  bool LayoutObject::computeIsFixedContainer(const client_cssom::CSSStyleDeclaration &style) const
  {
    // TODO: implement this method.
    return false;
  }

  bool LayoutObject::computeIsAbsoluteContainer(const client_cssom::CSSStyleDeclaration &style) const
  {
    // TODO: implement this method.
    return false;
  }

  shared_ptr<LayoutBlock> LayoutObject::containingBlock() const
  {
    if (!isTextOrSVGChild())
    {
      if (styleRef().hasProperty("position"))
      {
        auto position = styleRef().getPropertyValue("position");
        if (position == "fixed")
          return containingBlockForFixedPosition();
        else if (position == "absolute")
          return containingBlockForAbsolutePosition();
      }
    }

    shared_ptr<LayoutObject> object;
    object = parent();

    while (object != nullptr && ((object->isInline() && !object->isAtomicInlineLevel()) ||
                                 !object->isLayoutBlock()))
    {
      object = object->parent();
    }
    return dynamic_pointer_cast<LayoutBlock>(object);
  }

  shared_ptr<LayoutBlock> LayoutObject::containingBlockForFixedPosition() const
  {
    // TODO: implement this method.
    return nullptr;
  }

  shared_ptr<LayoutBlock> LayoutObject::containingBlockForAbsolutePosition() const
  {
    // TODO: implement this method.
    return nullptr;
  }

  shared_ptr<const LayoutBlock> LayoutObject::containingScrollContainer() const
  {
    auto object = parent();
    while (object != nullptr)
    {
      if (object->isScrollContainer())
        return dynamic_pointer_cast<const LayoutBlock>(object);
      object = object->parent();
    }
    return nullptr;
  }

  bool LayoutObject::visibleToHitTestRequest(const HitTestRequest &request) const
  {
    auto &style = styleRef();
    return style.getPropertyValue("visibility", "visible") == "visible" &&
           (request.ignorePointerEventsNone() ||
            style.getPropertyValue("pointer-events") != "none");
  }

  bool LayoutObject::visibleToHitTesting() const
  {
    auto &style = styleRef();
    return style.getPropertyValue("visibility", "visible") == "visible" &&
           style.getPropertyValue("pointer-events") != "none";
  }

  bool LayoutObject::hitTestAllPhases(HitTestResult &result,
                                      const HitTestRay &hitTestRay,
                                      const glm::vec3 &accumulatedOffset)
  {
    if (nodeAtPoint(result, hitTestRay, accumulatedOffset,
                    HitTestPhase::kForeground))
    {
      return true;
    }
    if (nodeAtPoint(result, hitTestRay, accumulatedOffset,
                    HitTestPhase::kFloat))
    {
      return true;
    }
    if (nodeAtPoint(result, hitTestRay, accumulatedOffset,
                    HitTestPhase::kDescendantBlockBackgrounds))
    {
      return true;
    }
    if (nodeAtPoint(result, hitTestRay, accumulatedOffset,
                    HitTestPhase::kSelfBlockBackground))
    {
      return true;
    }
    return false;
  }

  shared_ptr<dom::Node> LayoutObject::nodeForHitTest() const
  {
    if (node() != nullptr)
      return node();

    if (parent() != nullptr)
    {
      // TODO: check if the parent is a layout object.
    }
    return nullptr;
  }

  void LayoutObject::updateHitTestResult(HitTestResult &result, const glm::vec3 &point) const
  {
    if (result.innerNode())
      return;

    if (auto n = nodeForHitTest())
      result.setNodeAndPosition(n, point);
  }

  void LayoutObject::entityDidCreate(builtin_scene::ecs::EntityId entity)
  {
    auto configEntity = [this, &entity](Scene &scene)
    {
      if (node()->enableCustomGeometry())
      {
        // TODO: Support the mesh element rendering, add Mesh3d, MeshMaterial3d, etc.
        assert(false && "The mesh element rendering is not supported yet.");
      }
      else
      {
        auto webContextCtx = scene.getResource<WebContentContext>();
        assert(webContextCtx != nullptr && "The web content context must be set.");
        scene.getComponentChecked<Mesh3d>(webContextCtx->instancedMeshEntity())
            .getHandleCheckedAsRef<InstancedMeshBase>()
            .addInstance(entity); // Add the entity to the instanced mesh.

        // Add `WebContent` component to the entity.
        auto fragment = this->fragment();
        scene.addComponent(entity, WebContent(string(this->debugName()),
                                              fragment.contentWidth(),
                                              fragment.contentHeight()));
      }
    };
    useSceneWithCallback(configEntity);
  }

  void LayoutObject::entityWillBeDestroyed(builtin_scene::ecs::EntityId entity)
  {
    auto removeInstance = [this, &entity](Scene &scene)
    {
      if (!node()->enableCustomGeometry())
      {
        auto webContextCtx = scene.getResource<WebContentContext>();
        assert(webContextCtx != nullptr);
        scene.getComponentChecked<Mesh3d>(webContextCtx->instancedMeshEntity())
            .getHandleCheckedAsRef<InstancedMeshBase>()
            .removeInstance(entity); // Remove the entity from the instanced mesh.
      }
    };
    useSceneWithCallback(removeInstance);
  }

  void LayoutObject::formattingContextWillSet(DisplayType)
  {
  }

  void LayoutObject::formattingContextDidSet(FormattingContext &)
  {
  }

  void LayoutObject::styleWillChange(client_cssom::CSSStyleDeclaration &newStyle)
  {
    // Preprocess the length properties to convert the viewport-based relative length to pixels.
    // Such as "width: 50vw", "height: 50vh", etc.
    glm::vec3 viewport = viewRef().viewport.xyz();
#define PREPROCESS_LENGTH(NAME)                                                   \
  if (newStyle.hasProperty(NAME))                                                 \
  {                                                                               \
    auto length = newStyle.getPropertyValueAs<client_cssom::types::Length>(NAME); \
    if (length.isViewportBasedRelativeLength())                                   \
    {                                                                             \
      auto lengthInPixels = length.computeViewportBasedLengthInPixels(viewport);  \
      newStyle.setProperty(NAME, to_string(lengthInPixels) + "px");               \
    }                                                                             \
  }
    PREPROCESS_LENGTH("width")
    PREPROCESS_LENGTH("height")
    PREPROCESS_LENGTH("min-width")
    PREPROCESS_LENGTH("min-height")
    PREPROCESS_LENGTH("max-width")
    PREPROCESS_LENGTH("max-height")

#undef PREPROCESS_LENGTH

    // Update the transform's post-transform matrix if the "transform" property is provided.
    if (newStyle.hasProperty("transform"))
    {
      auto transformComponent = getSceneComponent<Transform>();
      if (transformComponent != nullptr)
      {
        auto &postTransform = transformComponent->getOrInitPostTransform();
        // TODO: how to avoid duplicated parsing?
        auto transformProperty = types::transform::Transform::Parse(newStyle.getPropertyValue("transform"));
        if (transformProperty.size() > 0)
        {
          glm::mat4 mat(1.0f);
          if (transformProperty.applyMatrixTo(mat) > 0)
            postTransform.setMatrix(mat);
        }
      }
    }
  }

  void LayoutObject::styleDidChange()
  {
  }

  void LayoutObject::sizeWillChange(const Fragment &newSize)
  {
  }

  void LayoutObject::sizeDidChange()
  {
  }

  void LayoutObject::willComputeLayout(const ConstraintSpace &avilableSpace)
  {
  }

  void LayoutObject::didComputeLayoutOnce(const ConstraintSpace &avilableSpace)
  {
  }
}
