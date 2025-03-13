#pragma once

#include <string>
#include <optional>
#include <crates/bindings.hpp>
#include <client/builtin_scene/ecs-inl.hpp>
#include <client/builtin_scene/scene.hpp>
#include <client/cssom/css_style_declaration.hpp>
#include <client/cssom/box_offset.hpp>
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

  class HTMLElement : public Element,
                      virtual public client_cssom::BoxOffset
  {
    friend class RenderHTMLDocument;

  public:
    using Element::Element;

  public:
    void blur();
    void focus();
    void click();

  public:
    inline float offsetWidth() const override { return offsetWidth_; }
    inline float &offsetWidth() override { return offsetWidth_; }
    inline float offsetHeight() const override { return offsetHeight_; }
    inline float &offsetHeight() override { return offsetHeight_; }

  public:
    void createdCallback() override;

  protected:
    /**
     * Render the element to the scene.
     *
     * @param scene The scene to render the element.
     */
    virtual bool renderElement(builtin_scene::Scene &scene) { return true; };
    /**
     * Adopt the specified style to the element, it will copy the style properties to the element's
     * adopted style, and update the layout node's style.
     *
     * @param style The style to adopt.
     * @returns Whether the layout style is updated successfully.
     */
    virtual bool adoptStyle(const client_cssom::CSSStyleDeclaration &style) { return true; };

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
    // The style attribute.
    const client_cssom::CSSStyleDeclaration &style() const { return *style_; }
    // The style attribute reference.
    std::shared_ptr<client_cssom::CSSStyleDeclaration> styleRef() { return style_; }

  private:
    float offsetWidth_ = 0.0f;
    float offsetHeight_ = 0.0f;

  private:
    std::shared_ptr<client_cssom::CSSStyleDeclaration> style_;
  };
}
