#pragma once

#include <string>
#include <memory>
#include <crates/jsbindings/bindings.hpp>
#include <client/html/html_element.hpp>
#include "css_selector_parser.hpp"

namespace client_cssom::selectors
{
  class MatchingContext
  {
  public:
    MatchingContext() = default;
  };

  // Existing Rust-based API (for compatibility)
  /**
   * Check if the element matches the specified selectors.
   *
   * @param selectors The CSS selector list.
   * @param element The element to check.
   * @returns Whether the element matches the selectors.
   */
  bool matchesSelectorList(const crates::css2::selectors::SelectorList &selectors,
                           const std::shared_ptr<dom::HTMLElement> element);

  /**
   * Check if the element matches the specified selector.
   *
   * @param selector The CSS selector.
   * @param element The element to check.
   * @returns Whether the element matches the selector.
   */
  bool matchesSelector(const crates::css2::selectors::Selector &selector,
                       const std::shared_ptr<dom::HTMLElement> element,
                       MatchingContext &context);

  /**
   * Check if the element matches the specified selector component.
   *
   * @param selector The CSS selector.
   * @param it The iterator of the selector components.
   * @param element The element to check.
   * @returns Whether the element matches the selector component.
   */
  bool matchesSelectorComponent(const crates::css2::selectors::Selector &selector,
                                std::vector<crates::css2::selectors::Component>::const_iterator &it,
                                const shared_ptr<dom::HTMLElement> element,
                                MatchingContext &context);

  // New native C++ API
  /**
   * Check if the element matches the specified native C++ selectors.
   *
   * @param selectors The native CSS selector list.
   * @param element The element to check.
   * @returns Whether the element matches the selectors.
   */
  bool matchesSelectorList(const SelectorList &selectors,
                           const std::shared_ptr<dom::HTMLElement> element);

  /**
   * Check if the element matches the specified native C++ selector.
   *
   * @param selector The native CSS selector.
   * @param element The element to check.
   * @returns Whether the element matches the selector.
   */
  bool matchesSelector(const Selector &selector,
                       const std::shared_ptr<dom::HTMLElement> element,
                       MatchingContext &context);

  /**
   * Check if the element matches the specified native C++ selector component.
   *
   * @param selector The native CSS selector.
   * @param it The iterator of the selector components.
   * @param element The element to check.
   * @returns Whether the element matches the selector component.
   */
  bool matchesSelectorComponent(const Selector &selector,
                                std::vector<Component>::const_iterator &it,
                                const std::shared_ptr<dom::HTMLElement> element,
                                MatchingContext &context);

  /**
   * Check if the element matches the specified native C++ selector component.
   * NOTE: The component should not be a combinator.
   *
   * @param component The CSS selector component.
   * @param element The element to check.
   * @returns Whether the element matches the component.
   */
  bool matchesSelectorComponentNonCombinator(const Component &component,
                                             const std::shared_ptr<dom::HTMLElement> element,
                                             MatchingContext &context);
}
