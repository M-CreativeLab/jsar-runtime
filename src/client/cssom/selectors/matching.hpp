#pragma once

#include <string>
#include <memory>
#include <crates/bindings.hpp>
#include <client/dom/html_element.hpp>

namespace client_cssom::selectors
{
  /**
   * Check if the element matches the specified selectors.
   *
   * @param selectors The CSS selector list.
   * @param element The element to check.
   * @returns Whether the element matches the selectors.
   */
  bool matchesSelectorList(const crates::css::CSSSelectorList &selectors,
                           const std::shared_ptr<dom::HTMLElement> element);

  /**
   * Check if the element matches the specified selector.
   *
   * @param selector The CSS selector.
   * @param element The element to check.
   * @returns Whether the element matches the selector.
   */
  bool matchesSelector(const crates::css::CSSSelector &selector,
                       const std::shared_ptr<dom::HTMLElement> element);
}
