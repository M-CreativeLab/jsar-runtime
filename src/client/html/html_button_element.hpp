#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLButtonElement final : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;

  public:
    HTMLButtonElement(std::shared_ptr<Document> ownerDocument)
        : HTMLElement("BUTTON", ownerDocument)
    {
    }

  public:
    // Button-specific properties
    bool disabled = false;
    std::string type = "button";  // submit, reset, button
    std::string form;             // form id
    std::string name;
    std::string value;

  private:
    void createdCallback(bool from_scripting) override;
    void attributeChangedCallback(const std::string &name,
                                  const std::string &oldValue,
                                  const std::string &newValue) override;
  };
}