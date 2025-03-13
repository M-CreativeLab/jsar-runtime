#include <client/cssom/units.hpp>
#include <client/cssom/types/transform.hpp>

#include "./scene_object.hpp"
#include "./document.hpp"
#include "./text.hpp"
#include "./html_image_element.hpp"

namespace dom
{
  using namespace std;
  using namespace builtin_scene;
  using namespace crates::layout2::styles;
  using LayoutNode = crates::layout2::Node;
  using LayoutStyle = crates::layout2::LayoutStyle;

  SceneObject::SceneObject(shared_ptr<HTMLDocument> htmlDocument, string name)
      : name_(name)
  {
    if (htmlDocument != nullptr)
      initialize(*htmlDocument);

    defaultStyle_.setProperty("display", "block");
    defaultStyle_.setProperty("width", "auto");
    defaultStyle_.setProperty("height", "auto");
  }

  SceneObject::SceneObject(SceneObject &that)
      : scene_(that.scene_),
        entity_(that.entity_),
        layoutAllocator_(that.layoutAllocator_),
        layoutNode_(that.layoutNode_),
        adoptedStyle_(that.adoptedStyle_),
        defaultStyle_(that.defaultStyle_),
        name_(that.name_)
  {
  }

  std::shared_ptr<LayoutNode> SceneObject::layoutNode()
  {
    return layoutNode_;
  }

  void SceneObject::useScene(const function<void(Scene &)> &callback)
  {
    auto sceneRef = scene_.lock();
    if (sceneRef != nullptr)
      callback(*sceneRef);
  }

  bool SceneObject::render(Node &node)
  {
    auto sceneRef = scene_.lock();
    if (TR_UNLIKELY(sceneRef == nullptr) || !entity_.has_value())
      return false;

    auto meshComponent = getSceneComponent<Mesh3d>();
    if (meshComponent != nullptr)
    {
      if (skipRender())
      {
        meshComponent->disableRendering(); // Disable rendering if the object is skipped.
        return false;
      }
      meshComponent->resumeRendering(); // Resume rendering if the object is not skipped.
    }

    auto layout = fetchLayoutAndDispatchChangeEvent(node);
#ifdef TR_CLIENT_DOM_VERBOSE
    cout << "Rendering SceneObject(" << name_ << "): " << layout << endl;
#endif

    renderObject(*sceneRef, layout);
    return true;
  }

  void SceneObject::renderObject(Scene &scene, const client_cssom::Layout &layout)
  {
    if (entity_.has_value())
    {
      auto &entity = entity_.value();

      // Update the layout into the `WebContent` component.
      auto webContent = getSceneComponent<WebContent>();
      if (webContent != nullptr)
        webContent->setLayout(layout); // Update the layout result into the `WebContent` component.

      // Update bouding box
      auto boundingBox = getSceneComponent<BoundingBox>();
      if (boundingBox != nullptr)
        boundingBox->updateSize(layout.width(), layout.height(), layout.depth());

      // Update transform by the layout.
      {
        auto transform = getSceneComponent<Transform>();
        if (transform != nullptr)
        {
          transform->setScale({client_cssom::pixelToMeter(boundingBox->width()),
                               client_cssom::pixelToMeter(boundingBox->height()),
                               1.0f});

          float left = layout.left(); // Get the left position.
          float top = layout.top();   // Get the top position.
          auto isRootEntity = hasSceneComponent<hierarchy::Root>();
          if (!isRootEntity)
          {
            auto &parentComponent = getSceneComponentChecked<hierarchy::Parent>();
            auto rootBoundingBox = scene.getComponent<BoundingBox>(parentComponent.root());
            if (TR_LIKELY(rootBoundingBox != nullptr))
            {
              /**
               * Transform the xyz() in LTW space to the left-handed world space.
               *
               * First, calculate the distance from the root bounding box to the current bounding box:
               *
               * ```
               * var distance = (root - box) / 2
               * ```
               *
               * Then move the origin to the left-top-center of the root bounding box:
               *
               * ```
               * var origin = distance * (-1, 1, 1)
               * ```
               *
               * 3D space uses right(+x) and up(+y), thus if we wanna move the origin to the left and top, we need to multiply the
               * y-axis by -1 only.
               *
               * Finally, calculate the offset in world space to make the final translation:
               *
               * ```
               * var offset = origin + layout.xyz() * (1, -1, 1)
               * ```
               *
               * Note that there is a transformation are required to convert the layout space, namely right(+x) and up(-y), to the
               * world space, namely right(+x) and up(+y).
               */
              auto origin = (*rootBoundingBox - *boundingBox) / 2.0f * glm::vec3(-1.0f, 1.0f, 1.0f);
              auto offset = origin + layout.xyz() * glm::vec3(1.0f, -1.0f, 1.0f);
              left = offset.x;
              top = offset.y;
            }
          }
          transform->setTranslation({client_cssom::pixelToMeter(left),
                                     client_cssom::pixelToMeter(top),
                                     layout.depth()});
        }
      }

      auto material3d = getSceneComponent<MeshMaterial3d>();
      if (material3d != nullptr)
      {
        // TODO: update material properties?
      }
    }
  }

  void SceneObject::connectedCallback(shared_ptr<Node> node)
  {
    auto parent = node->getParentNodeAs<SceneObject>();
    if (!initialized_)
    {
      auto ownerDocument = node->getOwnerDocumentReferenceAs<HTMLDocument>(true);
      initialize(*ownerDocument);
    }

    // Create the entity
    auto createEntity = [this, &node, parent](Scene &scene)
    {
      entity_ = scene.createElement(node->nodeName, node,
                                    parent == nullptr ? nullopt : parent->entity_);
      assert(entity_.has_value());
    };
    useScene(createEntity);

    // Layout
    if (layoutAllocator_ != nullptr)
    {
      layoutNode_ = make_shared<LayoutNode>(*layoutAllocator_);
      setLayoutStyle(*node, adoptedStyle_);

      // Append this node to the parent layout node.
      if (parent != nullptr)
      {
        assert(parent->layoutNode_ != nullptr);
        parent->layoutNode_->addChild(*layoutNode_);
      }
    }
  }

  void SceneObject::disconnectedCallback()
  {
    auto removeEntity = [this](Scene &scene)
    {
      if (entity_.has_value())
      {
        scene.removeElement(entity_.value());
        entity_ = nullopt;
      }
    };
    useScene(removeEntity);

    // Destroy the layout node
    if (layoutNode_ != nullptr)
      layoutNode_.reset();
  }

  client_cssom::Layout SceneObject::fetchLayoutAndDispatchChangeEvent(Node &node)
  {
    client_cssom::Layout layout;
    auto parent = node.getParentNodeAs<SceneObject>();
    if (parent == nullptr)
    {
      // If the parent is not found or not a SceneObject, use the layout directly.
      layout = layoutNode_->layout();
    }
    else
    {
      // Otherwise, merge the layout with the parent layout.
      layout = client_cssom::Layout::Merge(parent->computedLayout_, layoutNode_->layout());
    }

    // Update the `computedLayout_` value
    computedLayout_ = layout;
    assert(computedLayout_.has_value());

    // Dispatch the `onLayoutChanged` if the layout size is changed.
    if (computedLayout_->needsResize(offsetWidth(), offsetHeight()))
    {
      offsetWidth() = layout.width();
      offsetHeight() = layout.height();
      setLayoutStyle(node, adoptedStyle_); // Update the layout style if the size is changed.
      onLayoutChanged();
    }

    // TODO: support offsetTop, offsetLeft, offsetParent, etc.
    return computedLayout_.value();
  }

  bool SceneObject::adoptStyleOn(Node &node, const client_cssom::CSSStyleDeclaration &style)
  {
    client_cssom::CSSStyleDeclaration newStyle = style;
    newStyle.update(defaultStyle_, true); // Update the default style if these properties are not present.

    if (adoptedStyle_.equals(newStyle)) // Skip if the style is the same.
      return false;

    // Update the post-transform if the transform property is provided.
    if (style.hasProperty("transform"))
    {
      auto transformComponent = getSceneComponent<Transform>();
      if (transformComponent != nullptr)
      {
        auto &postTransform = transformComponent->getOrInitPostTransform();
        auto transformProperty = client_cssom::types::transform::Transform::Parse(style.getPropertyValue("transform"));
        if (transformProperty.size() > 0)
        {
          glm::mat4 mat(1.0f);
          if (transformProperty.applyMatrixTo(mat) > 0)
            postTransform.setMatrix(mat);
        }
      }
    }

    adoptedStyle_ = newStyle;
    onAdoptedStyleChanged();

    // Update the layout node style.
    return setLayoutStyle(node, adoptedStyle_);
  }

  void SceneObject::initialize(const HTMLDocument &htmlDocument)
  {
    if (initialized_)
      return;
    scene_ = htmlDocument.scene;
    layoutAllocator_ = htmlDocument.layoutAllocator();
    assert(scene_.lock() != nullptr && "The scene must be valid.");
    assert(layoutAllocator_ != nullptr && "The layout allocator must be valid.");
    initialized_ = true;
  }

  bool SceneObject::skipRender() const
  {
    if (!adoptedStyle_.hasProperty("display"))
      return false;

    using namespace crates::layout2::styles;
    auto display = adoptedStyle_.getPropertyValueAs<Display>("display");
    return display == Display::None();
  }

  bool SceneObject::setLayoutStyle(const Node &node, const client_cssom::CSSStyleDeclaration &style)
  {
    if (layoutNode_ == nullptr)
      return false;

    LayoutStyle layoutStyle(style); // Create the layout style from the CSS style.

    // Update the layout style for Text node.
    if (node.nodeType == NodeType::TEXT_NODE)
    {
      try
      {
        auto &text = dynamic_cast<const Text &>(node);
        dom::geometry::DOMRect textRect;

        // Check if the width is auto, then calculate the width from the text.
        if (layoutStyle.width().isAuto())
        {
          textRect = text.getTextClientRect(); // Use inf if the width is auto.
          layoutStyle.setWidth(Dimension::Length(textRect.width()));
        }
        else
          textRect = text.getTextClientRect(offsetWidth());

        // Check if the height is auto, then use the calculated height.
        if (layoutStyle.height().isAuto())
          layoutStyle.setHeight(Dimension::Length(textRect.height()));
      }
      catch (const std::bad_cast &e)
      {
        // Ignore the exception.
      }
    }
    else if (node.nodeType == NodeType::ELEMENT_NODE)
    {
      if (layoutStyle.width().isAuto() && defaultBoundingBox_.width.has_value())
        layoutStyle.setWidth(Dimension::Length(defaultBoundingBox_.width.value()));
      if (layoutStyle.height().isAuto() && defaultBoundingBox_.height.has_value())
        layoutStyle.setHeight(Dimension::Length(defaultBoundingBox_.height.value()));
    }

    layoutNode_->setStyle(layoutStyle);
#ifdef TR_CLIENT_DOM_VERBOSE
    cout << "Updated layout style for SceneObject(" << name_ << "): " << layoutStyle << endl;
    cout << "source style: " << style << endl;
#endif
    return true;
  }
}
