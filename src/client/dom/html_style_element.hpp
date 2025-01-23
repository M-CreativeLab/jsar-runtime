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
    void createdCallback() override;
    void connectedCallback() override;

  public:
    bool blocking;
    bool disabled;
    inline const client_cssom::StyleSheet &sheet() const { return *sheet_; }

  private:
    std::unique_ptr<client_cssom::StyleSheet> sheet_;
  };
}
