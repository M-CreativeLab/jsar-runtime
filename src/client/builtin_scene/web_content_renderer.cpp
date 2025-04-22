#include <optional>
#include <assert.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkPaint.h>
#include <skia/include/core/SkRRect.h>
#include <skia/include/core/SkRect.h>
#include <skia/include/core/SkColor.h>
#include <skia/include/core/SkPathEffect.h>
#include <skia/include/effects/SkDashPathEffect.h>
#include <client/layout/fragment.hpp>

#include "./hierarchy.hpp"
#include "./transform.hpp"
#include "./instanced_mesh.hpp"
#include "./mesh_material.hpp"
#include "./meshes.hpp"
#include "./materials.hpp"
#include "./web_content.hpp"
#include "./text.hpp"
#include "./image.hpp"

namespace builtin_scene::web_renderer
{
  using namespace std;
  using namespace skia::textlayout;

  void InitSystem::onExecute()
  {
    auto webContentCtx = getResource<WebContentContext>();
    auto meshes = getResource<Meshes>();
    auto materials = getResource<Materials>();
    assert(webContentCtx != nullptr &&
           meshes != nullptr &&
           materials != nullptr);

    auto material = Material::Make<materials::WebContentInstancedMaterial>();
    webContentCtx->instancedMeshEntity_ = spawn(
        hierarchy::Root(true),
        Mesh3d(meshes->add(MeshBuilder::CreateInstancedMesh<meshes::Box>("HTMLClassicMeshes", 1.0f, 1.0f, 1.0f)),
               false),
        MeshMaterial3d(materials->add(material)),
        Transform::FromXYZ(0.0f, 0.0f, 0.0f));
  }

  void RenderBaseSystem::onExecute()
  {
    auto list = queryEntitiesWithComponent<WebContent>([](const WebContent &content) -> bool
                                                       { return content.canvas() != nullptr && content.isDirty(); });
    if (list.size() == 0)
      return;

    for (auto &item : list)
      render(item.first, *item.second);
  }

  optional<SkPaint> drawBackground(SkCanvas *canvas, SkRRect &originalRRect,
                                   const client_layout::Fragment &fragment,
                                   const client_cssom::CSSStyleDeclaration &style)
  {
    if (style.hasProperty("background-color"))
    {
      auto color = style.getPropertyValueAs<client_cssom::types::Color>("background-color");

      SkPaint fillPaint;
      fillPaint.setColor(color);
      fillPaint.setAntiAlias(true);
      fillPaint.setStyle(SkPaint::kFill_Style);
      {
        const SkRect &originalRect = originalRRect.rect();
        float insetTop = fragment.border().top();
        float insetRight = fragment.border().right();
        float insetBottom = fragment.border().bottom();
        float insetLeft = fragment.border().left();

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
      return fillPaint;
    }
    else
    {
      return nullopt;
    }
  }

  enum class BorderCorner
  {
    kTopLeft,
    kTopRight,
    kBottomRight,
    kBottomLeft
  };

  string to_string(const BorderCorner &corner)
  {
    switch (corner)
    {
    case BorderCorner::kTopLeft:
      return "border-top-left-radius";
    case BorderCorner::kTopRight:
      return "border-top-right-radius";
    case BorderCorner::kBottomRight:
      return "border-bottom-right-radius";
    case BorderCorner::kBottomLeft:
      return "border-bottom-left-radius";
    default:
      return "";
    }
  }

  // Compute the radius for a specific corner of the rounded rectangle.
  optional<SkVector> computeRoundedRectRadius(const SkRect &rect, const client_cssom::CSSStyleDeclaration &style,
                                              const BorderCorner &corner)
  {
    string name = to_string(corner);
    if (!style.hasProperty(name))
      return nullopt;

    auto cornerRadius = style.getPropertyValueAs<client_cssom::types::LengthPercentage>(name);
    if (cornerRadius.isPercentage())
    {
      return SkVector({cornerRadius.computePercentageToPixels(rect.width()),
                       cornerRadius.computePercentageToPixels(rect.height())});
    }
    else if (cornerRadius.isAbsoluteLength())
    {
      auto computedPixels = cornerRadius.computeAbsoluteLengthInPixels();
      return SkVector({computedPixels, computedPixels});
    }
    else
    {
      return nullopt;
    }
  }

  bool shouldDrawRoundedRect(SkRRect &roundedRect, SkRect &rect,
                             const client_cssom::CSSStyleDeclaration &style)
  {
    // Set the radius for all four corners.
    auto borderTopLeftRadius = computeRoundedRectRadius(rect, style, BorderCorner::kTopLeft);
    auto borderTopRightRadius = computeRoundedRectRadius(rect, style, BorderCorner::kTopRight);
    auto borderBottomRightRadius = computeRoundedRectRadius(rect, style, BorderCorner::kBottomRight);
    auto borderBottomLeftRadius = computeRoundedRectRadius(rect, style, BorderCorner::kBottomLeft);

    // Fast check for all zero radii.
    if (borderTopLeftRadius == nullopt &&
        borderTopRightRadius == nullopt &&
        borderBottomRightRadius == nullopt &&
        borderBottomLeftRadius == nullopt)
    {
      return false;
    }

    SkVector defaultRadius = {0.0f, 0.0f};
    SkVector radii[4] = {
        borderTopLeftRadius.value_or(defaultRadius),
        borderTopRightRadius.value_or(defaultRadius),
        borderBottomRightRadius.value_or(defaultRadius),
        borderBottomLeftRadius.value_or(defaultRadius)};
    roundedRect.setRectRadii(rect, radii);

    // Check if the radii are all zero.
    for (int i = 0; i < 4; i++)
    {
      if (radii[i].x() != 0.0f || radii[i].y() != 0.0f)
        return true;
    }

    // All corners have zero radius, so no need to draw rounded rectangle.
    return false;
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

  // Should draw the border edge, and return the computed border width.
  inline bool shouldDrawBorderEdge(const client_cssom::CSSStyleDeclaration &style,
                                   const string &edgeName,
                                   float &computedBorderWidth)
  {
    auto borderWidth = style.getPropertyValue("border-" + edgeName + "-width");
    auto borderStyle = style.getPropertyValue("border-" + edgeName + "-style");

    // Fast check for border style and width.
    if (borderStyle == "" ||
        borderStyle == "none" ||
        borderWidth == "" ||
        borderWidth == "0")
      return false;

    if (borderWidth == "thin")
      computedBorderWidth = 1.0f;
    else if (borderWidth == "medium")
      computedBorderWidth = 3.0f;
    else if (borderWidth == "thick")
      computedBorderWidth = 5.0f;
    else
    {
      client_cssom::types::LengthPercentage resolvedWidth(borderWidth);
      computedBorderWidth = resolvedWidth.computeAbsoluteLengthInPixels();
      if (computedBorderWidth <= 0.0f)
        return false;
    }
    return true;
  }

  bool drawBorders(SkCanvas *canvas, SkRRect &roundedRect,
                   const client_layout::Fragment &fragment,
                   const client_cssom::CSSStyleDeclaration &style)
  {
    using namespace client_cssom::types;

    bool hasBorders = false;
    SkPaint paint;
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setAntiAlias(true);

    float computedBorderWidth = 0.0f;
    if (shouldDrawBorderEdge(style, "top", computedBorderWidth))
    {
      auto borderColor = style.getPropertyValueAs<Color>("border-top-color");
      float halfBorderWidth = computedBorderWidth / 2.0f;

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
      hasBorders = true;
    }
    if (shouldDrawBorderEdge(style, "right", computedBorderWidth))
    {
      auto borderColor = style.getPropertyValueAs<Color>("border-right-color");
      float halfBorderWidth = computedBorderWidth / 2.0f;

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
      hasBorders = true;
    }
    if (shouldDrawBorderEdge(style, "bottom", computedBorderWidth))
    {
      auto borderColor = style.getPropertyValueAs<client_cssom::types::Color>("border-bottom-color");
      float halfBorderWidth = computedBorderWidth / 2.0f;

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
      hasBorders = true;
    }
    if (shouldDrawBorderEdge(style, "left", computedBorderWidth))
    {
      auto borderColor = style.getPropertyValueAs<client_cssom::types::Color>("border-left-color");
      float halfBorderWidth = computedBorderWidth / 2.0f;

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
      hasBorders = true;
    }
    return hasBorders;
  }

  void RenderBackgroundSystem::render(ecs::EntityId entity, WebContent &content)
  {
    const auto &style = content.style();
    const auto &fragment = content.fragment();
    if (!fragment.has_value()) // No layout, no rendering.
      return;

    auto canvas = content.canvas();
    canvas->clear(SK_ColorTRANSPARENT);

    float top = 0.0f;
    float left = 0.0f;

    SkRect rect = SkRect::MakeXYWH(left, top,
                                   content.width() - 2 * left,
                                   content.height() - 2 * top);
    SkRRect &roundedRect = content.roundedRect_;
    bool drawRoundedRect = shouldDrawRoundedRect(roundedRect, rect, style);

    auto backgroundPaint = drawBackground(canvas, roundedRect, fragment.value(), style);
    if (backgroundPaint.has_value())
    {
      auto fillPaint = backgroundPaint.value();
      if (fillPaint.getStyle() == SkPaint::kFill_Style &&
          !drawRoundedRect) // Disable using texture if the background is not rounded.
      {
        content.setTextureUsing(false); // Disable using texture to decrease the texture memory usage.

        auto fillColor = fillPaint.getColor4f();
        content.setBackgroundColor(fillColor.fR, fillColor.fG, fillColor.fB, fillColor.fA);
        content.setOpaque(fillColor.fA == 1.0f);
      }
      else
      {
        content.setTextureUsing(true);
      }
    }
    if (drawBorders(canvas, roundedRect, fragment.value(), style))
      content.setTextureUsing(true); // enable texture when there are borders.
  }

  void RenderImageSystem::render(ecs::EntityId entity, WebContent &content)
  {
    auto imageComponent = getComponent<Image2d>(entity);
    if (imageComponent == nullptr ||
        !imageComponent->hasImageData())
      return;

    // Disable using texture if the image is not visible.
    if (!imageComponent->visible())
    {
      content.setTextureUsing(false);
      return;
    }

    auto canvas = content.canvas();
    canvas->save();
    {
      SkRRect &roundedRect = content.roundedRect_;
      canvas->clipRRect(roundedRect, true);

      sk_sp<SkImage> image = imageComponent->image();
      SkRect srcRect = SkRect::MakeWH(image->width(), image->height());
      SkRect dstRect = SkRect::MakeWH(content.width(), content.height());
      canvas->drawImageRect(image, srcRect, dstRect,
                            SkSamplingOptions(), nullptr,
                            SkCanvas::kStrict_SrcRectConstraint);
    }
    canvas->restore();
    content.setTextureUsing(true);
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

    string &text = textComponent->content;
    auto paragraphStyle = content.paragraphStyle();
    auto paragraphBuilder = ParagraphBuilder::make(paragraphStyle, fontCollection_);
    paragraphBuilder->pushStyle(paragraphStyle.getTextStyle());
    paragraphBuilder->addText(text.c_str(), text.size());
    paragraphBuilder->pop();

    auto layoutWidth = round(getLayoutWidthForText(content)) + 1.0f;
    auto paragraph = paragraphBuilder->Build();
    paragraph->layout(layoutWidth);
    paragraph->paint(content.canvas(), 0.0f, 0.0f);
    content.setTextureUsing(true);
  }

  float RenderTextSystem::getLayoutWidthForText(WebContent &content)
  {
    const auto &fragment = content.fragment();
    return fragment->contentWidth();
  }

  void UpdateTextureSystem::render(ecs::EntityId entity, WebContent &content)
  {
    auto material3d = getInstancedMeshComponent<MeshMaterial3d>();
    assert(material3d != nullptr);

    auto webContentMaterial = material3d->material<materials::WebContentInstancedMaterial>();
    if (webContentMaterial)
    {
      auto status = webContentMaterial->updateTexture(content);
      // Mark the content as clean if the texture is no need to update or updated successfully.
      if (status != materials::WebContentInstancedMaterial::TextureUpdateStatus::kFailed)
        content.setDirty(false);
    }
  }
}
