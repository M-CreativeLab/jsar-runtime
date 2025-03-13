#include <assert.h>
#include <client/cssom/units.hpp>
#include <skia/include/core/SkImageInfo.h>

#include "./content2d.hpp"
#include "./document.hpp"
#include "./html_element.hpp"
#include "./scene_object.hpp"

namespace dom
{
  using namespace std;
  using namespace builtin_scene;
  using LayoutNode = crates::layout2::Node;

  Content2d::Content2d(shared_ptr<Node> sceneObjectNode)
      : sceneObject_(dynamic_pointer_cast<SceneObject>(sceneObjectNode)),
        devicePixelRatio_(client_cssom::DevicePixelRatio)
  {
    assert(sceneObject_ != nullptr && "The node must be a `SceneObject`.");
  }

  Content2d::Content2d(Content2d &that)
      : sceneObject_(that.sceneObject_),
        contentSurface_(that.contentSurface_)
  {
  }

  void Content2d::onNodeConnected()
  {
    assert(sceneObject_ != nullptr && "The scene object must be valid.");

    // Append the content2d's entity to the WebContent's instanced mesh
    auto addToInstancedMesh = [this](Scene &scene)
    {
      auto webContextCtx = scene.getResource<WebContentContext>();
      assert(webContextCtx != nullptr);
      scene.getComponentChecked<Mesh3d>(webContextCtx->instancedMeshEntity())
          .getHandleCheckedAsRef<InstancedMeshBase>()
          .addInstance(entity());
    };
    sceneObject_->useScene(addToInstancedMesh);

    // Reset the material to `WebContentMaterial`.
    auto resetMaterial = [this](Scene &scene)
    {
      auto id = entity();
      scene.removeComponent<Mesh3d>(id, true);
      scene.removeComponent<MeshMaterial3d>(id, true);
    };
    sceneObject_->useScene(resetMaterial);

    // Initialize the canvas for the content.
    auto initCanvas = [this](Scene &scene)
    {
      auto surface = createOrGetSurface();
      SkCanvas *canvas = surface != nullptr ? surface->getCanvas() : nullptr;
      scene.addComponent(entity(), WebContent(canvas,
                                              sceneObject_->name_,
                                              sceneObject_->adoptedStyle_));
    };
    sceneObject_->useScene(initCanvas);
  }

  void Content2d::onNodeDisconnected()
  {
    auto removeFromInstancedMesh = [this](Scene &scene)
    {
      auto webContextCtx = scene.getResource<WebContentContext>();
      assert(webContextCtx != nullptr);
      scene.getComponentChecked<Mesh3d>(webContextCtx->instancedMeshEntity())
          .getHandleCheckedAsRef<InstancedMeshBase>()
          .removeInstance(entity());
    };
    sceneObject_->useScene(removeFromInstancedMesh);

    auto removeWebContent = [this](Scene &scene)
    {
      scene.removeComponent<WebContent>(entity());
    };
    sceneObject_->useScene(removeWebContent);
  }

  void Content2d::onLayoutSizeChanged()
  {
    auto surface = createOrGetSurface();
    if (surface != nullptr)
    {
      assert(sceneObject_->entity_.has_value());
      auto &entity = sceneObject_->entity_.value();

      // Resize the surface for the content.
      auto resizeCanvas = [this, entity, surface](Scene &scene)
      {
        auto content = scene.getComponent<WebContent>(entity);
        assert(content != nullptr);
        assert(surface != nullptr);

        SkCanvas *canvas = surface->getCanvas();
        if (TR_LIKELY(canvas != nullptr))
        {
          if (devicePixelRatio_ != 1.0f)
            canvas->scale(devicePixelRatio_, devicePixelRatio_);
          content->setCanvas(canvas);
        }
      };
      sceneObject_->useScene(resizeCanvas);
    }
  }

  void Content2d::onAdoptedStyleChanged()
  {
    auto updateStyle = [this](Scene &scene)
    {
      auto webContent = scene.getComponent<WebContent>(entity());
      assert(webContent != nullptr);
      webContent->setStyle(sceneObject_->adoptedStyle_);
    };
    sceneObject_->useScene(updateStyle);
  }

  const builtin_scene::ecs::EntityId &Content2d::entity() const
  {
    assert(sceneObject_->entity_.has_value());
    return sceneObject_->entity_.value();
  }

  std::shared_ptr<builtin_scene::Scene> Content2d::scene()
  {
    auto sceneRef = sceneObject_->scene_.lock();
    assert(sceneRef != nullptr && "The scene must be valid.");
    return sceneRef;
  }

  sk_sp<SkSurface> Content2d::createOrGetSurface()
  {
    auto width = sceneObject_->offsetWidth();
    auto height = sceneObject_->offsetHeight();
    if (width <= 0 || height <= 0) // Skip if the size is invalid.
      return nullptr;

    // TODO: use Skia Genesh(GPU) to increase the performance.
    SkImageInfo imageInfo = SkImageInfo::MakeN32Premul(width * devicePixelRatio_,
                                                       height * devicePixelRatio_);
    if (contentSurface_ == nullptr)
    {
      contentSurface_ = SkSurfaces::Raster(imageInfo);
    }
    else
    {
      auto newSurface = contentSurface_->makeSurface(imageInfo);
      contentSurface_.reset();
      contentSurface_ = newSurface;
    }
    return contentSurface_;
  }
}
