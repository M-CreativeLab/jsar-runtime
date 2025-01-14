#pragma once

#include <string>
#include <unordered_map>
#include <ostream>
#include <common/utility.hpp>
#include <crates/bindings.hpp>
#include "./color.hpp"

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
        : pdb_(crates::css::CSSPropertyDeclarationBlock::ParseStyleDeclaration(""))
    {
    }
    CSSStyleDeclaration(const std::string &cssText)
        : pdb_(crates::css::CSSPropertyDeclarationBlock::ParseStyleDeclaration(cssText))
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
    operator crates::layout::style::LayoutStyle() const;
    /**
     * Custom the conversion to `std::string`.
     */
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
     * Get the property value as a specific type given a property name.
     *
     * @param propertyName The name of the CSS property.
     * @returns The property value as a specific type such as `Dimension`.
     */
    template <typename T>
      requires(std::is_same_v<T, float> ||
               std::is_integral_v<T> ||
               std::is_same_v<T, Color> ||
               std::is_same_v<T, crates::layout::style::Display> ||
               std::is_same_v<T, crates::layout::style::Position> ||
               std::is_same_v<T, crates::layout::style::Overflow> ||
               std::is_same_v<T, crates::layout::style::Dimension> ||
               std::is_same_v<T, crates::layout::style::LengthPercentageAuto> ||
               std::is_same_v<T, crates::layout::style::LengthPercentage>)
    inline T getPropertyValueAs(const std::string &propertyName) const
    {
      using namespace crates::layout::style;

      const auto &value = getPropertyValue(propertyName);
      if constexpr (std::is_same_v<T, float>)
        return value != "" ? std::stof(value) : 0.0f;
      if constexpr (std::is_integral_v<T>)
        return value != "" ? std::stoi(value) : 0;

      if constexpr (std::is_same_v<T, crates::layout::style::Display>)
      {
        if (value == "block")
          return Display::Block;
        else if (value == "flex")
          return Display::Flex;
        else if (value == "grid")
          return Display::Grid;
        else if (value == "none")
          return Display::None;
        else
          return Display::Block;
      }

      if constexpr (std::is_same_v<T, crates::layout::style::Position>)
        return value == "absolute" ? Position::Absolute : Position::Relative;

      if constexpr (std::is_same_v<T, crates::layout::style::Overflow>)
      {
        if (value == "hidden")
          return Overflow::Hidden;
        else if (value == "scroll")
          return Overflow::Scroll;
        else if (value == "clip")
          return Overflow::Clip;
        else
          return Overflow::Visible;
      }

      if constexpr (std::is_same_v<T, Color> ||
                    std::is_same_v<T, crates::layout::style::Dimension> ||
                    std::is_same_v<T, crates::layout::style::LengthPercentageAuto> ||
                    std::is_same_v<T, crates::layout::style::LengthPercentage>)
        return T(value);

      // NOTE: unreachable
      assert(false);
      return T();
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
    std::shared_ptr<crates::css::CSSPropertyDeclarationBlock> pdb_;
  };
}
