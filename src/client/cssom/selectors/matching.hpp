#pragma once

#include <string>
#include <crates/bindings.hpp>
#include <client/dom/html_element.hpp>

namespace client_cssom::selectors
{
  /**
   * Check if the element matches the specified selector.
   */
  bool matchesSelector(const crates::css::CSSSelectorList &selectors,
                       const std::shared_ptr<dom::HTMLElement> &element);
}
