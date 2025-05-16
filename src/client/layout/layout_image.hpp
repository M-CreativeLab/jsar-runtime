#pragma once

#include <skia/include/core/SkBitmap.h>
#include <client/builtin_scene/ecs.hpp>

#include "./layout_replaced.hpp"

namespace client_layout
{
  /**
   * The layout box for image types:
   *
   * 1. normal image types: <img>, <picture>.
   * 2. content image types: "content: url(path/to/image.png)".
   */
  class LayoutImage final : public LayoutReplaced
  {
    using LayoutReplaced::LayoutReplaced;

  public:
    const char *name() const override { return "LayoutImage"; }
    bool isLayoutImage() const override final { return true; }

    // Adjusts the image size according to the style, and returns `false` if the size is not changed.
    bool adjustImageSize();
    void setImageBitmap(std::shared_ptr<SkBitmap> srcBitmap);

  private:
    void entityDidCreate(builtin_scene::ecs::EntityId entity) override;
    void entityWillBeDestroyed(builtin_scene::ecs::EntityId entity) override;
    void styleWillChange(client_cssom::ComputedStyle &new_style) override;
    void didComputeLayoutOnce(const ConstraintSpace &) override;
    void sizeDidChange() override;

    void layoutDidFirstReady(const Fragment &);

    // Set if the image should be visible or not in drawing.
    void setVisible(bool);

  private:
    // Whether the layout is computed, it means the loading can be started when the layout is computed.
    bool is_layout_ready_ = false;
    std::optional<bool> last_visible_ = std::nullopt;
  };
}
