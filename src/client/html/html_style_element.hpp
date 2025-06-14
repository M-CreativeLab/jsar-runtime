#pragma once

#include <string>
#include <optional>
#include <client/cssom/stylesheet.hpp>
#include "./html_element.hpp"

namespace dom
{
  class HTMLStyleElement : public HTMLElement
  {
    using HTMLElement::HTMLElement;

  public:
    HTMLStyleElement(std::shared_ptr<Document> ownerDocument)
        : HTMLElement("STYLE", ownerDocument)
    {
    }

  public:
    void createdCallback(bool from_scripting) override;
    void connectedCallback() override;

  private:
    bool isRenderable() const override final
    {
      return false;
    }

  public:
    bool blocking = true;
    bool disabled = false;
    inline const client_cssom::StyleSheet &sheet() const
    {
      return *sheet_;
    }

  private:
    std::shared_ptr<client_cssom::StyleSheet> sheet_;
  };
}
