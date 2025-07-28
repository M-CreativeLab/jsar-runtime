#pragma once

#include <string>
#include <optional>
#include <deque>
#include <shared_mutex>

#include <node/uv.h>
#include <crates/bindings.hpp>
#include <client/builtin_scene/ecs-inl.hpp>
#include <client/builtin_scene/scene.hpp>
#include <client/cssom/css_style_declaration.hpp>
#include <client/cssom/box_offset.hpp>
#include <client/dom/element.hpp>

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
    HTMLElement(const HTMLElement &);
    HTMLElement &operator=(const HTMLElement &) = delete;

    void blur();
    void focus();
    void click();
    std::optional<std::string> getDataset(const std::string &key);
    void setDataset(const std::string &key, const std::string &value);
    void removeDataset(const std::string &key);

  public:
    inline float offsetWidth() const override
    {
      return offset_width_;
    }
    inline float &offsetWidth() override
    {
      return offset_width_;
    }
    inline float offsetHeight() const override
    {
      return offset_height_;
    }
    inline float &offsetHeight() override
    {
      return offset_height_;
    }

    // Fetch resource with the given URL asynchronously.
    void fetchArrayBufferLikeResource(const std::string &url,
                                      std::function<void(const void *data, size_t length)> callback);

  public:
    void createdCallback(bool from_scripting) override;
    void attributeChangedCallback(const std::string &name,
                                  const std::string &oldValue,
                                  const std::string &newValue) override;
    void connectedCallback() override;
    void disconnectedCallback() override;

  protected:
    void markAsDirty() override;

  private:
    bool isHTMLElement() const override final
    {
      return true;
    }
    void invalidateStyleCache();

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
    const client_cssom::CSSStyleDeclaration &style() const
    {
      return *style_;
    }
    // The style attribute reference.
    std::shared_ptr<client_cssom::CSSStyleDeclaration> styleRef()
    {
      return style_;
    }

  private:
    float offset_width_ = 0.0f;
    float offset_height_ = 0.0f;

  private:
    std::unordered_map<std::string, std::string> dataset_;
    std::shared_ptr<client_cssom::CSSStyleDeclaration> style_;
  };
}
