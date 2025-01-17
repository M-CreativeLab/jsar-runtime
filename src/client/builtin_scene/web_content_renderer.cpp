#include <optional>
#include <assert.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkPaint.h>
#include <skia/include/core/SkRRect.h>
#include <skia/include/core/SkRect.h>
#include <skia/include/core/SkColor.h>
#include <skia/include/core/SkPathEffect.h>
#include <skia/include/effects/SkDashPathEffect.h>

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
      if (content->canvas() == nullptr) // Skip rendering if the canvas is not initialized.
        continue;
      if (content->isDirty()) // Skip rendering if the content is not dirty.
        render(entity, *content);
    }
  }

  void drawBackground(SkCanvas *canvas, SkRRect &originalRRect,
                      const crates::layout::Layout &layout,
                      const client_cssom::CSSStyleDeclaration &style)
  {
    if (style.hasProperty("background-color"))
    {
      SkPaint fillPaint;
      fillPaint.setColor(style.getPropertyValueAs<client_cssom::types::Color>("background-color"));
      fillPaint.setAntiAlias(true);
      fillPaint.setStyle(SkPaint::kFill_Style);
      {
        const SkRect &originalRect = originalRRect.rect();
        float insetTop = layout.border().top();
        float insetRight = layout.border().right();
        float insetBottom = layout.border().bottom();
        float insetLeft = layout.border().left();

        SkRect rect = SkRect::MakeXYWH(originalRect.fLeft + insetLeft,
                                       originalRect.fTop + insetTop,
                                       originalRect.width() - insetLeft - insetRight,
                                       originalRect.height() - insetTop - insetBottom);
        SkRRect roundedRect;
        SkVector radii[4];
        for (int i = 0; i < 4; i++)
          radii[i] = originalRRect.radii(static_cast<SkRRect::Corner>(i));
        roundedRect.setRectRadii(rect, radii);
        canvas->drawRRect(roundedRect, fillPaint);
      }
    }
  }

  void setBorderPaintEffect(SkPaint &paint, std::optional<client_cssom::types::BorderStyleKeyword> borderStyle,
                            float strokeWidth)
  {
    if (!borderStyle.has_value())
    {
      paint.setStrokeWidth(strokeWidth);
      return;
    }
    auto keyword = borderStyle.value();
    if (keyword == client_cssom::types::BorderStyleKeyword::kNone)
    {
      paint.setStrokeWidth(0);
      return;
    }
    if (keyword == client_cssom::types::BorderStyleKeyword::kSolid)
    {
      paint.setStrokeWidth(strokeWidth);
      return;
    }
    if (keyword == client_cssom::types::BorderStyleKeyword::kDashed)
    {
      paint.setStrokeWidth(strokeWidth);

      const SkScalar intervals[] = {10, 5};
      paint.setPathEffect(SkDashPathEffect::Make(intervals, 2, 0));
      return;
    }
    if (keyword == client_cssom::types::BorderStyleKeyword::kDotted)
    {
      paint.setStrokeWidth(strokeWidth);

      const SkScalar intervals[] = {2, 5};
      paint.setPathEffect(SkDashPathEffect::Make(intervals, 2, 0));
      return;
    }

    {
      // Default case
      paint.setStrokeWidth(strokeWidth);
    }
  }

  void drawBorders(SkCanvas *canvas, SkRRect &roundedRect,
                   const crates::layout::Layout &layout,
                   const client_cssom::CSSStyleDeclaration &style)
  {
    using namespace client_cssom::types;

    SkPaint paint;
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setAntiAlias(true);

    if (style.hasProperty("border-top-width"))
    {
      auto borderWidth = layout.border().top();
      auto borderColor = style.getPropertyValueAs<Color>("border-top-color");
      float halfBorderWidth = borderWidth / 2.0f;

      paint.setColor(borderColor);
      setBorderPaintEffect(paint, style.getPropertyValueAs<BorderStyleKeyword>("border-top-style"),
                           halfBorderWidth * 2);

      SkPath path;
      const SkRect &rect = roundedRect.rect();
      // Draw the left top corner
      path.arcTo(SkRect::MakeXYWH(rect.fLeft + halfBorderWidth,
                                  rect.fTop + halfBorderWidth,
                                  roundedRect.radii(SkRRect::kUpperLeft_Corner).x() * 2,
                                  roundedRect.radii(SkRRect::kUpperLeft_Corner).y() * 2),
                 180.0f, 90.0f, false);
      path.lineTo(rect.fRight - roundedRect.radii(SkRRect::kUpperRight_Corner).x() - halfBorderWidth,
                  rect.fTop + halfBorderWidth);
      path.arcTo(SkRect::MakeXYWH(rect.fRight - roundedRect.radii(SkRRect::kUpperRight_Corner).x() * 2 - halfBorderWidth,
                                  rect.fTop + halfBorderWidth,
                                  roundedRect.radii(SkRRect::kUpperRight_Corner).x() * 2,
                                  roundedRect.radii(SkRRect::kUpperRight_Corner).y() * 2),
                 270.0f, 90.0f, false);
      canvas->drawPath(path, paint);
    }
    if (style.hasProperty("border-right-width"))
    {
      auto borderWidth = layout.border().right();
      auto borderColor = style.getPropertyValueAs<Color>("border-right-color");
      float halfBorderWidth = borderWidth / 2.0f;

      paint.setColor(borderColor);
      setBorderPaintEffect(paint, style.getPropertyValueAs<BorderStyleKeyword>("border-right-style"),
                           halfBorderWidth * 2);

      SkPath path;
      const SkRect &rect = roundedRect.rect();
      path.moveTo(rect.fRight - halfBorderWidth,
                  rect.fTop + roundedRect.radii(SkRRect::kUpperRight_Corner).y() + halfBorderWidth);
      path.lineTo(rect.fRight - halfBorderWidth,
                  rect.fBottom - roundedRect.radii(SkRRect::kLowerRight_Corner).y() - halfBorderWidth);
      canvas->drawPath(path, paint);
    }
    if (style.hasProperty("border-bottom-width"))
    {
      auto borderWidth = layout.border().bottom();
      auto borderColor = style.getPropertyValueAs<client_cssom::types::Color>("border-bottom-color");
      float halfBorderWidth = borderWidth / 2.0f;

      paint.setColor(borderColor);
      setBorderPaintEffect(paint, style.getPropertyValueAs<BorderStyleKeyword>("border-bottom-style"),
                           halfBorderWidth * 2);

      SkPath path;
      const SkRect &rect = roundedRect.rect();
      path.arcTo(SkRect::MakeXYWH(rect.fRight - roundedRect.radii(SkRRect::kLowerRight_Corner).x() * 2 - halfBorderWidth,
                                  rect.fBottom - roundedRect.radii(SkRRect::kLowerRight_Corner).y() * 2 - halfBorderWidth,
                                  roundedRect.radii(SkRRect::kLowerRight_Corner).x() * 2,
                                  roundedRect.radii(SkRRect::kLowerRight_Corner).y() * 2),
                 0.0f, 90.0f, false);
      path.lineTo(rect.fLeft + roundedRect.radii(SkRRect::kLowerLeft_Corner).x() + halfBorderWidth,
                  rect.fBottom - halfBorderWidth);
      path.arcTo(SkRect::MakeXYWH(rect.fLeft + halfBorderWidth,
                                  rect.fBottom - roundedRect.radii(SkRRect::kLowerLeft_Corner).y() * 2 - halfBorderWidth,
                                  roundedRect.radii(SkRRect::kLowerLeft_Corner).x() * 2,
                                  roundedRect.radii(SkRRect::kLowerLeft_Corner).y() * 2),
                 90.0f, 90.0f, false);
      canvas->drawPath(path, paint);
    }
    if (style.hasProperty("border-left-width"))
    {
      auto borderWidth = layout.border().left();
      auto borderColor = style.getPropertyValueAs<client_cssom::types::Color>("border-left-color");
      float halfBorderWidth = borderWidth / 2.0f;

      paint.setColor(borderColor);
      setBorderPaintEffect(paint, style.getPropertyValueAs<BorderStyleKeyword>("border-left-style"),
                           halfBorderWidth * 2);

      SkPath path;
      const SkRect &rect = roundedRect.rect();
      path.moveTo(rect.fLeft + halfBorderWidth,
                  rect.fBottom - roundedRect.radii(SkRRect::kLowerLeft_Corner).y() - halfBorderWidth);
      path.lineTo(rect.fLeft + halfBorderWidth,
                  rect.fTop + roundedRect.radii(SkRRect::kUpperLeft_Corner).y() + halfBorderWidth);
      canvas->drawPath(path, paint);
    }
  }

  void RenderBackgroundSystem::render(ecs::EntityId entity, WebContent &content)
  {
    const auto &style = content.style();
    const auto &layout = content.layout();
    if (!layout.has_value()) // No layout, no rendering.
      return;

    auto canvas = content.canvas();
    float top = 0.0f;
    float left = 0.0f;

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

    drawBackground(canvas, roundedRect, layout.value(), style);
    drawBorders(canvas, roundedRect, layout.value(), style);
  }

  RenderTextSystem::RenderTextSystem()
      : RenderBaseSystem(),
        clientContext_(TrClientContextPerProcess::Get()),
        fontCollection_(clientContext_->getFontCacheManager()),
        paragraphBuilder_(nullptr)
  {
  }

  void RenderTextSystem::render(ecs::EntityId entity, WebContent &content)
  {
    auto textComponent = getComponent<Text2d>(entity);
    if (textComponent == nullptr)
      return;

    const auto &layout = content.layout();
    if (!layout.has_value()) // No layout, no rendering.
      return;

    string &text = textComponent->content;
    auto paragraphBuilder = ParagraphBuilder::make(content.paragraphStyle, fontCollection_);
    paragraphBuilder->pushStyle(content.textStyle);
    paragraphBuilder->addText(text.c_str(), text.size());
    paragraphBuilder->pop();

    auto containerWidth = layout.value().width();
    auto paragraph = paragraphBuilder->Build();
    paragraph->layout(containerWidth);
    paragraph->paint(content.canvas(), 0.0f, 0.0f);
  }

  void UpdateTextureSystem::render(ecs::EntityId entity, WebContent &content)
  {
    auto material3d = getComponent<MeshMaterial3d>(entity);
    if (material3d == nullptr)
      return;

    auto webContentMaterial = material3d->material<materials::WebContentMaterial>();
    if (webContentMaterial != nullptr && webContentMaterial->updateTexture(content))
      content.setDirty(false); // Mark the content as clean if the texture is updated successfully.
  }
}
