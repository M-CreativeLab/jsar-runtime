#include "./ClipStack.hpp"
#include <algorithm>

namespace builtin_scene::layers
{
  ClipStack::ClipStack(const LayerRenderConfig &config)
      : config_(config)
  {
  }

  void ClipStack::pushClip(SkCanvas *canvas, const SkRect &rect)
  {
    ClipState state;
    state.type = ClipState::kRect;
    state.rect = rect;

    if (shouldUseScissor(rect))
    {
      applyScissorClip(canvas, rect, state);
    }
    else
    {
      // Fallback to canvas clipping
      state.saveCount = canvas->save();
      canvas->clipRect(rect, true);
      state.useStencil = true;
    }

    clipStack_.push(state);
    updateEffectiveClip(rect);
  }

  void ClipStack::pushClip(SkCanvas *canvas, const SkRRect &roundedRect)
  {
    ClipState state;
    state.type = ClipState::kRRect;
    state.roundedRect = roundedRect;

    if (shouldUseScissor(roundedRect))
    {
      // Use scissor for axis-aligned rounded rects with small radii
      applyScissorClip(canvas, roundedRect.rect(), state);
    }
    else
    {
      applyStencilClip(canvas, roundedRect, state);
    }

    clipStack_.push(state);
    updateEffectiveClip(roundedRect.rect());
  }

  void ClipStack::pushClip(SkCanvas *canvas, const SkPath &path)
  {
    ClipState state;
    state.type = ClipState::kPath;
    state.path = path;

    applyStencilClip(canvas, path, state);
    clipStack_.push(state);

    // Update effective clip with path bounds
    SkRect pathBounds = path.getBounds();
    updateEffectiveClip(pathBounds);
  }

  void ClipStack::popClip(SkCanvas *canvas)
  {
    if (clipStack_.empty())
      return;

    ClipState state = clipStack_.top();
    clipStack_.pop();

    // Restore canvas state
    canvas->restoreToCount(state.saveCount);

    // Update effective clip
    if (clipStack_.empty())
    {
      currentEffectiveClip_ = std::nullopt;
    }
    else
    {
      // Recalculate effective clip from remaining stack
      recalculateEffectiveClip();
    }
  }

  void ClipStack::clearAll(SkCanvas *canvas)
  {
    while (!clipStack_.empty())
    {
      ClipState state = clipStack_.top();
      clipStack_.pop();
      canvas->restoreToCount(state.saveCount);
    }
    currentEffectiveClip_ = std::nullopt;
  }

  std::optional<SkRect> ClipStack::getCurrentClipRect() const
  {
    return currentEffectiveClip_;
  }

  bool ClipStack::shouldUseScissor(const SkRect &rect) const
  {
    if (!config_.preferScissorClipping)
      return false;

    // Scissor test works well for axis-aligned rectangles
    return true;
  }

  bool ClipStack::shouldUseScissor(const SkRRect &roundedRect) const
  {
    if (!config_.preferScissorClipping)
      return false;

    // Use scissor only for rectangles with very small corner radii
    // where the difference is negligible
    const float maxRadiusForScissor = 2.0f * config_.devicePixelRatio;

    for (int i = 0; i < 4; i++)
    {
      SkVector radius = roundedRect.radii(static_cast<SkRRect::Corner>(i));
      if (radius.x() > maxRadiusForScissor || radius.y() > maxRadiusForScissor)
        return false;
    }

    return true;
  }

  void ClipStack::applyScissorClip(SkCanvas *canvas, const SkRect &rect, ClipState &state)
  {
    state.saveCount = canvas->save();
    canvas->clipRect(rect, true);
    state.useScissor = true;
  }

  void ClipStack::applyStencilClip(SkCanvas *canvas, const SkRRect &roundedRect, ClipState &state)
  {
    if (!config_.enableStencilClipping)
    {
      // Fallback to simple rect clipping
      applyScissorClip(canvas, roundedRect.rect(), state);
      return;
    }

    state.saveCount = canvas->save();
    canvas->clipRRect(roundedRect, true);
    state.useStencil = true;
  }

  void ClipStack::applyStencilClip(SkCanvas *canvas, const SkPath &path, ClipState &state)
  {
    if (!config_.enableStencilClipping)
    {
      // Fallback to bounding rect clipping
      SkRect bounds = path.getBounds();
      applyScissorClip(canvas, bounds, state);
      return;
    }

    state.saveCount = canvas->save();
    canvas->clipPath(path, true);
    state.useStencil = true;
  }

  void ClipStack::updateEffectiveClip(const SkRect &newClip)
  {
    if (!currentEffectiveClip_.has_value())
    {
      currentEffectiveClip_ = newClip;
    }
    else
    {
      // Intersect with existing clip
      if (!currentEffectiveClip_->intersect(newClip))
      {
        // No intersection - set to empty
        currentEffectiveClip_ = SkRect::MakeEmpty();
      }
    }
  }

  void ClipStack::recalculateEffectiveClip()
  {
    currentEffectiveClip_ = std::nullopt;

    // Make a copy of the stack to iterate without modifying
    std::stack<ClipState> tempStack = clipStack_;
    std::vector<ClipState> states;

    while (!tempStack.empty())
    {
      states.push_back(tempStack.top());
      tempStack.pop();
    }

    // Process in reverse order (bottom to top of stack)
    for (auto it = states.rbegin(); it != states.rend(); ++it)
    {
      SkRect clipRect;

      switch (it->type)
      {
      case ClipState::kRect:
        clipRect = it->rect.value();
        break;
      case ClipState::kRRect:
        clipRect = it->roundedRect->rect();
        break;
      case ClipState::kPath:
        clipRect = it->path->getBounds();
        break;
      }

      updateEffectiveClip(clipRect);
    }
  }
}