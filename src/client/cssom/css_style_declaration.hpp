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
    CSSStyleDeclaration()
        : pdb_(crates::jsar::css::CSSPropertyDeclarationBlock::ParseStyleDeclaration(""))
    {
    }
    CSSStyleDeclaration(const std::string &cssText)
        : pdb_(crates::jsar::css::CSSPropertyDeclarationBlock::ParseStyleDeclaration(cssText))
    {
    }
    // Reconstruct a new CSSStyleDeclaration from another one's cssText to avoid the `pdb_` being shared.
    CSSStyleDeclaration(CSSStyleDeclaration &other) : CSSStyleDeclaration(other.cssText())
    {
    }

  public:
    /**
     * Textual representation of the declaration block, if and only if it is exposed via `HTMLElement.style`. Setting this
     * attribute changes the inline style.
     *
     * @returns The textual representation of the declaration block.
     */
    inline std::string cssText() const { return pdb_->cssText(); }
    /**
     * @returns The number of properties.
     */
    size_t length() const { return pdb_->size(); }

  public:
    /**
     * Custom the conversion to `LayoutStyle`.
     */
    operator crates::jsar::layout::style::LayoutStyle() const;
    friend std::ostream &operator<<(std::ostream &os, const CSSStyleDeclaration &style)
    {
      os << "CSSStyleDeclaration {" << style.cssText() << "}" << std::endl;
      return os;
    }

  public:
    /**
     * Get the optional priority, "important".
     *
     * @param propertyName The name of the CSS property.
     * @returns The optional priority, "important".
     */
    inline CSSPropertyPriority getPropertyPriority(const std::string &propertyName) const
    {
      if (pdb_->isPropertyImportant(propertyName))
        return CSSPropertyPriority::Important;
      else
        return CSSPropertyPriority::Normal;
    }
    /**
     * Get the property value given a property name.
     *
     * @param propertyName The name of the CSS property.
     * @returns The property value.
     */
    inline std::string getPropertyValue(const std::string &propertyName) const
    {
      return pdb_->getProperty(propertyName);
    }
    /**
     * Check if a property is set.
     * 
     * @param propertyName The name of the CSS property.
     * @returns Whether the property is set.
     */
    inline bool hasProperty(const std::string &propertyName) const
    {
      return pdb_->getProperty(propertyName) != "";
    }
    /**
     * Set a property value and priority within the declaration block.
     *
     * @param propertyName The name of the CSS property.
     * @param value The new value of the property.
     * @param priority The optional priority, "important".
     */
    inline void setProperty(const std::string &propertyName, const std::string &value,
                            CSSPropertyPriority priority = CSSPropertyPriority::Normal)
    {
      pdb_->setProperty(propertyName, value, priority == CSSPropertyPriority::Important);
    }
    /**
     * Set a property value and priority within the declaration block if the property is not already set.
     * 
     * @param propertyName The name of the CSS property.
     * @param value The new value of the property.
     * @param priority The optional priority, "important".
     */
    inline void setPropertyIfNotPresent(const std::string &propertyName, const std::string &value,
                                        CSSPropertyPriority priority = CSSPropertyPriority::Normal)
    {
      if (!hasProperty(propertyName))
        setProperty(propertyName, value, priority);
    }
    /**
     * Remove a property from the declaration block.
     *
     * @param propertyName The name of the CSS property.
     * @returns The removed property value.
     */
    std::string removeProperty(const std::string &propertyName)
    {
      return pdb_->removeProperty(propertyName);
    }

  private:
    std::shared_ptr<crates::jsar::css::CSSPropertyDeclarationBlock> pdb_;
  };
}
