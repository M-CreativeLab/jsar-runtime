#pragma once

#include <stack>
#include <optional>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkRect.h>
#include <skia/include/core/SkRRect.h>
#include <skia/include/core/SkPath.h>
#include "./LayerTypes.hpp"

namespace builtin_scene::layers
{
  /**
   * Manages the clipping state stack for layered rendering.
   * Handles both scissor (axis-aligned rectangles) and stencil 
   * (arbitrary shapes) clipping modes.
   */
  class ClipStack
  {
  public:
    struct ClipState
    {
      enum Type
      {
        kRect,
        kRRect,
        kPath
      };

      Type type;
      std::optional<SkRect> rect;
      std::optional<SkRRect> roundedRect;
      std::optional<SkPath> path;
      bool useScissor;
      bool useStencil;
      int saveCount; // Canvas save count when this clip was pushed

      ClipState()
          : type(kRect)
          , useScissor(false)
          , useStencil(false)
          , saveCount(0)
      {
      }
    };

  public:
    ClipStack(const LayerRenderConfig &config);
    ~ClipStack() = default;

    /**
     * Push a rectangular clip region onto the stack.
     * Uses scissor test when possible for performance.
     */
    void pushClip(SkCanvas *canvas, const SkRect &rect);

    /**
     * Push a rounded rectangular clip region onto the stack.
     * Uses stencil test for non-axis-aligned clips.
     */
    void pushClip(SkCanvas *canvas, const SkRRect &roundedRect);

    /**
     * Push an arbitrary path clip region onto the stack.
     * Always uses stencil test for complex shapes.
     */
    void pushClip(SkCanvas *canvas, const SkPath &path);

    /**
     * Pop the most recent clip region from the stack.
     * Restores the previous clipping state.
     */
    void popClip(SkCanvas *canvas);

    /**
     * Clear all clip regions and restore original state.
     */
    void clearAll(SkCanvas *canvas);

    /**
     * Get the current effective clip region in world space.
     */
    std::optional<SkRect> getCurrentClipRect() const;

    /**
     * Check if the stack is empty.
     */
    bool empty() const
    {
      return clipStack_.empty();
    }

    /**
     * Get the depth of the clip stack.
     */
    size_t depth() const
    {
      return clipStack_.size();
    }

  private:
    /**
     * Determine whether to use scissor or stencil for a given clip shape.
     */
    bool shouldUseScissor(const SkRect &rect) const;
    bool shouldUseScissor(const SkRRect &roundedRect) const;

    /**
     * Apply scissor clipping for axis-aligned rectangles.
     */
    void applyScissorClip(SkCanvas *canvas, const SkRect &rect, ClipState &state);

    /**
     * Apply stencil clipping for complex shapes.
     */
    void applyStencilClip(SkCanvas *canvas, const SkRRect &roundedRect, ClipState &state);
    void applyStencilClip(SkCanvas *canvas, const SkPath &path, ClipState &state);

    /**
     * Update the effective clip region with a new clip.
     */
    void updateEffectiveClip(const SkRect &newClip);

    /**
     * Recalculate the effective clip from the current stack.
     */
    void recalculateEffectiveClip();

  private:
    const LayerRenderConfig &config_;
    std::stack<ClipState> clipStack_;
    std::optional<SkRect> currentEffectiveClip_;
  };
}