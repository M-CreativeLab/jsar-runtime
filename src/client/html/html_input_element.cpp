#include <algorithm>
#include <limits>
#include <client/builtin_scene/ecs-inl.hpp>
#include "./html_input_element.hpp"

namespace dom
{
  void HTMLInputElement::createdCallback(bool from_scripting)
  {
    // Set default inline-block display for input elements
    defaultStyle_.setProperty("display", "inline-block");

    // Set default styles based on input type
    std::string input_type = type();
    if (input_type == "text" ||
        input_type == "email" ||
        input_type == "password" ||
        input_type == "number" ||
        input_type == "search" ||
        input_type == "url")
    {
      defaultStyle_.setProperty("padding", "0.25em");
      defaultStyle_.setProperty("border", "1px solid #ccc");
      defaultStyle_.setProperty("background-color", "white");
      defaultStyle_.setProperty("width", "12.5em");
      defaultStyle_.setProperty("height", "1.25em");
    }
    else if (input_type == "checkbox" ||
             input_type == "radio")
    {
      defaultStyle_.setProperty("width", "1em");
      defaultStyle_.setProperty("height", "1em");
    }
    else if (input_type == "button" ||
             input_type == "submit" ||
             input_type == "reset")
    {
      defaultStyle_.setProperty("padding", "0.25em 0.5em");
      defaultStyle_.setProperty("border", "1px solid #ccc");
      defaultStyle_.setProperty("background-color", "#f0f0f0");
      defaultStyle_.setProperty("cursor", "pointer");
    }

    HTMLElement::createdCallback(from_scripting);
  }

  void HTMLInputElement::attributeChangedCallback(const std::string &name,
                                                  const std::string &oldValue,
                                                  const std::string &newValue)
  {
    HTMLElement::attributeChangedCallback(name, oldValue, newValue);

    if (name == "type")
    {
      // Reset value and checked state when type changes
      if (newValue == "checkbox" || newValue == "radio")
      {
        checked_ = hasAttribute("checked");
      }
      else
      {
        checked_ = false;
        if (hasAttribute("value"))
        {
          value_ = getAttribute("value");
        }
      }
    }
    else if (name == "value")
    {
      if (type() != "checkbox" && type() != "radio")
      {
        value_ = newValue;
      }
    }
    else if (name == "checked")
    {
      if (type() == "checkbox" || type() == "radio")
      {
        checked_ = !newValue.empty();
      }
    }
  }

  bool HTMLInputElement::checkValidity() const
  {
    // Check custom validity first
    if (!custom_validity_message_.empty())
    {
      return false;
    }

    // Check required attribute
    if (required())
    {
      std::string input_type = type();
      if (input_type == "checkbox" || input_type == "radio")
      {
        if (!checked_)
          return false;
      }
      else
      {
        if (value_.empty())
          return false;
      }
    }

    // Basic email validation
    if (type() == "email" && !value_.empty())
    {
      // Simple email validation - contains @ and at least one dot after @
      size_t at_pos = value_.find('@');
      if (at_pos == std::string::npos || at_pos == 0 || at_pos == value_.length() - 1)
      {
        return false;
      }
      size_t dot_pos = value_.find('.', at_pos);
      if (dot_pos == std::string::npos || dot_pos == value_.length() - 1)
      {
        return false;
      }
    }

    return true;
  }

  void HTMLInputElement::setCustomValidity(const std::string &error)
  {
    custom_validity_message_ = error;
  }

  bool HTMLInputElement::reportValidity() const
  {
    bool valid = checkValidity();
    // In a real implementation, this would show validation UI
    // For now, just return the validity status
    return valid;
  }

  void HTMLInputElement::stepUp(int steps)
  {
    if (type() == "number")
    {
      double current = valueAsNumber();
      if (!std::isnan(current))
      {
        setValueAsNumber(current + steps);
      }
    }
  }

  void HTMLInputElement::stepDown(int steps)
  {
    if (type() == "number")
    {
      double current = valueAsNumber();
      if (!std::isnan(current))
      {
        setValueAsNumber(current - steps);
      }
    }
  }

  void HTMLInputElement::select()
  {
    // In a real implementation, this would select all text in the input
    // For now, this is a no-op in the spatial web context
  }

  void HTMLInputElement::setSelectionRange(int start, int end)
  {
    // In a real implementation, this would set text selection range
    // For now, this is a no-op in the spatial web context
  }

  double HTMLInputElement::valueAsNumber() const
  {
    if (type() == "number")
    {
      try
      {
        return std::stod(value_);
      }
      catch (const std::exception &)
      {
        return std::numeric_limits<double>::quiet_NaN();
      }
    }
    return std::numeric_limits<double>::quiet_NaN();
  }

  void HTMLInputElement::setValueAsNumber(double value)
  {
    if (type() == "number")
    {
      if (std::isnan(value))
      {
        value_ = "";
      }
      else
      {
        value_ = std::to_string(value);
      }
    }
  }

  void HTMLInputElement::handleClick()
  {
    // Only handle click for checkbox and radio buttons
    std::string input_type = type();
    if (input_type != "checkbox" && input_type != "radio")
      return;

    // Don't handle click if disabled
    if (disabled())
      return;

    if (input_type == "checkbox")
    {
      // Toggle checked state for checkbox
      setChecked(!checked_);
      // Clear indeterminate state when user clicks
      setIndeterminate(false);
    }
    else if (input_type == "radio")
    {
      // For radio buttons, uncheck all other radio buttons with the same name
      // and check this one
      if (!checked_)
      {
        // Find other radio buttons with the same name and uncheck them
        std::string radio_name = name();
        if (!radio_name.empty())
        {
          // TODO: Implement radio group handling
          // For now, just set this radio button as checked
        }
        setChecked(true);
      }
    }

    // Trigger change event
    triggerChangeEvent();
  }

  void HTMLInputElement::triggerChangeEvent()
  {
    // TODO: Implement proper event dispatching
    // For now, this is a placeholder for change event triggering
    // This should create and dispatch a 'change' event
  }

  void HTMLInputElement::click()
  {
    // Handle checkbox/radio interaction first
    handleClick();

    // Then call parent click implementation for any additional behavior
    HTMLElement::click();
  }

  void HTMLInputElement::updateVisualRepresentation()
  {
    // This method will be called by the layout system
    // The actual visual update will be handled by LayoutHTMLInput
    // We just need to mark the layout as needing an update
    // TODO: Add proper layout invalidation mechanism
  }
}
