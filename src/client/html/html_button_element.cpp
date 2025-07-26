#include <client/builtin_scene/ecs-inl.hpp>
#include "./html_button_element.hpp"

namespace dom
{
  void HTMLButtonElement::createdCallback(bool from_scripting)
  {
    // Set default styles for button element
    defaultStyle_.setProperty("display", "inline-block");
    defaultStyle_.setProperty("text-align", "center");
    defaultStyle_.setProperty("vertical-align", "middle");
    defaultStyle_.setProperty("cursor", "pointer");
    defaultStyle_.setProperty("border", "1px solid #ccc");
    defaultStyle_.setProperty("padding", "6px 12px");
    defaultStyle_.setProperty("margin", "0");
    defaultStyle_.setProperty("font-size", "14px");
    defaultStyle_.setProperty("line-height", "1.42857143");
    defaultStyle_.setProperty("border-radius", "4px");
    defaultStyle_.setProperty("background-color", "#f5f5f5");
    defaultStyle_.setProperty("color", "#333");

    HTMLElement::createdCallback(from_scripting);
  }

  void HTMLButtonElement::attributeChangedCallback(const std::string &name,
                                                   const std::string &oldValue,
                                                   const std::string &newValue)
  {
    if (name == "disabled")
    {
      disabled = !newValue.empty();
      if (disabled)
      {
        styleRef()->setProperty("cursor", "not-allowed");
        styleRef()->setProperty("opacity", "0.65");
      }
      else
      {
        styleRef()->setProperty("cursor", "pointer");
        styleRef()->setProperty("opacity", "1");
      }
    }
    else if (name == "type")
    {
      type = newValue.empty() ? "submit" : newValue;
    }
    else if (name == "form")
    {
      form = newValue;
    }
    else if (name == "name")
    {
      this->name = newValue;  // Use this-> to avoid conflict with parameter name
    }
    else if (name == "value")
    {
      value = newValue;
    }

    HTMLElement::attributeChangedCallback(name, oldValue, newValue);
  }
}