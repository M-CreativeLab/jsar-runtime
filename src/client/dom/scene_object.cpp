#include <client/cssom/units.hpp>
#include "./scene_object.hpp"
#include "./document.hpp"

namespace dom
{
  using namespace std;
  using namespace builtin_scene;
  using LayoutNode = crates::layout::Node;

  SceneObject::SceneObject(shared_ptr<HTMLDocument> htmlDocument, string name)
      : scene_(htmlDocument->scene),
        layoutAllocator_(htmlDocument->layoutAllocator()),
        name_(name)
  {
    assert(scene_.lock() != nullptr && "The scene must be valid.");
    assert(layoutAllocator_ != nullptr && "The layout allocator must be valid.");
  }

  SceneObject::SceneObject(SceneObject &that)
      : scene_(that.scene_),
        entity_(that.entity_),
        layoutAllocator_(that.layoutAllocator_),
        layoutNode_(that.layoutNode_),
        adoptedStyle_(that.adoptedStyle_),
        name_(that.name_)
  {
  }

  std::shared_ptr<crates::layout::Node> SceneObject::layoutNode()
  {
    return layoutNode_;
  }

  void SceneObject::useScene(const function<void(Scene &)> &callback)
  {
    auto sceneRef = scene_.lock();
    if (sceneRef != nullptr)
      callback(*sceneRef);
  }

  bool SceneObject::render()
  {
    auto layout = fetchLayoutAndDispatchChangeEvent();
#ifdef TR_CLIENT_DOM_VERBOSE
    cout << "Rendering SceneObject(" << name_ << "): " << layout << endl;
#endif

    auto sceneRef = scene_.lock();
    if (sceneRef != nullptr)
    {
      renderObject(*sceneRef, layout);
      return true;
    }
    else
    {
      return false;
    }
  }

  void SceneObject::renderObject(Scene &scene, const crates::layout::Layout &layout)
  {
    if (entity_.has_value())
    {
      auto &entity = entity_.value();
      {
        // Update the layout into the `WebContent` component.
        auto webContent = scene.getComponent<WebContent>(entity);
        if (webContent != nullptr)
          webContent->setLayout(layout); // Update the layout result into the `WebContent` component.
      }

      {
        // Update transform by the layout.
        auto transform = scene.getComponent<Transform>(entity);
        if (transform != nullptr)
        {
          auto actualWidth = client_cssom::pixelToMeter(layout.width());
          auto actualHeight = client_cssom::pixelToMeter(layout.height());
          transform->setScale({actualWidth, actualHeight, 1.0f});

          auto actualX = client_cssom::pixelToMeter(layout.x());
          auto actualY = client_cssom::pixelToMeter(layout.y());
          // Treat the z-index as the z-axis translation in pixels for now.
          // TODO: Support the z-index in the layout system, and convert it to meters.
          // auto actualZ = client_cssom::pixelToMeter(adoptedStyle_.getPropertyValueAs<float>("z-index"));
          transform->setTranslation({actualX, actualY, 0.0f});
        }
      }

      auto material3d = scene.getComponent<MeshMaterial3d>(entity);
      if (material3d != nullptr)
      {
        // TODO: update material properties?
      }
    }
  }

  void SceneObject::connectedCallback(const Node &node)
  {
    auto parent = node.getParentNodeAs<SceneObject>();

    // Create the entity
    auto createEntity = [this, &node, parent](Scene &scene)
    {
      entity_ = scene.createElement(node.nodeName,
                                    parent == nullptr ? nullopt : parent->entity_);
      assert(entity_.has_value());
    };
    useScene(createEntity);

    // Layout
    {
      layoutNode_ = make_shared<LayoutNode>(*layoutAllocator_);
      layoutNode_->setStyle(adoptedStyle_);

      // Append this node to the parent layout node.
      if (parent != nullptr)
      {
        assert(parent->layoutNode_ != nullptr);
        parent->layoutNode_->addChild(*layoutNode_);
      }
    }
  }

  crates::layout::Layout SceneObject::fetchLayoutAndDispatchChangeEvent()
  {
    auto layout = layoutNode_->layout();
    if (layout.width() != offsetWidth() ||
        layout.height() != offsetHeight())
    {
      offsetWidth() = layout.width();
      offsetHeight() = layout.height();
      onLayoutChanged();
    }
    // TODO: support offsetTop, offsetLeft, offsetParent, etc.
    return layout;
  }

  bool SceneObject::adoptStyleOn(Node &node, client_cssom::CSSStyleDeclaration &style)
  {
    adoptedStyle_ = style;
    onAdoptedStyleChanged();

    // Update the layout node style.
    if (layoutNode_ != nullptr)
    {
#ifdef TR_CLIENT_DOM_VERBOSE
      cout << "Adopting style for SceneObject(): " << adoptedStyle_ << endl;
#endif
      layoutNode_->setStyle(adoptedStyle_);
      return true;
    }
    else
    {
      return false;
    }
  }
}
