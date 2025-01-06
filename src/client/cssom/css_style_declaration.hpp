#pragma once

#include <string>
#include <unordered_map>
#include <ostream>
#include <crates/jsar_jsbindings.h>

namespace client_cssom
{
  enum class CSSPropertyPriority
  {
    Normal,
    Important,
  };

  struct CSSProperty
  {
    std::string name;
    std::string value;
    CSSPropertyPriority priority = CSSPropertyPriority::Normal;
  };

  /**
   * The `CSSStyleDeclaration` interface represents an object that is a CSS declaration block, and exposes style information
   * and various style-related methods and properties.
   */
  class CSSStyleDeclaration
  {
  public:
    CSSStyleDeclaration() = default;
    CSSStyleDeclaration(CSSStyleDeclaration &other)
        : cssText_(other.cssText_), properties_(other.properties_)
    {
    }

  public:
    /**
     * Textual representation of the declaration block, if and only if it is exposed via `HTMLElement.style`. Setting this
     * attribute changes the inline style.
     *
     * @returns The textual representation of the declaration block.
     */
    std::string cssText() const
    {
      return cssText_;
    }
    /**
     * @returns The number of properties.
     */
    size_t length() const
    {
      return properties_.size();
    }

  public:
    /**
     * Custom the conversion to `LayoutStyle`.
     */
    operator crates::jsar::layout::style::LayoutStyle() const;
    friend std::ostream &operator<<(std::ostream &os, const CSSStyleDeclaration &style)
    {
      os << "CSSStyleDeclaration {" << std::endl;
      for (const auto &item : style.properties_)
      {
        const auto &name = item.first;
        const auto &property = item.second;
        os << " " << name << ": " << property.value;
        if (property.priority == CSSPropertyPriority::Important)
          os << " !important";
        os << ";" << std::endl;
      }
      os << "}";
      return os;
    }

  public:
    /**
     * Get the optional priority, "important".
     *
     * @param propertyName The name of the CSS property.
     * @returns The optional priority, "important".
     */
    CSSPropertyPriority getPropertyPriority(const std::string &propertyName) const
    {
      auto it = properties_.find(propertyName);
      if (it != properties_.end())
        return it->second.priority;
      else
        return CSSPropertyPriority::Normal;
    }
    /**
     * Get the property value given a property name.
     *
     * @param propertyName The name of the CSS property.
     * @returns The property value.
     */
    std::string getPropertyValue(const std::string &propertyName) const
    {
      auto it = properties_.find(propertyName);
      if (it != properties_.end())
        return it->second.value;
      else
        return "";
    }
    /**
     * Set a property value and priority within the declaration block.
     *
     * @param propertyName The name of the CSS property.
     * @param value The new value of the property.
     * @param priority The optional priority, "important".
     */
    void setProperty(const std::string &propertyName, const std::string &value,
                     CSSPropertyPriority priority = CSSPropertyPriority::Normal)
    {
      CSSProperty cssProperty;
      cssProperty.name = propertyName;
      cssProperty.value = value;
      cssProperty.priority = priority;
      properties_[propertyName] = cssProperty;
    }
    /**
     * Remove a property from the declaration block.
     *
     * @param propertyName The name of the CSS property.
     */
    void removeProperty(const std::string &propertyName)
    {
      properties_.erase(propertyName);
    }

  private:
    std::string cssText_;
    std::unordered_map<std::string, CSSProperty> properties_;
  };
}
