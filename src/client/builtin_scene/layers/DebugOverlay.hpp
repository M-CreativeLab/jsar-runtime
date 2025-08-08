#pragma once

#include <vector>
#include <memory>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkPaint.h>
#include <skia/include/core/SkRect.h>
#include <skia/include/core/SkRRect.h>
#include <skia/include/core/SkColor.h>
#include "./LayerTypes.hpp"

namespace builtin_scene::layers
{
  /**
   * Provides debug visualization for layer bounds and clip regions.
   * Useful for debugging layer hierarchy and clipping issues.
   */
  class DebugOverlay
  {
  public:
    struct DebugInfo
    {
      ecs::EntityId entityId;
      int layerIndex;
      SkRect bounds;
      std::optional<SkRect> clipRect;
      std::optional<SkRRect> roundedClipRect;
      glm::vec3 scrollOffset;
      bool isScrollable;
      bool hasOverflow;
      std::vector<std::shared_ptr<LayerNode>> children;
    };

  public:
    DebugOverlay(const LayerRenderConfig &config);
    ~DebugOverlay() = default;

    /**
     * Enable or disable debug overlay rendering.
     */
    void setEnabled(bool enabled)
    {
      enabled_ = enabled;
    }
    bool isEnabled() const
    {
      return enabled_;
    }

    /**
     * Collect debug information from the layer tree.
     */
    void collectDebugInfo(const std::vector<std::shared_ptr<LayerNode>> &rootLayers);

    /**
     * Render debug overlay on the given canvas.
     * Shows layer bounds, clip regions, and hierarchy information.
     */
    void render(SkCanvas *canvas, float devicePixelRatio = 1.0f);

    /**
     * Clear collected debug information.
     */
    void clear();

  private:
    /**
     * Recursively collect debug info from layer tree.
     */
    void collectLayerDebugInfo(std::shared_ptr<LayerNode> node, int depth = 0);

    /**
     * Draw layer bounds with color coding based on layer properties.
     */
    void drawLayerBounds(SkCanvas *canvas, const DebugInfo &info, float devicePixelRatio);

    /**
     * Draw clip regions with different styles.
     */
    void drawClipRegion(SkCanvas *canvas, const DebugInfo &info, float devicePixelRatio);

    /**
     * Draw scroll indicators for scrollable layers.
     */
    void drawScrollIndicators(SkCanvas *canvas, const DebugInfo &info, float devicePixelRatio);

    /**
     * Draw layer hierarchy connections.
     */
    void drawHierarchyConnections(SkCanvas *canvas, float devicePixelRatio);

    /**
     * Get color for layer based on its properties.
     */
    SkColor getLayerColor(const DebugInfo &info) const;

    /**
     * Get stroke width adjusted for device pixel ratio.
     */
    float getStrokeWidth(float baseWidth, float devicePixelRatio) const;

  private:
    const LayerRenderConfig &config_;
    bool enabled_;
    std::vector<DebugInfo> debugInfos_;

    // Debug drawing styles
    SkPaint layerBoundsPaint_;
    SkPaint clipRegionPaint_;
    SkPaint scrollIndicatorPaint_;
    SkPaint hierarchyPaint_;
  };
}