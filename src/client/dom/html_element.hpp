#pragma once

#include <string>
#include <optional>
#include <client/builtin_scene/ecs-inl.hpp>
#include <client/builtin_scene/scene.hpp>
#include <client/cssom/css_style_declaration.hpp>
#include "./element.hpp"

namespace dom
{
  /**
   * The directionality of the element to render content or text.
   */
  enum class HTMLElementDirection
  {
    LTR,
    RTL,
    Auto
  };

  class HTMLElement : public Element
  {
    friend class RenderHTMLDocument;

  public:
    using Element::Element;

  public:
    void blur();
    void focus();
    void click();

  public:
    void connectedCallback() override;

  protected:
    /**
     * Render the element to the scene.
     * 
     * @param scene The scene to render the element.
     */
    virtual void renderElement(builtin_scene::Scene &scene);
    /**
     * Get the scene of this document to draw the element.
     */
    std::shared_ptr<builtin_scene::Scene> scene();

  public:
    HTMLElementDirection dir = HTMLElementDirection::LTR;
    bool draggable = false;
    bool hidden = false;
    std::string innerText;
    std::string lang;
    std::string nonce;
    std::string outerText;
    std::string title;
    std::string translate;
    client_cssom::CSSStyleDeclaration style;

  protected:
    std::optional<builtin_scene::ecs::EntityId> entity_ = std::nullopt;
    /**
     * The adopted style of the element, which is used to apply the CSS styles.
     */
    client_cssom::CSSStyleDeclaration adoptedStyle_;
  };
}
