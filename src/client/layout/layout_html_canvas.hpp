#pragma once

#include <skia/include/core/SkBitmap.h>
#include <client/builtin_scene/ecs.hpp>

#include "./layout_replaced.hpp"

namespace client_layout
{
  class LayoutHTMLCanvas final : public LayoutReplaced
  {
    using LayoutReplaced::LayoutReplaced;

  public:
    const char *name() const override { return "LayoutHTMLCanvas"; }
    bool isCanvas() const override final { return true; }

    bool adjustDrawingSize();
    void setDrawingBitmap(std::shared_ptr<const SkBitmap> src_bitmap);
    // Mark the canvas as dirty to update the bitmap to content's texture.
    void markCanvasAsDirty();

  private:
    void entityDidCreate(builtin_scene::ecs::EntityId entity) override;
    void entityWillBeDestroyed(builtin_scene::ecs::EntityId entity) override;

  private:
    // Whether the layout is computed, it means the loading can be started when the layout is computed.
    bool is_layout_ready_ = false;
    std::optional<bool> last_visible_ = std::nullopt;
  };
}
