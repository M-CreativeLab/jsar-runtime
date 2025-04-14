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
#include "./types/length_keywords.hpp"
#include "./types/length.hpp"
#include "./types/number_keywords.hpp"
#include "./types/number.hpp"
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
                              std::is_same_v<T, types::LengthPercentage> ||
                              std::is_same_v<T, types::LineWidth> ||
                              std::is_same_v<T, types::NumberLengthPercentage> ||
                              std::is_same_v<T, types::TextAlign> ||
                              std::is_same_v<T, crates::layout2::styles::BoxSizing> ||
                              std::is_same_v<T, crates::layout2::styles::Display> ||
                              std::is_same_v<T, crates::layout2::styles::Dimension> ||
                              std::is_same_v<T, crates::layout2::styles::LengthPercentageAuto> ||
                              std::is_same_v<T, crates::layout2::styles::LengthPercentage> ||
                              std::is_same_v<T, crates::layout2::styles::Overflow> ||
                              std::is_same_v<T, crates::layout2::styles::Position> ||
                              std::is_same_v<T, crates::layout2::styles::AlignItems> ||
                              std::is_same_v<T, crates::layout2::styles::AlignSelf> ||
                              std::is_same_v<T, crates::layout2::styles::AlignContent> ||
                              std::is_same_v<T, crates::layout2::styles::JustifyItems> ||
                              std::is_same_v<T, crates::layout2::styles::JustifySelf> ||
                              std::is_same_v<T, crates::layout2::styles::JustifyContent> ||
                              std::is_same_v<T, crates::layout2::styles::FlexDirection> ||
                              std::is_same_v<T, crates::layout2::styles::FlexWrap>;

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
    using PropertyDeclarationBlock = crates::css2::properties::PropertyDeclarationBlock;

  public:
    CSSStyleDeclaration()
        : pdb_(std::make_shared<PropertyDeclarationBlock>()),
          cachedCssText_(std::nullopt)
    {
    }
    CSSStyleDeclaration(const std::string &cssText)
        : pdb_(PropertyDeclarationBlock::ParseStyleDeclaration(cssText)),
          cachedCssText_(std::nullopt)
    {
    }
    CSSStyleDeclaration(std::unique_ptr<PropertyDeclarationBlock> pdb)
        : pdb_(std::move(pdb)),
          cachedCssText_(std::nullopt)
    {
    }
    CSSStyleDeclaration(const CSSStyleDeclaration &other) : pdb_(other.pdb_),
                                                            cachedCssText_(std::nullopt)
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
    operator crates::layout2::LayoutStyle() const;
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
     * Update the declaration block with another one.
     *
     * If the second argument `omitIfPresent` is set to `true`, it means that only these properties will be updated if they
     * are not existent in the declaration block.
     *
     * @param other The other CSSStyleDeclaration to update.
     * @param omitIfPresent Whether to omit the property if it is already present.
     * @returns Whether the declaration block is updated.
     */
    bool update(const CSSStyleDeclaration &other, bool omitIfPresent = false);
    /**
     * Get the property name at the given index.
     *
     * @param index The index of the CSS property.
     * @returns The property name.
     */
    inline std::string item(size_t index) const
    {
      return pdb_->item(index);
    }
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
     * @param defaultValue The default value to return if the property is not set.
     * @returns The property value.
     */
    inline std::string getPropertyValue(const std::string &propertyName, std::string defaultValue = "") const
    {
      auto value = pdb_->getProperty(propertyName);
      return value != "" ? value : defaultValue;
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
               std::is_same_v<T, types::BorderStyleKeyword>
    T getPropertyValueAs(const std::string &propertyName) const
    {
      using namespace crates::layout2::styles;

      // float, int, etc.
      const auto &value = getPropertyValue(propertyName);
      if constexpr (std::is_same_v<T, float>)
        return value != "" ? std::stof(value) : 0.0f;
      if constexpr (std::is_integral_v<T>)
        return value != "" ? std::stoi(value) : 0;

      // keywords or other enums
      if constexpr (std::is_same_v<T, types::BorderStyleKeyword>)
        return client_cssom::types::parseKeyword<T>(value).value_or(T::kNone);

      // CSSOM types
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
      if (propertyChangedCallback_ != nullptr)
        propertyChangedCallback_(propertyName);
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
      if (propertyChangedCallback_ != nullptr)
        propertyChangedCallback_(propertyName);
      return value;
    }
    /**
     * Set the callback function when a property is changed.
     *
     * @param callback The callback function when a property is changed.
     */
    inline void setPropertyChangedCallback(const std::function<void(const std::string &)> &callback)
    {
      propertyChangedCallback_ = callback;
    }

  private:
    std::shared_ptr<PropertyDeclarationBlock> pdb_;
    std::function<void(const std::string &)> propertyChangedCallback_ = nullptr;
    mutable std::optional<std::string> cachedCssText_ = std::nullopt;
  };
}
