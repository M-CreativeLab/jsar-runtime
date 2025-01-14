#include <skia/include/core/SkImageInfo.h>
#include <client/builtin_scene/ecs-inl.hpp>
#include <client/builtin_scene/web_content.hpp>
#include <client/builtin_scene/materials/web_content.hpp>
#include "./html_div_element.hpp"

namespace dom
{
  void HTMLContentElement::connectedCallback()
  {
    HTMLPlaneElement::connectedCallback();

    // Implement connectedCallback for HTMLContentElement
    assert(entity_.has_value());
    {
      using namespace builtin_scene;

      auto resetMaterial = [this](Scene &scene)
      {
        auto materials = scene.getResource<Materials>();
        scene.replaceComponent(entity_.value(),
                               MeshMaterial3d(materials->add(
                                   Material::Make<materials::WebContentMaterial>())));
      };
      useScene(resetMaterial);

      auto initCanvas = [this](Scene &scene)
      {
        auto surface = createOrGetSurface();
        SkCanvas *canvas = surface != nullptr ? surface->getCanvas() : nullptr;
        scene.addComponent(entity_.value(), WebContent(canvas, adoptedStyle_));
      };
      useScene(initCanvas);
    }
  }

  void HTMLContentElement::onLayoutSizeChanged()
  {
    HTMLPlaneElement::onLayoutSizeChanged();

    // Implement onLayoutSizeChanged for HTMLContentElement
    assert(entity_.has_value());
    auto surface = createOrGetSurface();
    if (surface != nullptr)
    {
      using namespace builtin_scene;

      // Resize the surface for the content.
      auto resizeCanvas = [this, surface](Scene &scene)
      {
        auto content = scene.getComponent<WebContent>(entity_.value());
        assert(content != nullptr);
        if (TR_LIKELY(surface != nullptr))
        {
          content->setCanvas(surface->getCanvas());

          auto meshMaterial3d = scene.getComponent<MeshMaterial3d>(entity_.value());
          if (TR_UNLIKELY(meshMaterial3d == nullptr))
            return;
          auto material = meshMaterial3d->material<materials::WebContentMaterial>();
          if (TR_LIKELY(material != nullptr))
            material->updateTexture(*content); // Resize the material with the resized content.
        }
      };
      useScene(resizeCanvas);
    }
  }

  void HTMLContentElement::onAdoptedStyleChanged()
  {
    HTMLPlaneElement::onLayoutSizeChanged();

    // Implement onAdoptedStyleChanged for HTMLContentElement
    assert(entity_.has_value());
    auto updateStyle = [this](builtin_scene::Scene &scene)
    {
      using namespace builtin_scene;
      auto content = scene.getComponent<WebContent>(entity_.value());
      assert(content != nullptr);
      content->setStyle(adoptedStyle_);
    };
    useScene(updateStyle);
  }

  sk_sp<SkSurface> HTMLContentElement::createOrGetSurface()
  {
    auto width = offsetWidth();
    auto height = offsetHeight();
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
