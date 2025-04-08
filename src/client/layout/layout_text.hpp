#pragma once

#include <memory>
#include <client/builtin_scene/ecs.hpp>
#include <client/dom/types.hpp>

#include "./layout_object.hpp"

namespace client_layout
{
  class LayoutText final : public LayoutObject
  {
  public:
    /**
     * Construct a text sequence.
     *
     * @param tree The box tree.
     * @param textNode The text node to be associated with the text sequence.
     */
    LayoutText(std::shared_ptr<dom::Text> textNode);

  public:
    const char *name() const override { return "LayoutText"; }
    bool isText() const override final { return true; }

    std::shared_ptr<dom::Text> textNode() const;
    std::string plainText() const;
    inline size_t plainTextLength() const { return plainText().length(); }

    // Adjust the input `ConstraintSpace` by the text and returns the adjusted space.
    const ConstraintSpace adjustSpace(const ConstraintSpace &inputSpace) const;

  private:
    void entityDidCreated(builtin_scene::ecs::EntityId entity) override;
    void entityWillBeDestroyed(builtin_scene::ecs::EntityId entity) override;
    void didComputeLayoutOnce(const ConstraintSpace &avilableSpace) override final;
  };
}
