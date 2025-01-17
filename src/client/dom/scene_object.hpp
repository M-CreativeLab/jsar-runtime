#pragma once

#include <functional>
#include <optional>
#include <memory>
#include <crates/bindings.hpp>
#include <client/builtin_scene/scene.hpp>
#include <client/builtin_scene/ecs-inl.hpp>
#include <client/cssom/css_style_declaration.hpp>
#include <client/cssom/layout.hpp>
#include <client/cssom/box_offset.hpp>
#include "./node.hpp"

namespace dom
{
  class HTMLDocument;
  class SceneObject : virtual public client_cssom::BoxOffset
  {
    friend class Content2d;

  public:
    SceneObject(std::shared_ptr<HTMLDocument> htmlDocument, std::string name);
    SceneObject(SceneObject &that);
    virtual ~SceneObject() = default;

  public:
    /**
     * When the layout size is changed, this method will be called.
     */
    virtual void onLayoutChanged() {};
    /**
     * When the adopted style is changed, this method will be called.
     */
    virtual void onAdoptedStyleChanged() {};

  public:
    /**
     * Get the layout node reference of the element.
     */
    std::shared_ptr<crates::layout::Node> layoutNode();

  protected:
    /**
     * A utility method to use the scene weak reference safely.
     * 
     * @param callback The callback function to use the scene.
     */
    void useScene(const std::function<void(builtin_scene::Scene &)> &callback);
    /**
     * Render the scene object.
     * 
     * @returns Whether the scene object is rendered successfully.
     */
    bool render();
    void renderObject(builtin_scene::Scene &scene, const client_cssom::Layout &layout);
    void connectedCallback(const Node &node);

  protected: // Layout methods
    /**
     * Get the element's layout result, and update the offset members, such as `offsetWidth_` and
     * `offsetHeight_`.
     *
     * It also dispatches the `onLayoutSizeChanged` method when the layout size is changed.
     *
     * @returns The layout result.
     */
    [[nodiscard]] client_cssom::Layout fetchLayoutAndDispatchChangeEvent();
    /**
     * Adopt the specified style to the element, it will copy the style properties to the element's
     * adopted style, and update the layout node's style.
     *
     * @param style The style to adopt.
     * @returns Whether the layout style is updated successfully.
     */
    bool adoptStyleOn(Node &node, client_cssom::CSSStyleDeclaration &style);

  protected:
    std::weak_ptr<builtin_scene::Scene> scene_;
    std::optional<builtin_scene::ecs::EntityId> entity_ = std::nullopt;
    std::shared_ptr<crates::layout::Allocator> layoutAllocator_ = nullptr;
    std::shared_ptr<crates::layout::Node> layoutNode_ = nullptr;
    client_cssom::CSSStyleDeclaration adoptedStyle_;

  private:
    std::string name_;
  };
}
