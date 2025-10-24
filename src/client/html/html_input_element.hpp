#pragma once

#include <string>
#include "./html_element.hpp"

namespace endor
{
  namespace dom
  {
    class HTMLInputElement final : public HTMLElement
    {
    public:
      using HTMLElement::HTMLElement;

    public:
      HTMLInputElement(std::shared_ptr<Document> ownerDocument)
          : HTMLElement("INPUT", ownerDocument)
      {
      }

    public:
      // Standard input properties
      std::string type() const
      {
        return getAttribute("type", "text");
      }
      void setType(const std::string &value)
      {
        setAttribute("type", value);
      }

      std::string value() const
      {
        return value_;
      }
      void setValue(const std::string &value)
      {
        value_ = value;
      }

      bool checked() const
      {
        return checked_;
      }
      void setChecked(bool value)
      {
        checked_ = value;
      }

      bool disabled() const
      {
        return hasAttribute("disabled");
      }
      void setDisabled(bool value)
      {
        if (value)
          setAttribute("disabled", "");
        else
          removeAttribute("disabled");
      }

      bool required() const
      {
        return hasAttribute("required");
      }
      void setRequired(bool value)
      {
        if (value)
          setAttribute("required", "");
        else
          removeAttribute("required");
      }

      std::string placeholder() const
      {
        return getAttribute("placeholder");
      }
      void setPlaceholder(const std::string &value)
      {
        setAttribute("placeholder", value);
      }

      std::string name() const
      {
        return getAttribute("name");
      }
      void setName(const std::string &value)
      {
        setAttribute("name", value);
      }

      // Form validation
      bool checkValidity() const;
      void setCustomValidity(const std::string &error);
      bool reportValidity() const;

      // Number input methods
      void stepUp(int steps = 1);
      void stepDown(int steps = 1);

      // Text selection methods
      void select();
      void setSelectionRange(int start, int end);

      // Value as number for numeric inputs
      double valueAsNumber() const;
      void setValueAsNumber(double value);

    private:
      void createdCallback(bool from_scripting) override;
      void attributeChangedCallback(const std::string &name,
                                    const std::string &oldValue,
                                    const std::string &newValue) override;

    private:
      std::string value_;
      bool checked_ = false;
      std::string custom_validity_message_;
    };
  }
} // namespace endor
