#pragma once

#include <memory>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkCanvas.h>
#include <crates/bindings.hpp>
#include <client/cssom/css_style_declaration.hpp>
#include "./node.hpp"
#include "./scene_object.hpp"

namespace dom
{
  class Content2d
  {
    friend class RenderHTMLDocument;
    friend class HTMLContent2dElement;
    friend class Text;

  public:
    Content2d(std::shared_ptr<Node> sceneObjectNode);
    Content2d(Content2d &that);
    virtual ~Content2d() = default;

  public:
    float devicePixelRatio() const { return devicePixelRatio_; }

  protected:
    void onNodeConnected();
    void onLayoutSizeChanged();
    void onAdoptedStyleChanged();

  private:
    const builtin_scene::ecs::EntityId &entity() const;
    std::shared_ptr<builtin_scene::Scene> scene();
    sk_sp<SkSurface> createOrGetSurface();

  private:
    std::shared_ptr<SceneObject> sceneObject_;
    sk_sp<SkSurface> contentSurface_;
    float devicePixelRatio_ = 1.0f;
  };
}
