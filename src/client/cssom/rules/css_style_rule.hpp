#pragma once

#include <string>
#include <memory>
#include <crates/bindings.hpp>
#include <client/cssom/css_style_declaration.hpp>
#include <client/cssom/selectors/css_selector_parser.hpp>
#include <client/cssom/selectors/matching.hpp>
#include <client/html/html_element.hpp>

#include "./css_grouping_rule.hpp"

namespace client_cssom::rules
{
  class CSSStyleRule final : public CSSGroupingRule
  {
    friend class client_cssom::CSSRuleList;
    using CSSGroupingRule::CSSGroupingRule;

  public:
    CSSStyleRule(crates::css2::stylesheets::StyleRule &inner)
        : CSSGroupingRule()
        , selectorText_(inner.selectorsText())
        , style_(inner.takeBlock())
    {
      auto parsed = selectors::CSSelectorParser::parseSelectors(selectorText_);
      if (parsed)
      {
        selectors_ = std::move(*parsed);
      }
      // Note: If parsing fails, selectors_ will be empty, which is appropriate
    }

  public:
    const selectors::SelectorList &selectors() const
    {
      return selectors_;
    }

    std::string selectorText() const
    {
      return selectorText_;
    }

    const CSSStyleDeclaration &style() const
    {
      return style_;
    }

    /**
     * Check if an element matches this rule's selectors
     * @param element The element to check
     * @return true if the element matches any selector in this rule
     */
    bool matches(const std::shared_ptr<dom::HTMLElement> element) const
    {
      return selectors::matchesSelectorList(selectors_, element);
    }

  private:
    std::string selectorText_;
    selectors::SelectorList selectors_; // Native C++ selectors (replaces Rust selectors)
    CSSStyleDeclaration style_;
  };
}
