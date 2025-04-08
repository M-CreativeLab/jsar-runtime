#include <client/cssom/types/transform.hpp>
#include <client/dom/node.hpp>
#include <client/dom/all_html_elements.hpp>
#include <client/dom/document-inl.hpp>

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
    if (isLayoutView())
      return dynamic_pointer_cast<LayoutView>(shared_from_this());
    else
      return document()->layoutView();
  }

  LayoutView &LayoutObject::viewRef()
  {
    return document()->layoutViewRef();
  }

  shared_ptr<const LayoutView> LayoutObject::view() const
  {
    return document()->layoutView();
  }

  const LayoutView &LayoutObject::viewRef() const
  {
    return document()->layoutViewRef();
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
    entityDidCreated(entity_.value());
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

  const Fragment LayoutObject::fragment() const
  {
    assert(formattingContext_ != nullptr && "Formatting context must be set.");
    if (parent() == nullptr)
      return formattingContext_->liveFragment();

    Fragment baseFragment = parent()->fragment();
    return baseFragment.position(formattingContext_->liveFragment());
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

  void LayoutObject::onChildRemoved(shared_ptr<LayoutObject> oldChild)
  {
    oldChild->formattingContext_->onRemoved(*formattingContext_);
  }

  void LayoutObject::onChildReplaced(shared_ptr<LayoutObject> newChild, shared_ptr<LayoutObject> oldChild)
  {
    newChild->formattingContext_->onReplaced(*formattingContext_,
                                             *oldChild->formattingContext_);
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

  void LayoutObject::replaceChild(shared_ptr<LayoutObject> newChild, shared_ptr<LayoutObject> oldChild)
  {
    auto children = virtualChildren();
    if (!children)
      return;

    auto replaced = children->replaceChildNode(shared_from_this(), newChild, oldChild);
    assert(replaced == oldChild && "Failed to replace the old child with the new child.");
    onChildReplaced(newChild, oldChild);
  }

  void LayoutObject::setFormattingContext(DisplayType display)
  {
    formattingContext_ = FormattingContext::Make(display, view());
  }

  bool LayoutObject::setStyle(const CSSStyleDeclaration &style)
  {
    // Update the transform's post-transform matrix if the "transform" property is provided.
    if (style.hasProperty("transform"))
    {
      auto transformComponent = getSceneComponent<Transform>();
      if (transformComponent != nullptr)
      {
        auto &postTransform = transformComponent->getOrInitPostTransform();
        // TODO: how to avoid duplicated parsing?
        auto transformProperty = types::transform::Transform::Parse(style.getPropertyValue("transform"));
        if (transformProperty.size() > 0)
        {
          glm::mat4 mat(1.0f);
          if (transformProperty.applyMatrixTo(mat) > 0)
            postTransform.setMatrix(mat);
        }
      }
    }

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
    return formattingContext_ == nullptr
               ? false
               : formattingContext_->setLayoutStyle(style);
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
          resized = webContent.resetSkSurface(newSize.width(), newSize.height());
        }
      }
    };
    useSceneWithCallback(resizeEntity);

    if (resized == true)
      sizeDidChanged();
    return resized;
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

    if (style_->hasProperty("position"))
    {
      auto position = style_->getPropertyValue("position");
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
      if (style_->hasProperty("position"))
      {
        auto position = style_->getPropertyValue("position");
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

  void LayoutObject::entityDidCreated(builtin_scene::ecs::EntityId entity)
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
        scene.addComponent(entity, WebContent(string(this->name()),
                                              fragment.width(),
                                              fragment.height()));
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

  void LayoutObject::styleWillChange(const client_cssom::CSSStyleDeclaration &newStyle)
  {
  }

  void LayoutObject::styleDidChanged(const client_cssom::CSSStyleDeclaration &oldStyle)
  {
  }

  void LayoutObject::sizeWillChange(const Fragment &newSize)
  {
  }

  void LayoutObject::sizeDidChanged()
  {
  }

  void LayoutObject::willComputeLayout(const ConstraintSpace &avilableSpace)
  {
  }

  void LayoutObject::didComputeLayoutOnce(const ConstraintSpace &avilableSpace)
  {
  }
}
