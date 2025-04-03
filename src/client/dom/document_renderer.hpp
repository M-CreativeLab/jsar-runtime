#pragma once

#include <memory>
#include <client/builtin_scene/ecs-inl.hpp>
#include <client/layout/constraint_space.hpp>
#include <client/layout/layout_view_visitor.hpp>
#include <client/layout/layout_object.hpp>
#include <client/layout/layout_text.hpp>

#include "./document.hpp"

namespace dom
{
  enum class TreverseOrder
  {
    PreOrder, // Pre-order traversal: root -> left -> right
    PostOrder // Post-order traversal: left -> right -> root
  };

  // The HTML rendering ECS system, which is used to render the HTML document.
  class RenderHTMLDocument final : public builtin_scene::ecs::System,
                                   public client_layout::LayoutViewVisitor
  {
  public:
    RenderHTMLDocument(HTMLDocument *document);

  public:
    const std::string name() const override { return "dom.RenderHTMLDocument"; }
    void onExecute() override;

  private:
    bool onVisitObject(client_layout::LayoutObject &object, int depth) override;
    void onVisitBox(const client_layout::LayoutBox &box, int depth) override;
    void onVisitText(const client_layout::LayoutText &text, int depth) override;
    void renderEntity(const builtin_scene::ecs::EntityId &entity, const client_layout::Fragment &fragment);

    /**
     * Traverse `HTMLElement` or `Text` children from a root node.
     *
     * @param elementOrTextNode The root element or text node.
     * @param elementCallback The callback function for the element node.
     * @param textNodeCallback The callback function for the text node.
     * @param order The traverse order.
     */
    void traverseElementOrTextNode(std::shared_ptr<Node> elementOrTextNode,
                                   std::function<bool(std::shared_ptr<HTMLElement>)> elementCallback,
                                   std::function<void(std::shared_ptr<Text>)> textNodeCallback,
                                   TreverseOrder order);

    // The target width to render the document.
    inline float targetWidth() const
    {
      std::shared_ptr<browser::Window> window = document_->defaultView();
      assert(window != nullptr);
      return window->innerWidth();
    }

    // The target height to render the document.
    inline float targetHeight() const
    {
      std::shared_ptr<browser::Window> window = document_->defaultView();
      assert(window != nullptr);
      return window->innerHeight();
    }

    // The target constraint space to render the document.
    client_layout::ConstraintSpace targetSpace() const
    {
      std::shared_ptr<browser::Window> window = document_->defaultView();
      assert(window != nullptr);
      return client_layout::ConstraintSpace(window->innerWidth(), window->innerHeight());
    }

  private:
    HTMLDocument *document_ = nullptr;
  };
}
