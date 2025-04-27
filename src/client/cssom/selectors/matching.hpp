#pragma once

#include <string>
#include <memory>
#include <crates/bindings.hpp>
#include <client/html/html_element.hpp>

namespace client_cssom::selectors
{
  class MatchingContext
  {
  public:
    MatchingContext() = default;
  };

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
}
