#pragma once

#include <concepts>
#include <string>
#include <unordered_map>
#include <ostream>
#include <common/utility.hpp>
#include <crates/bindings.hpp>

#include "./types/color.hpp"
#include "./types/direction.hpp"
#include "./types/font_style.hpp"
#include "./types/length.hpp"
#include "./types/length_keywords.hpp"
#include "./types/number_keywords.hpp"
#include "./types/text_align.hpp"
#include "./types/keyword.hpp"

namespace client_cssom
{
  template <typename T>
  concept is_property_value = std::is_same_v<T, types::Color> ||
                              std::is_same_v<T, types::Direction> ||
                              std::is_same_v<T, types::FontStyle> ||
                              std::is_same_v<T, types::FontWeight> ||
                              std::is_same_v<T, types::Length> ||
                              std::is_same_v<T, types::LineWidth> ||
                              std::is_same_v<T, types::TextAlign> ||
                              std::is_same_v<T, crates::layout::style::Dimension> ||
                              std::is_same_v<T, crates::layout::style::LengthPercentageAuto> ||
                              std::is_same_v<T, crates::layout::style::LengthPercentage>;

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
        : pdb_(crates::css::CSSPropertyDeclarationBlock::ParseStyleDeclaration("")),
          cachedCssText_(std::nullopt)
    {
    }
    CSSStyleDeclaration(const std::string &cssText)
        : pdb_(crates::css::CSSPropertyDeclarationBlock::ParseStyleDeclaration(cssText)),
          cachedCssText_(std::nullopt)
    {
    }
    // Reconstruct a new CSSStyleDeclaration from another one's cssText to avoid the `pdb_` being shared.
    CSSStyleDeclaration(const CSSStyleDeclaration &other) : CSSStyleDeclaration(other.cssText())
    {
    }

  public:
    /**
     * Textual representation of the declaration block, if and only if it is exposed via `HTMLElement.style`. Setting this
     * attribute changes the inline style.
     *
     * @returns The textual representation of the declaration block.
     */
    inline std::string cssText() const
    {
      if (!cachedCssText_.has_value())
        cachedCssText_ = pdb_->cssText();
      return cachedCssText_.value();
    }
    /**
     * @returns The number of properties.
     */
    size_t length() const { return pdb_->size(); }

  public:
    /**
     * Custom the conversion to `LayoutStyle`.
     */
    operator crates::layout::style::LayoutStyle() const;
    bool operator==(const CSSStyleDeclaration &other) const { return equals(other); }
    bool operator!=(const CSSStyleDeclaration &other) const { return !equals(other); }
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
     * Check if the declaration block is equal to another one.
     *
     * @param other The other CSSStyleDeclaration to compare.
     * @returns Whether the declaration block is equal to the other one.
     */
    bool equals(const CSSStyleDeclaration &other) const;
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
      requires is_property_value<T> ||
               std::is_same_v<T, float> ||
               std::is_integral_v<T> ||
               std::is_same_v<T, types::BorderStyleKeyword> ||
               std::is_same_v<T, crates::layout::style::Display> ||
               std::is_same_v<T, crates::layout::style::BoxSizing> ||
               std::is_same_v<T, crates::layout::style::Position> ||
               std::is_same_v<T, crates::layout::style::Overflow>
    T getPropertyValueAs(const std::string &propertyName) const
    {
      using namespace crates::layout::style;

      // float, int, etc.
      const auto &value = getPropertyValue(propertyName);
      if constexpr (std::is_same_v<T, float>)
        return value != "" ? std::stof(value) : 0.0f;
      if constexpr (std::is_integral_v<T>)
        return value != "" ? std::stoi(value) : 0;

      // keywords or other enums
      if constexpr (std::is_same_v<T, types::BorderStyleKeyword>)
        return client_cssom::types::parseKeyword<T>(value).value_or(T::kNone);

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

      if constexpr (std::is_same_v<T, crates::layout::style::BoxSizing>)
        return value == "border-box" ? BoxSizing::BorderBox : BoxSizing::ContentBox;

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

      if constexpr (is_property_value<T>)
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
      cachedCssText_ = std::nullopt;
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
      auto value = pdb_->removeProperty(propertyName);
      cachedCssText_ = std::nullopt;
      return value;
    }

  private:
    std::shared_ptr<crates::css::CSSPropertyDeclarationBlock> pdb_;
    mutable std::optional<std::string> cachedCssText_ = std::nullopt;
  };
}
