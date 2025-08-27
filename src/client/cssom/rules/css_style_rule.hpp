#pragma once

#include <string>
#include <memory>
#include <crates/jsbindings/bindings.hpp>

#include "./css_grouping_rule.hpp"
#include "../css_style_declaration.hpp"
#include "../selectors/css_selector_parser.hpp"

namespace client_cssom::rules
{
  class CSSStyleRule final : public CSSGroupingRule
  {
    friend class client_cssom::CSSRuleList;
    using CSSGroupingRule::CSSGroupingRule;

  public:
    CSSStyleRule(crates::css2::stylesheets::StyleRule &inner)
        : CSSGroupingRule()
        , selectors_(inner.selectors())
        , style_(inner.takeBlock())
        , selectorText_(inner.selectorsText())
    {
      // RESOLVED TODO: use C++ css parser to parse from `inner.selectorsText()` instead of current parser from Rust.
      auto parsed = selectors::CSSelectorParser::parseSelectors(selectorText_);
      if (parsed)
      {
        nativeSelectors_ = std::move(*parsed);
        useNativeParser_ = true;
      }
      else
      {
        // Fallback to Rust parser if C++ parser fails
        useNativeParser_ = false;
      }
    }

  public:
    const crates::css2::selectors::SelectorList &selectors() const
    {
      return selectors_;
    }

    /**
     * Get native C++ selectors if available
     * @return Pointer to native selectors, or nullptr if using Rust parser
     */
    const selectors::SelectorList *nativeSelectors() const
    {
      return useNativeParser_ ? &nativeSelectors_ : nullptr;
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
     * Check if this rule uses the native C++ parser
     * @return true if using C++ parser, false if using Rust parser
     */
    bool usesNativeParser() const
    {
      return useNativeParser_;
    }

    /**
     * Check if an element matches this rule's selectors
     * Uses the appropriate parser (C++ preferred, Rust fallback)
     * @param element The element to check
     * @return true if the element matches any selector in this rule
     */
    bool matches(const std::shared_ptr<dom::HTMLElement> element) const
    {
      if (useNativeParser_)
      {
        return selectors::matchesSelectorList(nativeSelectors_, element);
      }
      else
      {
        return selectors::matchesSelectorList(selectors_, element);
      }
    }

  private:
    crates::css2::selectors::SelectorList selectors_; // Original Rust-based selectors (kept for compatibility)
    selectors::SelectorList nativeSelectors_;         // New native C++ selectors
    CSSStyleDeclaration style_;
    std::string selectorText_;
    bool useNativeParser_ = false;
  };
}
