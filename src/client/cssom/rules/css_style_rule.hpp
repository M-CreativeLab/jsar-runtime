#pragma once

#include <string>
#include <memory>
#include <crates/bindings.hpp>

#include "./css_grouping_rule.hpp"
#include "../css_style_declaration.hpp"

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
    {
    }

  public:
    const crates::css2::selectors::SelectorList &selectors() const
    {
      return selectors_;
    }
    std::string selectorText() const
    {
      return static_cast<std::string>(selectors_);
    }
    const CSSStyleDeclaration &style() const
    {
      return style_;
    }

  private:
    crates::css2::selectors::SelectorList selectors_;
    CSSStyleDeclaration style_;
  };
}
