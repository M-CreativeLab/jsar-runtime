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
    using Element::Element;

  public:
    void blur();
    void focus();
    void click();
    std::optional<std::string> getDataset(const std::string &key);
    void setDataset(const std::string &key, const std::string &value);
    void removeDataset(const std::string &key);

  public:
    inline float offsetWidth() const override { return offsetWidth_; }
    inline float &offsetWidth() override { return offsetWidth_; }
    inline float offsetHeight() const override { return offsetHeight_; }
    inline float &offsetHeight() override { return offsetHeight_; }

  public:
    void createdCallback() override;
    void attributeChangedCallback(const std::string &name,
                                  const std::string &oldValue, const std::string &newValue) override;
    void classListChangedCallback(const DOMTokenList &newClassList) override;

  private:
    bool isHTMLElement() const override final { return true; }

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
    std::unordered_map<std::string, std::string> dataset_;
    std::shared_ptr<client_cssom::CSSStyleDeclaration> style_;
  };
}
