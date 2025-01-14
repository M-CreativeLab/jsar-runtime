#include <assert.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkPaint.h>
#include <skia/include/core/SkRRect.h>
#include <skia/include/core/SkRect.h>
#include <skia/include/core/SkColor.h>

#include "./mesh_material.hpp"
#include "./materials/web_content.hpp"
#include "./web_content.hpp"
#include "./text.hpp"

namespace builtin_scene::web_renderer
{
  using namespace skia::textlayout;

  void RenderBaseSystem::onExecute()
  {
    auto entities = queryEntities<WebContent>();
    if (entities.size() == 0)
      return;

    for (auto &entity : entities)
    {
      auto content = getComponent<WebContent>(entity);
      assert(content != nullptr);
      render(entity, *content);
    }
  }

  void RenderBackgroundSystem::render(ecs::EntityId entity, WebContent &content)
  {
    const auto &style = content.style();
    auto canvas = content.canvas();
    float top = 5.0f;
    float left = 5.0f;

    SkRect rect = SkRect::MakeXYWH(left, top,
                                   content.width() - 2 * left,
                                   content.height() - 2 * top);
    SkRRect roundedRect;
    {
      // Set the radius for all four corners.
      float borderTopLeftRadius = style.getPropertyValueAs<float>("border-top-left-radius");
      float borderTopRightRadius = style.getPropertyValueAs<float>("border-top-right-radius");
      float borderBottomRightRadius = style.getPropertyValueAs<float>("border-bottom-right-radius");
      float borderBottomLeftRadius = style.getPropertyValueAs<float>("border-bottom-left-radius");
      SkVector radii[4] = {
          {borderTopLeftRadius, borderTopLeftRadius},
          {borderTopRightRadius, borderTopRightRadius},
          {borderBottomRightRadius, borderBottomRightRadius},
          {borderBottomLeftRadius, borderBottomLeftRadius}};
      roundedRect.setRectRadii(rect, radii);
    }

    if (style.hasProperty("background-color"))
    {
      SkPaint fillPaint;
      fillPaint.setColor(style.getPropertyValueAs<client_cssom::Color>("background-color"));
      fillPaint.setAntiAlias(true);
      fillPaint.setStyle(SkPaint::kFill_Style);
      canvas->drawRRect(roundedRect, fillPaint);
    }

    SkPaint borderPaint;
    {
      borderPaint.setAntiAlias(true);
      borderPaint.setStyle(SkPaint::kStroke_Style);

      // Support for border color, border-width, border-style for top, right, bottom, left.
      if (style.hasProperty("border-top-color"))
      {
        borderPaint.setColor(style.getPropertyValueAs<client_cssom::Color>("border-top-color"));
        borderPaint.setStrokeWidth(style.getPropertyValueAs<float>("border-top-width"));
        canvas->drawLine(rect.fLeft, rect.fTop, rect.fRight, rect.fTop, borderPaint);
      }
      if (style.hasProperty("border-right-color"))
      {
        borderPaint.setColor(style.getPropertyValueAs<client_cssom::Color>("border-right-color"));
        borderPaint.setStrokeWidth(style.getPropertyValueAs<float>("border-right-width"));
        canvas->drawLine(rect.fRight, rect.fTop, rect.fRight, rect.fBottom, borderPaint);
      }
      if (style.hasProperty("border-bottom-color"))
      {
        borderPaint.setColor(style.getPropertyValueAs<client_cssom::Color>("border-bottom-color"));
        borderPaint.setStrokeWidth(style.getPropertyValueAs<float>("border-bottom-width"));
        canvas->drawLine(rect.fRight, rect.fBottom, rect.fLeft, rect.fBottom, borderPaint);
      }
      if (style.hasProperty("border-left-color"))
      {
        borderPaint.setColor(style.getPropertyValueAs<client_cssom::Color>("border-left-color"));
        borderPaint.setStrokeWidth(style.getPropertyValueAs<float>("border-left-width"));
        canvas->drawLine(rect.fLeft, rect.fBottom, rect.fLeft, rect.fTop, borderPaint);
      }
    }
  }

  void RenderTextSystem::render(ecs::EntityId entity, WebContent &content)
  {
    // auto fontCollection = sk_make_sp<FontCollection>();
    // auto paragraphBuilder = ParagraphBuilder::make(content.paragraphStyle, fontCollection);

    // paragraphBuilder->pushStyle(content.textStyle);
    // paragraphBuilder->addText("Hello, world!");
    // paragraphBuilder->pop();

    // auto paragraph = paragraphBuilder->Build();
    // paragraph->layout(300.0f);
    // paragraph->paint(content.canvas(), 0.0f, 0.0f);
  }

  void UpdateTextureSystem::render(ecs::EntityId entity, WebContent &content)
  {
    auto material3d = getComponent<MeshMaterial3d>(entity);
    if (material3d == nullptr)
      return;

    auto webContentMaterial = material3d->material<materials::WebContentMaterial>();
    if (webContentMaterial != nullptr)
      webContentMaterial->updateTexture(content);
  }
}
