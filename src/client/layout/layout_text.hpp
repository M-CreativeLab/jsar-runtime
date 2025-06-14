#pragma once

#include <memory>
#include <client/builtin_scene/ecs.hpp>
#include <client/dom/types.hpp>

#include "./layout_object.hpp"

namespace client_layout
{
  constexpr char16_t kBulletCharacter = 0x2022;
  constexpr char16_t kBlackSquareCharacter = 0x25A0;
  constexpr char16_t kWhiteBulletCharacter = 0x25E6;

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
    const char *name() const override
    {
      return "LayoutText";
    }
    bool isText() const override final
    {
      return true;
    }
    bool isEmptyText() const override final
    {
      return plainTextLength() == 0;
    }

    std::shared_ptr<dom::Text> textNode() const;
    std::string plainText() const;
    std::string transformedText() const;
    inline size_t plainTextLength() const
    {
      return plainText().length();
    }

    // Adjust the input `ConstraintSpace` by the text and returns the adjusted space.
    const ConstraintSpace adjustSpace(const ConstraintSpace &inputSpace) const;

    // Called when the text content is changed, it internally updates the text for layout and rendering.
    void textDidChange();

  private:
    bool nodeAtPoint(HitTestResult &, const HitTestRay &, const glm::vec3 &, HitTestPhase) override final
    {
      assert(false && "Unreachable.");
    }

    void entityDidCreate(builtin_scene::ecs::EntityId entity) override;
    void entityWillBeDestroyed(builtin_scene::ecs::EntityId entity) override;
    void formattingContextDidSet(FormattingContext &) override;
    void styleWillChange(client_cssom::ComputedStyle &new_style) override;
    void didComputeLayoutOnce(const ConstraintSpace &) override final;

    // Adjust the text's internal content size, call this when the text content is changed.
    void adjustTextContentSize(const ConstraintSpace &);
    std::string transformAndSecureText(const std::string &original) const;

  private:
    std::optional<ConstraintSpace> last_space_;
    mutable std::optional<std::string> plain_text_;
    mutable std::optional<std::string> transformed_text_;
    mutable bool is_text_content_dirty_ = true;
  };
}
