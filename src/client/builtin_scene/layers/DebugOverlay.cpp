#include "./DebugOverlay.hpp"
#include <algorithm>
#include <skia/include/effects/SkDashPathEffect.h>

namespace builtin_scene::layers
{
  DebugOverlay::DebugOverlay(const LayerRenderConfig &config)
      : config_(config)
      , enabled_(config.enableDebugOverlay)
  {
    // Initialize debug drawing styles
    layerBoundsPaint_.setStyle(SkPaint::kStroke_Style);
    layerBoundsPaint_.setAntiAlias(true);
    layerBoundsPaint_.setStrokeWidth(1.0f);

    clipRegionPaint_.setStyle(SkPaint::kStroke_Style);
    clipRegionPaint_.setAntiAlias(true);
    clipRegionPaint_.setStrokeWidth(2.0f);
    clipRegionPaint_.setColor(SK_ColorRED);

    scrollIndicatorPaint_.setStyle(SkPaint::kFill_Style);
    scrollIndicatorPaint_.setAntiAlias(true);
    scrollIndicatorPaint_.setColor(SkColorSetARGB(128, 0, 255, 0));

    hierarchyPaint_.setStyle(SkPaint::kStroke_Style);
    hierarchyPaint_.setAntiAlias(true);
    hierarchyPaint_.setStrokeWidth(1.0f);
    hierarchyPaint_.setColor(SkColorSetARGB(128, 128, 128, 128));
  }

  void DebugOverlay::collectDebugInfo(const std::vector<std::shared_ptr<LayerNode>> &rootLayers)
  {
    clear();

    for (const auto &rootLayer : rootLayers)
    {
      collectLayerDebugInfo(rootLayer, 0);
    }
  }

  void DebugOverlay::render(SkCanvas *canvas, float devicePixelRatio)
  {
    if (!enabled_ || debugInfos_.empty())
      return;

    canvas->save();

    // Draw hierarchy connections first (behind other elements)
    drawHierarchyConnections(canvas, devicePixelRatio);

    // Draw layer information
    for (const auto &info : debugInfos_)
    {
      drawLayerBounds(canvas, info, devicePixelRatio);
      drawClipRegion(canvas, info, devicePixelRatio);
      drawScrollIndicators(canvas, info, devicePixelRatio);
    }

    canvas->restore();
  }

  void DebugOverlay::clear()
  {
    debugInfos_.clear();
  }

  void DebugOverlay::collectLayerDebugInfo(std::shared_ptr<LayerNode> node, int depth)
  {
    DebugInfo info;
    info.entityId = node->entityId;
    info.layerIndex = node->layerIndex;
    info.clipRect = node->clipRect;
    info.roundedClipRect = node->roundedClipRect;
    info.scrollOffset = node->scrollOffset;
    info.isScrollable = node->isScrollable;
    info.hasOverflow = node->hasOverflow;
    info.children = node->children;

    // Calculate bounds from clip rect or set default
    if (node->clipRect.has_value())
    {
      info.bounds = node->clipRect.value();
    }
    else
    {
      info.bounds = SkRect::MakeXYWH(0, 0, 100, 100); // Default size for debugging
    }

    debugInfos_.push_back(info);

    // Recursively collect children
    for (const auto &child : node->children)
    {
      collectLayerDebugInfo(child, depth + 1);
    }
  }

  void DebugOverlay::drawLayerBounds(SkCanvas *canvas, const DebugInfo &info, float devicePixelRatio)
  {
    layerBoundsPaint_.setColor(getLayerColor(info));
    layerBoundsPaint_.setStrokeWidth(getStrokeWidth(1.0f, devicePixelRatio));

    canvas->drawRect(info.bounds, layerBoundsPaint_);

    // Draw layer index text
    SkPaint textPaint;
    textPaint.setAntiAlias(true);
    textPaint.setColor(SK_ColorBLACK);
    textPaint.setTextSize(12.0f * devicePixelRatio);

    SkString layerText = SkStringPrintf("L%d", info.layerIndex);
    canvas->drawString(layerText,
                       info.bounds.fLeft + 2.0f * devicePixelRatio,
                       info.bounds.fTop + 15.0f * devicePixelRatio,
                       SkFont(),
                       textPaint);
  }

  void DebugOverlay::drawClipRegion(SkCanvas *canvas, const DebugInfo &info, float devicePixelRatio)
  {
    clipRegionPaint_.setStrokeWidth(getStrokeWidth(2.0f, devicePixelRatio));

    if (info.roundedClipRect.has_value())
    {
      clipRegionPaint_.setColor(SK_ColorMAGENTA);
      canvas->drawRRect(info.roundedClipRect.value(), clipRegionPaint_);
    }
    else if (info.clipRect.has_value())
    {
      clipRegionPaint_.setColor(SK_ColorRED);

      // Draw dashed line for clip rect
      const SkScalar intervals[] = {5.0f * devicePixelRatio, 5.0f * devicePixelRatio};
      auto dashEffect = SkDashPathEffect::Make(intervals, 2, 0);
      clipRegionPaint_.setPathEffect(dashEffect);

      canvas->drawRect(info.clipRect.value(), clipRegionPaint_);

      clipRegionPaint_.setPathEffect(nullptr);
    }
  }

  void DebugOverlay::drawScrollIndicators(SkCanvas *canvas, const DebugInfo &info, float devicePixelRatio)
  {
    if (!info.isScrollable)
      return;

    // Draw scroll offset indicator
    if (info.scrollOffset.x != 0.0f || info.scrollOffset.y != 0.0f)
    {
      scrollIndicatorPaint_.setColor(SkColorSetARGB(128, 0, 255, 0));

      SkRect scrollIndicator = SkRect::MakeXYWH(
        info.bounds.fLeft + info.scrollOffset.x,
        info.bounds.fTop + info.scrollOffset.y,
        8.0f * devicePixelRatio,
        8.0f * devicePixelRatio);

      canvas->drawOval(scrollIndicator, scrollIndicatorPaint_);
    }

    // Draw overflow indicator
    if (info.hasOverflow)
    {
      scrollIndicatorPaint_.setColor(SkColorSetARGB(128, 255, 165, 0)); // Orange

      SkRect overflowIndicator = SkRect::MakeXYWH(
        info.bounds.fRight - 8.0f * devicePixelRatio,
        info.bounds.fTop,
        8.0f * devicePixelRatio,
        8.0f * devicePixelRatio);

      canvas->drawRect(overflowIndicator, scrollIndicatorPaint_);
    }
  }

  void DebugOverlay::drawHierarchyConnections(SkCanvas *canvas, float devicePixelRatio)
  {
    hierarchyPaint_.setStrokeWidth(getStrokeWidth(1.0f, devicePixelRatio));

    for (const auto &info : debugInfos_)
    {
      SkPoint parentCenter = SkPoint::Make(info.bounds.centerX(), info.bounds.centerY());

      for (const auto &child : info.children)
      {
        // Find the child in our debug info list
        auto childIt = std::find_if(debugInfos_.begin(), debugInfos_.end(), [&child](const DebugInfo &debugInfo)
                                    { return debugInfo.entityId == child->entityId; });

        if (childIt != debugInfos_.end())
        {
          SkPoint childCenter = SkPoint::Make(childIt->bounds.centerX(), childIt->bounds.centerY());
          canvas->drawLine(parentCenter, childCenter, hierarchyPaint_);
        }
      }
    }
  }

  SkColor DebugOverlay::getLayerColor(const DebugInfo &info) const
  {
    // Color code layers based on properties
    if (info.isScrollable && info.hasOverflow)
      return SK_ColorBLUE; // Scrollable with overflow
    else if (info.isScrollable)
      return SK_ColorCYAN; // Scrollable without overflow
    else if (info.clipRect.has_value() || info.roundedClipRect.has_value())
      return SK_ColorGREEN; // Has clipping
    else
      return SK_ColorGRAY; // Basic layer
  }

  float DebugOverlay::getStrokeWidth(float baseWidth, float devicePixelRatio) const
  {
    return std::max(1.0f, baseWidth * devicePixelRatio);
  }
}