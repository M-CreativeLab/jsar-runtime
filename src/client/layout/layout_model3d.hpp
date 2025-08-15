#pragma once

#include <client/builtin_scene/ecs.hpp>
#include <client/dom/node.hpp>
#include <client/html/html_model_element.hpp>

#include "./layout_replaced.hpp"

namespace client_layout
{
  /**
   * The layout box for 3D model types:
   *
   * 1. 3D Gaussian Splatting models: <model type="3dgs">.
   */
  class LayoutModel3d final : public LayoutReplaced
  {
    using LayoutReplaced::LayoutReplaced;

  public:
    const char *name() const override
    {
      return "LayoutModel3d";
    }
    bool isLayoutModel3d() const override final
    {
      return true;
    }

    void setModelData(const std::vector<dom::HTMLModelElement::GaussianSplat> &splats);

  private:
    inline dom::HTMLModelElement &modelElement()
    {
      return dom::Node::AsChecked<dom::HTMLModelElement>(node());
    }
    inline const dom::HTMLModelElement &modelElement() const
    {
      return dom::Node::AsChecked<const dom::HTMLModelElement>(node());
    }

    void entityDidCreate(builtin_scene::ecs::EntityId entity) override;
    void entityWillBeDestroyed(builtin_scene::ecs::EntityId entity) override;
    void styleWillChange(client_cssom::ComputedStyle &new_style) override;
    void didComputeLayoutOnce(const ConstraintSpace &) override;
    void sizeDidChange(const Fragment &newSize) override;

    void layoutDidFirstReady(const Fragment &);

    // Set if the model should be visible or not in drawing.
    void setVisible(bool);

  private:
    // Whether the layout is computed, it means the loading can be started when the layout is computed.
    bool is_layout_ready_ = false;
    std::optional<bool> last_visible_ = std::nullopt;
  };
}