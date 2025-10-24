#include "./layout_html_input.hpp"
#include <client/builtin_scene/ecs-inl.hpp>

namespace endor
{
  namespace client_layout
  {
    void LayoutHTMLInput::entityDidCreate(builtin_scene::ecs::EntityId entity)
    {
      LayoutReplaced::entityDidCreate(entity);

      // Add any input-specific entity components here
      // For now, we'll use the basic layout handling
    }

    void LayoutHTMLInput::entityWillBeDestroyed(builtin_scene::ecs::EntityId entity)
    {
      // Clean up any input-specific entity components here

      LayoutReplaced::entityWillBeDestroyed(entity);
    }
  }
} // namespace endor
