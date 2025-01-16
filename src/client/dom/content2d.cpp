#include <assert.h>
#include <skia/include/core/SkImageInfo.h>

#include "./content2d.hpp"
#include "./document.hpp"
#include "./html_element.hpp"
#include "./scene_object.hpp"

namespace dom
{
  using namespace std;
  using namespace builtin_scene;
  using LayoutNode = crates::layout::Node;

  Content2d::Content2d(shared_ptr<Node> sceneObjectNode)
      : sceneObject_(dynamic_pointer_cast<SceneObject>(sceneObjectNode))
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

    // Reset the material to `WebContentMaterial`.
    auto resetMaterial = [this](Scene &scene)
    {
      auto materials = scene.getResource<Materials>();
      scene.replaceComponent(entity(),
                             MeshMaterial3d(materials->add(
                                 Material::Make<materials::WebContentMaterial>())));
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

  void Content2d::onLayoutSizeChanged()
  {
    auto surface = createOrGetSurface();
    std::cout << "Resize layout " << sceneObject_->name_ << ": " << surface << std::endl;
    if (surface != nullptr)
    {
      assert(sceneObject_->entity_.has_value());
      auto &entity = sceneObject_->entity_.value();

      // Resize the surface for the content.
      auto resizeCanvas = [entity, surface](Scene &scene)
      {
        auto content = scene.getComponent<WebContent>(entity);
        assert(content != nullptr);
        if (TR_LIKELY(surface != nullptr))
        {
          content->setCanvas(surface->getCanvas());

          auto meshMaterial3d = scene.getComponent<MeshMaterial3d>(entity);
          if (TR_UNLIKELY(meshMaterial3d == nullptr))
            return;
          auto material = meshMaterial3d->material<materials::WebContentMaterial>();
          if (TR_LIKELY(material != nullptr))
            material->updateTexture(*content); // Resize the material with the resized content.
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
    SkImageInfo imageInfo = SkImageInfo::MakeN32Premul(width, height);
    if (contentSurface_ == nullptr)
      contentSurface_ = SkSurfaces::Raster(imageInfo);
    else
      contentSurface_ = contentSurface_->makeSurface(imageInfo);
    return contentSurface_;
  }
}
