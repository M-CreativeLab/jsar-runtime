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
#include <client/cssom/box_bounding.hpp>

#include "./geometry/dom_box.hpp"
#include "./node.hpp"

namespace dom
{
  class HTMLDocument;
  class SceneObject : virtual public client_cssom::BoxOffset,
                      virtual public client_cssom::BoxBounding
  {
    friend class Node;
    friend class Text;
    friend class Content2d;

  public:
    SceneObject(std::shared_ptr<HTMLDocument> htmlDocument, std::string name);
    SceneObject(const SceneObject &that);
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
    [[nodiscard]] std::shared_ptr<crates::layout2::Node> layoutNode();

  protected:
    template <typename T>
    bool hasSceneComponent() const
    {
      auto sceneRef = scene_.lock();
      if (TR_UNLIKELY(sceneRef == nullptr) || !entity_.has_value())
        return false;
      return sceneRef->hasComponent<T>(entity_.value());
    }
    /**
     * Get the scene's component of the given component type.
     */
    template <typename T>
    std::shared_ptr<T> getSceneComponent() const
    {
      auto sceneRef = scene_.lock();
      if (TR_UNLIKELY(sceneRef == nullptr) || !entity_.has_value())
        return nullptr;
      return sceneRef->getComponent<T>(entity_.value());
    }
    /**
     * Get the scene's component of the given component type, and assert if the component is not found.
     */
    template <typename T>
    T &getSceneComponentChecked() const
    {
      auto sceneRef = scene_.lock();
      assert(sceneRef != nullptr && "The scene must be valid.");
      assert(entity_.has_value() && "The entity must be valid.");
      return sceneRef->getComponentChecked<T>(entity_.value());
    }
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
    bool render(Node &node);
    void renderObject(builtin_scene::Scene &scene, const client_cssom::Layout &layout);
    void connectedCallback(std::shared_ptr<Node> node);
    void disconnectedCallback();

  protected: // Layout methods
    /**
     * Get the element's layout result, and update the offset members, such as `offsetWidth_` and
     * `offsetHeight_`.
     *
     * It also dispatches the `onLayoutSizeChanged` method when the layout size is changed.
     *
     * @returns The layout result.
     */
    [[nodiscard]] client_cssom::Layout fetchLayoutAndDispatchChangeEvent(Node &node);
    /**
     * Adopt the specified style to the element, it will copy the style properties to the element's
     * adopted style, and update the layout node's style.
     *
     * @param style The style to adopt.
     * @returns Whether the layout style is updated successfully.
     */
    bool adoptStyleOn(Node &node, const client_cssom::CSSStyleDeclaration &style);
    // Set the layout style with the name, node, and style.
    bool setLayoutStyle(const Node &node, const client_cssom::CSSStyleDeclaration &style);

  private:
    // Initialize the scene object with the HTML document.
    void initialize(const HTMLDocument& htmlDocument);
    // Check if the scene object should be rendered.
    [[nodiscard]] bool skipRender() const;

  protected:
    bool initialized_ = false;
    std::weak_ptr<builtin_scene::Scene> scene_;
    std::optional<builtin_scene::ecs::EntityId> entity_ = std::nullopt;
    std::shared_ptr<crates::layout2::Allocator> layoutAllocator_ = nullptr;
    std::shared_ptr<crates::layout2::Node> layoutNode_ = nullptr;
    std::optional<client_cssom::Layout> computedLayout_ = std::nullopt;
    client_cssom::CSSStyleDeclaration defaultStyle_;
    client_cssom::CSSStyleDeclaration adoptedStyle_;

  private:
    std::string name_;
  };
}
