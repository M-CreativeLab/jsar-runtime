#pragma once

#include "./layout_replaced.hpp"

namespace client_layout
{
  class LayoutHTMLInput final : public LayoutReplaced
  {
    using LayoutReplaced::LayoutReplaced;

  public:
    const char *name() const override
    {
      return "LayoutHTMLInput";
    }
    bool isInput() const override final
    {
      return true;
    }

  private:
    void entityDidCreate(builtin_scene::ecs::EntityId entity) override;
    void entityWillBeDestroyed(builtin_scene::ecs::EntityId entity) override;
  };
}
