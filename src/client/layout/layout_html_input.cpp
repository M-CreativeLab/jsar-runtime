#include "./layout_html_input.hpp"
#include <client/builtin_scene/ecs-inl.hpp>
#include <client/builtin_scene/web_content.hpp>
#include <client/html/html_input_element.hpp>
#include <client/dom/node.hpp>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkPaint.h>
#include <skia/include/core/SkPath.h>
#include <skia/include/core/SkRRect.h>

namespace client_layout
{
  using namespace builtin_scene;
  using namespace dom;

  void LayoutHTMLInput::entityDidCreate(builtin_scene::ecs::EntityId entity)
  {
    LayoutReplaced::entityDidCreate(entity);

    // Add WebContent component for checkbox/radio rendering
    if (needsCustomRendering())
    {
      auto addWebContentComponent = [&entity](Scene &scene)
      {
        scene.addComponent(entity, WebContent("input-control", 20.0f, 20.0f));
      };
      useSceneWithCallback(addWebContentComponent);

      // Trigger initial render
      updateInputVisuals();
    }
  }

  void LayoutHTMLInput::entityWillBeDestroyed(builtin_scene::ecs::EntityId entity)
  {
    auto removeWebContentComponent = [&entity](Scene &scene)
    {
      if (scene.hasComponent<WebContent>(entity))
      {
        scene.removeComponent<WebContent>(entity);
      }
    };
    useSceneWithCallback(removeWebContentComponent);

    LayoutReplaced::entityWillBeDestroyed(entity);
  }

  bool LayoutHTMLInput::needsCustomRendering() const
  {
    const auto &input_element = Node::AsChecked<const HTMLInputElement>(node());
    std::string type = input_element.type();
    return type == "checkbox" || type == "radio";
  }

  void LayoutHTMLInput::updateInputVisuals()
  {
    if (!needsCustomRendering())
      return;

    auto updateVisuals = [this](Scene &scene)
    {
      if (!scene.hasComponent<WebContent>(entity()))
        return;

      WebContent &webContent = scene.getComponentChecked<WebContent>(entity());
      webContent.setContentDirty(true);

      renderCheckboxOrRadio();
    };
    useSceneWithCallback(updateVisuals);
  }

  void LayoutHTMLInput::renderCheckboxOrRadio()
  {
    auto renderControl = [this](Scene &scene)
    {
      if (!scene.hasComponent<WebContent>(entity()))
        return;

      WebContent &webContent = scene.getComponentChecked<WebContent>(entity());
      SkCanvas *canvas = webContent.canvas();
      if (!canvas)
        return;

      const auto &input_element = Node::AsChecked<const HTMLInputElement>(node());
      std::string type = input_element.type();
      bool checked = input_element.checked();
      bool disabled = input_element.disabled();
      bool indeterminate = input_element.indeterminate();

      // Clear the canvas
      canvas->clear(SK_ColorTRANSPARENT);

      // Get control dimensions
      float width = webContent.logicalWidth();
      float height = webContent.logicalHeight();
      float size = std::min(width, height);

      // Center the control
      float x = (width - size) * 0.5f;
      float y = (height - size) * 0.5f;
      SkRect controlRect = SkRect::MakeXYWH(x, y, size, size);

      // Define colors
      SkColor borderColor = disabled ? SkColorSetARGB(255, 170, 170, 170) : SkColorSetARGB(255, 118, 118, 118);
      SkColor backgroundColor = disabled ? SkColorSetARGB(255, 245, 245, 245) : SK_ColorWHITE;
      SkColor checkColor = disabled ? SkColorSetARGB(255, 170, 170, 170) : SkColorSetARGB(255, 51, 51, 51);

      if (type == "checkbox")
      {
        // Draw checkbox background
        SkPaint bgPaint;
        bgPaint.setColor(backgroundColor);
        bgPaint.setStyle(SkPaint::kFill_Style);
        bgPaint.setAntiAlias(true);

        SkRRect roundedRect = SkRRect::MakeRectXY(controlRect, 2.0f, 2.0f);
        canvas->drawRRect(roundedRect, bgPaint);

        // Draw checkbox border
        SkPaint borderPaint;
        borderPaint.setColor(borderColor);
        borderPaint.setStyle(SkPaint::kStroke_Style);
        borderPaint.setStrokeWidth(1.0f);
        borderPaint.setAntiAlias(true);
        canvas->drawRRect(roundedRect, borderPaint);

        // Draw checkmark if checked, or indeterminate bar if indeterminate
        if (indeterminate)
        {
          // Draw indeterminate bar (horizontal line)
          SkPaint indeterminatePaint;
          indeterminatePaint.setColor(checkColor);
          indeterminatePaint.setStyle(SkPaint::kStroke_Style);
          indeterminatePaint.setStrokeWidth(2.0f);
          indeterminatePaint.setStrokeCap(SkPaint::kRound_Cap);
          indeterminatePaint.setAntiAlias(true);

          float barInset = size * 0.3f;
          float barY = y + size * 0.5f;
          canvas->drawLine(x + barInset, barY, x + size - barInset, barY, indeterminatePaint);
        }
        else if (checked)
        {
          // Draw checkmark
          SkPaint checkPaint;
          checkPaint.setColor(checkColor);
          checkPaint.setStyle(SkPaint::kStroke_Style);
          checkPaint.setStrokeWidth(2.0f);
          checkPaint.setStrokeCap(SkPaint::kRound_Cap);
          checkPaint.setStrokeJoin(SkPaint::kRound_Join);
          checkPaint.setAntiAlias(true);

          // Draw checkmark path
          SkPath checkPath;
          float checkInset = size * 0.25f;
          float checkX = x + checkInset;
          float checkY = y + size * 0.5f;
          float checkW = size - 2 * checkInset;
          float checkH = checkW * 0.6f;

          checkPath.moveTo(checkX, checkY);
          checkPath.lineTo(checkX + checkW * 0.4f, checkY + checkH * 0.6f);
          checkPath.lineTo(checkX + checkW, checkY - checkH * 0.4f);

          canvas->drawPath(checkPath, checkPaint);
        }
      }
      else if (type == "radio")
      {
        // Draw radio button background
        SkPaint bgPaint;
        bgPaint.setColor(backgroundColor);
        bgPaint.setStyle(SkPaint::kFill_Style);
        bgPaint.setAntiAlias(true);

        float centerX = controlRect.centerX();
        float centerY = controlRect.centerY();
        float radius = size * 0.5f;

        canvas->drawCircle(centerX, centerY, radius, bgPaint);

        // Draw radio button border
        SkPaint borderPaint;
        borderPaint.setColor(borderColor);
        borderPaint.setStyle(SkPaint::kStroke_Style);
        borderPaint.setStrokeWidth(1.0f);
        borderPaint.setAntiAlias(true);
        canvas->drawCircle(centerX, centerY, radius, borderPaint);

        // Draw radio dot if checked
        if (checked)
        {
          SkPaint dotPaint;
          dotPaint.setColor(checkColor);
          dotPaint.setStyle(SkPaint::kFill_Style);
          dotPaint.setAntiAlias(true);

          float dotRadius = radius * 0.4f;
          canvas->drawCircle(centerX, centerY, dotRadius, dotPaint);
        }
      }

      webContent.setContentDirty(false);
      webContent.setSurfaceDirty(true);
    };

    useSceneWithCallback(renderControl);
  }
}
