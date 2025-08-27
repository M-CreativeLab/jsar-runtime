#pragma once

#include <string>
#include <memory>
#include <crates/jsbindings/bindings.hpp>

#include "./css_grouping_rule.hpp"
#include "../css_style_declaration.hpp"
#include "../selectors/css_selector_parser.hpp"

namespace client_cssom::rules
{
  /**
   * Alternative implementation of CSSStyleRule that uses native C++ CSS selector parsing
   * instead of the Rust-based parser. This addresses the TODO in the original CSSStyleRule.
   */
  class CSSStyleRuleNative final : public CSSGroupingRule
  {
    friend class client_cssom::CSSRuleList;
    using CSSGroupingRule::CSSGroupingRule;

  public:
    /**
     * Constructor that parses selectors using native C++ parser
     * @param selectorText The CSS selector string to parse
     * @param block The style declaration block
     */
    CSSStyleRuleNative(const std::string &selectorText, CSSStyleDeclaration style)
        : CSSGroupingRule()
        , selectorText_(selectorText)
        , style_(std::move(style))
    {
      // Use C++ CSS parser to parse selectors from text
      auto parsed = selectors::CSSelectorParser::parseSelectors(selectorText);
      if (parsed)
      {
        nativeSelectors_ = std::move(*parsed);
      }
    }

    /**
     * Constructor that works with Rust StyleRule but uses C++ parser for selectors
     * This demonstrates how to replace the Rust parser with C++ parser
     */
    CSSStyleRuleNative(crates::css2::stylesheets::StyleRule &inner)
        : CSSGroupingRule()
        , selectorText_(inner.selectorsText())
        , style_(inner.takeBlock())
    {
      // Use C++ CSS parser to parse from selectorsText() instead of using Rust selectors
      auto parsed = selectors::CSSelectorParser::parseSelectors(selectorText_);
      if (parsed)
      {
        nativeSelectors_ = std::move(*parsed);
      }
    }

  public:
    /**
     * Get the native C++ selector list
     */
    const selectors::SelectorList &nativeSelectors() const
    {
      return nativeSelectors_;
    }

    /**
     * Get the selector text
     */
    std::string selectorText() const
    {
      return selectorText_;
    }

    /**
     * Get the style declaration
     */
    const CSSStyleDeclaration &style() const
    {
      return style_;
    }

    /**
     * Check if an element matches this rule's selectors using native C++ matching
     */
    bool matches(const std::shared_ptr<dom::HTMLElement> element) const
    {
      return selectors::matchesSelectorList(nativeSelectors_, element);
    }

  private:
    std::string selectorText_;
    selectors::SelectorList nativeSelectors_;
    CSSStyleDeclaration style_;
  };

  /**
   * Updated version of the original CSSStyleRule with C++ parser integration
   * This shows how to implement the TODO comment in css_style_rule.hpp
   */
  class CSSStyleRuleUpdated final : public CSSGroupingRule
  {
    friend class client_cssom::CSSRuleList;
    using CSSGroupingRule::CSSGroupingRule;

  public:
    CSSStyleRuleUpdated(crates::css2::stylesheets::StyleRule &inner)
        : CSSGroupingRule()
        , rustSelectors_(inner.selectors())
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
    /**
     * Get the selector list (prefers native C++ implementation)
     */
    const crates::css2::selectors::SelectorList &selectors() const
    {
      // For compatibility, still return Rust selectors
      return rustSelectors_;
    }

    /**
     * Get native C++ selectors if available
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
     */
    bool usesNativeParser() const
    {
      return useNativeParser_;
    }

    /**
     * Match element using the appropriate parser
     */
    bool matches(const std::shared_ptr<dom::HTMLElement> element) const
    {
      if (useNativeParser_)
      {
        return selectors::matchesSelectorList(nativeSelectors_, element);
      }
      else
      {
        return selectors::matchesSelectorList(rustSelectors_, element);
      }
    }

  private:
    crates::css2::selectors::SelectorList rustSelectors_; // Fallback
    selectors::SelectorList nativeSelectors_;             // Native C++
    CSSStyleDeclaration style_;
    std::string selectorText_;
    bool useNativeParser_ = false;
  };
}